#include "Enemy.h"

#include "Player.h"
#include "World.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace
{
constexpr float HealthBarHeight = 5.f;
constexpr float SupportFlashDuration = 0.5f;

sf::Vector2f normalizedOr(sf::Vector2f direction, sf::Vector2f fallback)
{
	const float lengthSquared = direction.x * direction.x + direction.y * direction.y;
	if (lengthSquared <= 0.0001f)
	{
		return fallback;
	}

	const float inverseLength = 1.f / std::sqrt(lengthSquared);
	return {direction.x * inverseLength, direction.y * inverseLength};
}

sf::Vector2f rotate(sf::Vector2f direction, float radians)
{
	const float cosine = std::cos(radians);
	const float sine = std::sin(radians);
	return {
		direction.x * cosine - direction.y * sine,
		direction.x * sine + direction.y * cosine
	};
}
}

Enemy::Enemy(sf::Vector2f position, EnemyArchetype archetype)
	: m_archetype(archetype),
	  m_position(position),
	  m_health(getEnemyDefinition(archetype).maximumHealth),
	  m_body(getEnemyDefinition(archetype).radius)
{
	const EnemyDefinition& definition = getEnemyDefinition(m_archetype);
	const float radius = definition.radius;
	const int seed = static_cast<int>(std::abs(position.x) + std::abs(position.y));
	m_strafeSign = (seed % 2 == 0) ? 1 : -1;

	m_body.setOrigin({radius, radius});
	m_body.setPosition(m_position);
	m_body.setOutlineThickness(3.f);
	updateAppearance();
}

EnemyArchetype Enemy::getArchetype() const
{
	return m_archetype;
}

const EnemyDefinition& Enemy::getDefinition() const
{
	return getEnemyDefinition(m_archetype);
}

float Enemy::getRadius() const
{
	return getDefinition().radius;
}

EnemyState Enemy::getState() const
{
	return m_state;
}

sf::Vector2f Enemy::getPosition() const
{
	return m_position;
}

float Enemy::getCurrentHealth() const
{
	return m_health.getCurrentHealth();
}

float Enemy::getMaximumHealth() const
{
	return m_health.getMaximumHealth();
}

bool Enemy::isAlive() const
{
	return m_health.isAlive();
}

bool Enemy::isMarked() const
{
	return m_statusEffects.isMarked();
}

float Enemy::getMarkTimeRemaining() const
{
	return m_statusEffects.getMarkTimeRemaining();
}

float Enemy::distanceTo(sf::Vector2f target) const
{
	const sf::Vector2f offset = target - m_position;
	return std::sqrt(offset.x * offset.x + offset.y * offset.y);
}

EnemyAction Enemy::update(float deltaSeconds, sf::Vector2f targetPosition, const World& world, bool targetAlive)
{
	EnemyAction action;

	m_contactCooldown = std::max(0.f, m_contactCooldown - deltaSeconds);
	m_attackCooldown = std::max(0.f, m_attackCooldown - deltaSeconds);
	m_supportCooldown = std::max(0.f, m_supportCooldown - deltaSeconds);
	m_supportFlash = std::max(0.f, m_supportFlash - deltaSeconds);
	m_statusEffects.update(deltaSeconds);

	if (!isAlive() || !targetAlive)
	{
		m_state = EnemyState::Idle;
		updateAppearance();
		return action;
	}

	const EnemyDefinition& definition = getDefinition();
	const sf::Vector2f toTarget = targetPosition - m_position;
	const float distance = distanceTo(targetPosition);
	const float avoidProbe = getRadius() + 120.f;
	const float chaseProbe = std::min(distance, avoidProbe);

	switch (definition.behavior)
	{
		case EnemyBehavior::Melee:
		{
			const float stopDistance = getRadius() + Player::Radius - 4.f;
			if (distance > stopDistance)
			{
				m_state = EnemyState::Chase;
				steer(toTarget, definition.moveSpeed, deltaSeconds, chaseProbe, world);
			}
			else
			{
				m_state = EnemyState::Attack;
			}
			break;
		}

		case EnemyBehavior::Ranged:
		{
			const bool hasSight = world.hasLineOfSight(m_position, targetPosition);
			const float retreatDistance = definition.preferredRange * 0.65f;

			if (!hasSight)
			{
				m_state = EnemyState::Chase;
				steer(toTarget, definition.moveSpeed, deltaSeconds, chaseProbe, world);
			}
			else if (distance < retreatDistance)
			{
				m_state = EnemyState::Reposition;
				steer(-toTarget, definition.moveSpeed * 1.05f, deltaSeconds, avoidProbe, world);
			}
			else if (distance > definition.attackRange)
			{
				m_state = EnemyState::Chase;
				steer(toTarget, definition.moveSpeed, deltaSeconds, chaseProbe, world);
			}
			else
			{
				m_state = EnemyState::Attack;
				const sf::Vector2f strafe =
					normalizedOr({-toTarget.y, toTarget.x}, {1.f, 0.f}) * static_cast<float>(m_strafeSign);
				steer(strafe, definition.moveSpeed * 0.45f, deltaSeconds, avoidProbe * 0.65f, world);

				if (m_attackCooldown <= 0.f && definition.projectileDamage > 0.f &&
					definition.projectileSpeed > 0.f && definition.projectileRange > 0.f)
				{
					action.fireProjectile = true;
					action.fireDirection = normalizedOr(toTarget, {1.f, 0.f});
					m_attackCooldown = definition.attackCooldown;
				}
			}
			break;
		}

		case EnemyBehavior::Support:
		{
			constexpr float SafeDistance = 240.f;
			constexpr float FollowDistance = 360.f;

			if (distance < SafeDistance)
			{
				m_state = EnemyState::Reposition;
				steer(-toTarget, definition.moveSpeed, deltaSeconds, avoidProbe, world);
			}
			else if (distance > FollowDistance)
			{
				m_state = EnemyState::Chase;
				steer(toTarget, definition.moveSpeed, deltaSeconds, chaseProbe, world);
			}
			else
			{
				m_state = EnemyState::Support;
				const sf::Vector2f strafe =
					normalizedOr({-toTarget.y, toTarget.x}, {1.f, 0.f}) * static_cast<float>(m_strafeSign);
				steer(strafe, definition.moveSpeed * 0.35f, deltaSeconds, avoidProbe * 0.65f, world);
			}

			if (m_supportCooldown <= 0.f && definition.supportHeal > 0.f && definition.supportRadius > 0.f)
			{
				action.supportPulse = true;
				m_supportCooldown = definition.supportCooldown;
				m_supportFlash = SupportFlashDuration;
			}
			break;
		}
	}

	updateAppearance();
	return action;
}

void Enemy::steer(sf::Vector2f desiredDirection, float speed, float deltaSeconds, float probeDistance, const World& world)
{
	if (speed <= 0.f || deltaSeconds <= 0.f)
	{
		return;
	}

	const float radius = getRadius();
	const sf::Vector2f desired = normalizedOr(desiredDirection, {1.f, 0.f});
	const float lookAhead = std::max(radius + 16.f, probeDistance);
	sf::Vector2f direction = desired;

	const auto pathIsClear = [this, &world, radius, lookAhead](sf::Vector2f probeDirection)
	{
		return world.hasClearPath(m_position, m_position + probeDirection * lookAhead, radius);
	};

	if (pathIsClear(desired))
	{
		m_detourTime = 0.f;
	}
	else
	{
		if (m_detourTime <= 0.f)
		{
			constexpr float TurnAngles[] = {0.7853981634f, 1.5707963268f, 2.3561944902f};
			const float preference = static_cast<float>(m_strafeSign);
			sf::Vector2f chosen = desired;

			bool found = false;
			for (const float turnSign : {preference, -preference})
			{
				for (const float angle : TurnAngles)
				{
					const sf::Vector2f candidate = rotate(desired, turnSign * angle);
					if (pathIsClear(candidate))
					{
						chosen = candidate;
						found = true;
						break;
					}
				}

				if (found)
				{
					break;
				}
			}

			m_detourDirection = chosen;
			m_detourTime = 0.6f;
		}

		if (m_detourTime > 0.f && pathIsClear(m_detourDirection))
		{
			direction = m_detourDirection;
		}
	}

	m_detourTime = std::max(0.f, m_detourTime - deltaSeconds);

	const sf::Vector2f displacement{
		direction.x * speed * deltaSeconds,
		direction.y * speed * deltaSeconds
	};
	m_position = world.moveWithCollisions(m_position, displacement, radius);
	m_body.setPosition(m_position);
}

void Enemy::applyDamage(float amount, float markedDamageMultiplier)
{
	const float reduction = std::clamp(getDefinition().damageReduction, 0.f, 0.95f);
	const float reducedAmount = std::max(0.f, amount) * (1.f - reduction);
	m_health.applyDamage(m_statusEffects.calculateDamage(reducedAmount, markedDamageMultiplier));
}

void Enemy::heal(float amount)
{
	if (isAlive() && amount > 0.f)
	{
		m_health.heal(amount);
	}
}

void Enemy::applyAbyssalMark(float durationSeconds)
{
	m_statusEffects.applyAbyssalMark(durationSeconds);
	updateAppearance();
}

bool Enemy::tryContactAttack()
{
	if (!isAlive() || m_contactCooldown > 0.f)
	{
		return false;
	}

	m_contactCooldown = getDefinition().contactCooldown;
	return true;
}

void Enemy::pushApart(sf::Vector2f displacement, const World& world)
{
	m_position = world.moveWithCollisions(m_position, displacement, getRadius());
	m_body.setPosition(m_position);
}

void Enemy::updateAppearance()
{
	const EnemyDefinition& definition = getDefinition();
	if (m_statusEffects.isMarked())
	{
		m_body.setFillColor(sf::Color(120, 76, 158));
		m_body.setOutlineColor(sf::Color(202, 151, 238));
	}
	else
	{
		m_body.setFillColor(definition.fillColor);
		m_body.setOutlineColor(m_supportFlash > 0.f ? sf::Color(140, 232, 170) : definition.outlineColor);
	}
}

void Enemy::render(sf::RenderTarget& target) const
{
	const EnemyDefinition& definition = getDefinition();

	if (m_supportFlash > 0.f && definition.supportRadius > 0.f)
	{
		const float ratio = m_supportFlash / SupportFlashDuration;
		sf::CircleShape pulse(definition.supportRadius);
		pulse.setOrigin({definition.supportRadius, definition.supportRadius});
		pulse.setPosition(m_position);
		pulse.setFillColor(sf::Color::Transparent);
		pulse.setOutlineColor(sf::Color(140, 232, 170, static_cast<std::uint8_t>(110.f * ratio)));
		pulse.setOutlineThickness(3.f);
		target.draw(pulse);
	}

	target.draw(m_body);

	const float barWidth = std::max(36.f, getRadius() * 2.2f);
	const sf::Vector2f barPosition(m_position.x - barWidth / 2.f, m_position.y - getRadius() - 14.f);
	sf::RectangleShape background({barWidth, HealthBarHeight});
	background.setPosition(barPosition);
	background.setFillColor(sf::Color(55, 34, 40));
	target.draw(background);

	sf::RectangleShape fill({
		barWidth * (m_health.getCurrentHealth() / m_health.getMaximumHealth()),
		HealthBarHeight
	});
	fill.setPosition(barPosition);
	fill.setFillColor(sf::Color(185, 66, 75));
	target.draw(fill);
}
