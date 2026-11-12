#include "CombatSystem.h"

#include "Player.h"
#include "World.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

#include <algorithm>
#include <cmath>
#include <optional>

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

void CombatSystem::update(float deltaSeconds, Player& player, const World& world)
{
	m_meleeCooldown = std::max(0.f, m_meleeCooldown - deltaSeconds);
	m_projectileCooldown = std::max(0.f, m_projectileCooldown - deltaSeconds);
	m_abyssalMarkCooldown = std::max(0.f, m_abyssalMarkCooldown - deltaSeconds);
	m_meleeFlash = std::max(0.f, m_meleeFlash - deltaSeconds);

	for (Enemy& enemy : m_enemies)
	{
		enemy.update(deltaSeconds, player.getPosition());
	}

	for (auto projectile = m_projectiles.begin(); projectile != m_projectiles.end();)
	{
		projectile->update(deltaSeconds);

		float nearestHitFraction = 2.f;
		Enemy* hitEnemy = nullptr;

		for (Enemy& enemy : m_enemies)
		{
			if (!enemy.isAlive())
			{
				continue;
			}

			const std::optional<float> hitFraction =
				projectile->getCircleHitFraction(enemy.getPosition(), Enemy::Radius);
			if (hitFraction && *hitFraction < nearestHitFraction)
			{
				nearestHitFraction = *hitFraction;
				hitEnemy = &enemy;
			}
		}

		bool hitWall = false;
		for (const sf::FloatRect& obstacle : world.getObstacles())
		{
			const std::optional<float> hitFraction = projectile->getRectHitFraction(obstacle);
			if (hitFraction && *hitFraction < nearestHitFraction)
			{
				nearestHitFraction = *hitFraction;
				hitEnemy = nullptr;
				hitWall = true;
			}
		}

		if (hitEnemy != nullptr)
		{
			hitEnemy->applyDamage(projectile->getDamage(), projectile->getMarkedDamageMultiplier());
		}

		if (hitWall || hitEnemy != nullptr || projectile->isExpired())
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

	const WeaponDefinition& weapon = m_inventory.getEquippedWeapon();
	m_meleeDirection = normalized(aimWorldPosition - player.getPosition(), player.getAimDirection());
	m_meleeOrigin = player.getPosition();
	player.setAimDirection(m_meleeDirection);
	m_meleeCooldown = weapon.meleeCooldown;
	m_meleeFlash = 0.16f;

	const float maximumDistance = weapon.meleeRange + Enemy::Radius;
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
				enemy.applyDamage(weapon.meleeDamage, weapon.markedDamageMultiplier);
			}
		}
	}

	return true;
}

bool CombatSystem::tryShoot(const Player& player, sf::Vector2f aimWorldPosition)
{
	const WeaponDefinition& weapon = m_inventory.getEquippedWeapon();
	if (m_projectileCooldown > 0.f || weapon.projectileDamage <= 0.f || weapon.projectileSpeed <= 0.f)
	{
		return false;
	}

	const sf::Vector2f direction = normalized(aimWorldPosition - player.getPosition(), player.getAimDirection());
	const sf::Vector2f spawnPosition(
		player.getPosition().x + direction.x * (Player::Radius + Projectile::Radius + 3.f),
		player.getPosition().y + direction.y * (Player::Radius + Projectile::Radius + 3.f));
	m_projectiles.emplace_back(
		spawnPosition,
		direction,
		weapon.projectileSpeed,
		weapon.projectileRange,
		weapon.projectileDamage,
		weapon.markedDamageMultiplier);
	m_projectileCooldown = weapon.projectileCooldown;
	return true;
}

bool CombatSystem::tryAbyssalMark(sf::Vector2f playerPosition, sf::Vector2f aimWorldPosition)
{
	if (m_abyssalMarkCooldown > 0.f)
	{
		return false;
	}

	const sf::Vector2f aimDirection = normalized(aimWorldPosition - playerPosition, {1.f, 0.f});
	const float maximumDistanceSquared = AbyssalMarkRange * AbyssalMarkRange;
	Enemy* target = nullptr;
	float nearestDistanceSquared = maximumDistanceSquared;

	for (Enemy& enemy : m_enemies)
	{
		if (!enemy.isAlive())
		{
			continue;
		}

		const sf::Vector2f toEnemy = enemy.getPosition() - playerPosition;
		const float distanceSquaredToEnemy = distanceSquared(playerPosition, enemy.getPosition());
		if (distanceSquaredToEnemy > nearestDistanceSquared || distanceSquaredToEnemy <= 0.f)
		{
			continue;
		}

		const float inverseDistance = 1.f / std::sqrt(distanceSquaredToEnemy);
		const float facingDot = (toEnemy.x * inverseDistance) * aimDirection.x +
			(toEnemy.y * inverseDistance) * aimDirection.y;
		if (facingDot >= MeleeArcDotThreshold)
		{
			nearestDistanceSquared = distanceSquaredToEnemy;
			target = &enemy;
		}
	}

	if (target == nullptr)
	{
		return false;
	}

	target->applyAbyssalMark(AbyssalMarkDuration);
	m_abyssalMarkCooldown = AbyssalMarkCooldown;
	return true;
}

bool CombatSystem::equipWeaponSlot(std::size_t slot)
{
	return m_inventory.equipSlot(slot);
}

const WeaponInventory& CombatSystem::getInventory() const
{
	return m_inventory;
}

float CombatSystem::getAbyssalMarkCooldownRemaining() const
{
	return m_abyssalMarkCooldown;
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
		const float arcLength = m_inventory.getEquippedWeapon().meleeRange;
		constexpr int ArcSegments = 12;

		sf::ConvexShape arc;
		arc.setPointCount(static_cast<std::size_t>(ArcSegments + 2));
		arc.setPoint(0, {0.f, 0.f});
		const float baseAngle = std::atan2(m_meleeDirection.y, m_meleeDirection.x);
		for (int index = 0; index <= ArcSegments; ++index)
		{
			const float angle = baseAngle - HalfArcRadians + (2.f * HalfArcRadians * index / ArcSegments);
			arc.setPoint(static_cast<std::size_t>(index + 1), {
				std::cos(angle) * arcLength,
				std::sin(angle) * arcLength
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
