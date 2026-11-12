#include "Enemy.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <algorithm>
#include <cmath>

namespace
{
constexpr float HealthBarWidth = 48.f;
constexpr float HealthBarHeight = 5.f;
}

Enemy::Enemy(sf::Vector2f position)
	: m_position(position),
	  m_health(MaximumHealth),
	  m_body(Radius)
{
	m_body.setOrigin({Radius, Radius});
	m_body.setFillColor(sf::Color(132, 52, 65));
	m_body.setOutlineColor(sf::Color(202, 91, 94));
	m_body.setOutlineThickness(3.f);
	m_body.setPosition(m_position);
	updateAppearance();
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

void Enemy::update(float deltaSeconds, sf::Vector2f targetPosition)
{
	m_contactCooldown = std::max(0.f, m_contactCooldown - deltaSeconds);
	m_statusEffects.update(deltaSeconds);
	updateAppearance();

	const sf::Vector2f offset = targetPosition - m_position;
	const float distanceSquared = offset.x * offset.x + offset.y * offset.y;
	if (distanceSquared > 1.f)
	{
		const float inverseDistance = 1.f / std::sqrt(distanceSquared);
		m_position.x += offset.x * inverseDistance * MoveSpeed * deltaSeconds;
		m_position.y += offset.y * inverseDistance * MoveSpeed * deltaSeconds;
		m_body.setPosition(m_position);
	}
}

void Enemy::applyDamage(float amount, float markedDamageMultiplier)
{
	m_health.applyDamage(m_statusEffects.calculateDamage(amount, markedDamageMultiplier));
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

	m_contactCooldown = ContactCooldown;
	return true;
}

void Enemy::updateAppearance()
{
	m_body.setFillColor(m_statusEffects.isMarked() ? sf::Color(120, 76, 158) : sf::Color(132, 52, 65));
	m_body.setOutlineColor(m_statusEffects.isMarked() ? sf::Color(202, 151, 238) : sf::Color(202, 91, 94));
}

void Enemy::render(sf::RenderTarget& target) const
{
	target.draw(m_body);

	const sf::Vector2f barPosition(m_position.x - HealthBarWidth / 2.f, m_position.y - Radius - 14.f);
	sf::RectangleShape background({HealthBarWidth, HealthBarHeight});
	background.setPosition(barPosition);
	background.setFillColor(sf::Color(55, 34, 40));
	target.draw(background);

	sf::RectangleShape fill({HealthBarWidth * (m_health.getCurrentHealth() / m_health.getMaximumHealth()), HealthBarHeight});
	fill.setPosition(barPosition);
	fill.setFillColor(sf::Color(185, 66, 75));
	target.draw(fill);
}
