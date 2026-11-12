#include "CombatSystem.h"

#include "Player.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

#include <algorithm>
#include <cmath>

namespace
{
sf::Vector2f normalized(sf::Vector2f direction, sf::Vector2f fallback)
{
	const float lengthSquared = direction.x * direction.x + direction.y * direction.y;
	if (lengthSquared <= 0.0001f)
	{
		return fallback;
	}

	const float inverseLength = 1.f / std::sqrt(lengthSquared);
	return {direction.x * inverseLength, direction.y * inverseLength};
}

float distanceSquared(sf::Vector2f first, sf::Vector2f second)
{
	const sf::Vector2f difference = first - second;
	return difference.x * difference.x + difference.y * difference.y;
}
}

CombatSystem::CombatSystem(sf::Vector2f enemyStartPosition)
	: m_enemies{Enemy(enemyStartPosition)}
{
}

void CombatSystem::update(float deltaSeconds, Player& player)
{
	m_meleeCooldown = std::max(0.f, m_meleeCooldown - deltaSeconds);
	m_projectileCooldown = std::max(0.f, m_projectileCooldown - deltaSeconds);
	m_meleeFlash = std::max(0.f, m_meleeFlash - deltaSeconds);

	for (Enemy& enemy : m_enemies)
	{
		enemy.update(deltaSeconds, player.getPosition());
	}

	for (auto projectile = m_projectiles.begin(); projectile != m_projectiles.end();)
	{
		projectile->update(deltaSeconds);
		bool hit = false;
		for (Enemy& enemy : m_enemies)
		{
			if (enemy.isAlive() && projectile->intersectsCircle(enemy.getPosition(), Enemy::Radius))
			{
				enemy.applyDamage(Projectile::Damage);
				hit = true;
				break;
			}
		}

		if (hit || projectile->isExpired())
		{
			projectile = m_projectiles.erase(projectile);
		}
		else
		{
			++projectile;
		}
	}

	for (Enemy& enemy : m_enemies)
	{
		if (enemy.isAlive() && distanceSquared(player.getPosition(), enemy.getPosition()) <=
			(Player::Radius + Enemy::Radius) * (Player::Radius + Enemy::Radius) && enemy.tryContactAttack())
		{
			player.applyDamage(Enemy::ContactDamage);
		}
	}

	m_enemies.erase(
		std::remove_if(m_enemies.begin(), m_enemies.end(), [](const Enemy& enemy) { return !enemy.isAlive(); }),
		m_enemies.end());
}

bool CombatSystem::tryMelee(Player& player, sf::Vector2f aimWorldPosition)
{
	if (m_meleeCooldown > 0.f)
	{
		return false;
	}

	m_meleeDirection = normalized(aimWorldPosition - player.getPosition(), player.getAimDirection());
	m_meleeOrigin = player.getPosition();
	player.setAimDirection(m_meleeDirection);
	m_meleeCooldown = MeleeCooldown;
	m_meleeFlash = 0.16f;

	const float maximumDistance = MeleeRange + Enemy::Radius;
	for (Enemy& enemy : m_enemies)
	{
		if (!enemy.isAlive())
		{
			continue;
		}

		const sf::Vector2f toEnemy = enemy.getPosition() - player.getPosition();
		const float distance = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
		if (distance <= maximumDistance && distance > 0.f)
		{
			const sf::Vector2f directionToEnemy(toEnemy.x / distance, toEnemy.y / distance);
			const float dot = m_meleeDirection.x * directionToEnemy.x + m_meleeDirection.y * directionToEnemy.y;
			if (dot >= MeleeArcDotThreshold)
			{
				enemy.applyDamage(MeleeDamage);
			}
		}
	}

	return true;
}

bool CombatSystem::tryShoot(const Player& player, sf::Vector2f aimWorldPosition)
{
	if (m_projectileCooldown > 0.f)
	{
		return false;
	}

	const sf::Vector2f direction = normalized(aimWorldPosition - player.getPosition(), player.getAimDirection());
	const sf::Vector2f spawnPosition(
		player.getPosition().x + direction.x * (Player::Radius + Projectile::Radius + 3.f),
		player.getPosition().y + direction.y * (Player::Radius + Projectile::Radius + 3.f));
	m_projectiles.emplace_back(spawnPosition, direction);
	m_projectileCooldown = ProjectileCooldown;
	return true;
}

void CombatSystem::render(sf::RenderTarget& target) const
{
	for (const Enemy& enemy : m_enemies)
	{
		enemy.render(target);
	}

	for (const Projectile& projectile : m_projectiles)
	{
		projectile.render(target);
	}

	if (m_meleeFlash > 0.f)
	{
		constexpr float HalfArcRadians = 1.0471975512f;
		constexpr float ArcLength = MeleeRange;
		constexpr int ArcSegments = 12;

		sf::ConvexShape arc;
		arc.setPointCount(static_cast<std::size_t>(ArcSegments + 2));
		arc.setPoint(0, {0.f, 0.f});
		const float baseAngle = std::atan2(m_meleeDirection.y, m_meleeDirection.x);
		for (int index = 0; index <= ArcSegments; ++index)
		{
			const float angle = baseAngle - HalfArcRadians + (2.f * HalfArcRadians * index / ArcSegments);
			arc.setPoint(static_cast<std::size_t>(index + 1), {
				std::cos(angle) * ArcLength,
				std::sin(angle) * ArcLength
			});
		}
		arc.setPosition(m_meleeOrigin);
		arc.setFillColor(sf::Color(236, 174, 103, 110));
		target.draw(arc);
	}
}

std::size_t CombatSystem::getEnemyCount() const
{
	return m_enemies.size();
}

const Enemy& CombatSystem::getEnemy(std::size_t index) const
{
	return m_enemies.at(index);
}

std::size_t CombatSystem::getProjectileCount() const
{
	return m_projectiles.size();
}
