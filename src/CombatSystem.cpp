#include "CombatSystem.h"

#include "Encounter.h"
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
	: m_arenaCenter(enemyStartPosition)
{
	m_enemies.emplace_back(enemyStartPosition, EnemyArchetype::Carronero);
}

void CombatSystem::update(float deltaSeconds, Player& player, const World& world)
{
	m_meleeCooldown = std::max(0.f, m_meleeCooldown - deltaSeconds);
	m_projectileCooldown = std::max(0.f, m_projectileCooldown - deltaSeconds);
	m_abyssalMarkCooldown = std::max(0.f, m_abyssalMarkCooldown - deltaSeconds);
	m_meleeFlash = std::max(0.f, m_meleeFlash - deltaSeconds);

	for (Enemy& enemy : m_enemies)
	{
		const EnemyAction action = enemy.update(deltaSeconds, player.getPosition(), world, player.isAlive());

		if (action.fireProjectile)
		{
			spawnEnemyProjectile(enemy, action.fireDirection);
		}

		if (action.supportPulse && enemy.isAlive())
		{
			const EnemyDefinition& definition = enemy.getDefinition();
			const float supportRadiusSquared = definition.supportRadius * definition.supportRadius;
			for (Enemy& ally : m_enemies)
			{
				if (&ally == &enemy || !ally.isAlive())
				{
					continue;
				}

				if (distanceSquared(ally.getPosition(), enemy.getPosition()) <= supportRadiusSquared)
				{
					ally.heal(definition.supportHeal);
				}
			}
		}
	}

	for (auto projectile = m_projectiles.begin(); projectile != m_projectiles.end();)
	{
		projectile->update(deltaSeconds);

		float nearestHitFraction = 2.f;
		Enemy* hitEnemy = nullptr;
		bool hitPlayer = false;

		if (projectile->getFaction() == ProjectileFaction::Player)
		{
			for (Enemy& enemy : m_enemies)
			{
				if (!enemy.isAlive())
				{
					continue;
				}

				const std::optional<float> hitFraction =
					projectile->getCircleHitFraction(enemy.getPosition(), enemy.getRadius());
				if (hitFraction && *hitFraction < nearestHitFraction)
				{
					nearestHitFraction = *hitFraction;
					hitEnemy = &enemy;
				}
			}
		}
		else if (player.isAlive())
		{
			const std::optional<float> hitFraction =
				projectile->getCircleHitFraction(player.getPosition(), Player::Radius);
			if (hitFraction && *hitFraction < nearestHitFraction)
			{
				nearestHitFraction = *hitFraction;
				hitPlayer = true;
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
				hitPlayer = false;
				hitWall = true;
			}
		}

		if (hitEnemy != nullptr)
		{
			hitEnemy->applyDamage(projectile->getDamage(), projectile->getMarkedDamageMultiplier());
		}
		else if (hitPlayer)
		{
			player.applyDamage(projectile->getDamage());
		}

		if (hitWall || hitEnemy != nullptr || hitPlayer || projectile->isExpired())
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
		if (!enemy.isAlive())
		{
			continue;
		}

		const float contactDistance = Player::Radius + enemy.getRadius();
		if (distanceSquared(player.getPosition(), enemy.getPosition()) <= contactDistance * contactDistance &&
			enemy.tryContactAttack())
		{
			player.applyDamage(enemy.getDefinition().contactDamage);
		}
	}

	m_enemies.erase(
		std::remove_if(m_enemies.begin(), m_enemies.end(), [](const Enemy& enemy) { return !enemy.isAlive(); }),
		m_enemies.end());

	resolveEnemySeparation(world);
	updateEncounter(deltaSeconds, world);
}

bool CombatSystem::tryMelee(Player& player, sf::Vector2f aimWorldPosition)
{
	if (m_meleeCooldown > 0.0001f)
	{
		return false;
	}

	const WeaponDefinition& weapon = m_inventory.getEquippedWeapon();
	m_meleeDirection = normalized(aimWorldPosition - player.getPosition(), player.getAimDirection());
	m_meleeOrigin = player.getPosition();
	player.setAimDirection(m_meleeDirection);
	m_meleeCooldown = weapon.meleeCooldown;
	m_meleeFlash = 0.16f;

	for (Enemy& enemy : m_enemies)	{
		if (!enemy.isAlive())
		{
			continue;
		}

		const sf::Vector2f toEnemy = enemy.getPosition() - player.getPosition();
		const float distance = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.y * toEnemy.y);
		if (distance <= weapon.meleeRange + enemy.getRadius() && distance > 0.f)
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
	if (m_projectileCooldown > 0.0001f || weapon.projectileDamage <= 0.f || weapon.projectileSpeed <= 0.f)
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
		weapon.markedDamageMultiplier,
		ProjectileFaction::Player);
	m_projectileCooldown = weapon.projectileCooldown;
	return true;
}

bool CombatSystem::tryAbyssalMark(sf::Vector2f playerPosition, sf::Vector2f aimWorldPosition)
{
	if (m_abyssalMarkCooldown > 0.0001f)
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

void CombatSystem::startEncounter(sf::Vector2f arenaCenter, const World& world)
{
	m_arenaCenter = arenaCenter;
	m_enemies.clear();
	m_projectiles.clear();
	m_meleeCooldown = 0.f;
	m_projectileCooldown = 0.f;
	m_abyssalMarkCooldown = 0.f;
	m_meleeFlash = 0.f;

	m_encounterActive = true;
	m_encounterComplete = false;
	m_waitingForNextWave = false;
	m_currentWave = 0;
	m_nextWaveDelay = 0.f;

	spawnWave(0, world);
}

void CombatSystem::restartEncounter(sf::Vector2f arenaCenter, const World& world)
{
	startEncounter(arenaCenter, world);
}

bool CombatSystem::isEncounterActive() const
{
	return m_encounterActive;
}

bool CombatSystem::isEncounterComplete() const
{
	return m_encounterComplete;
}

bool CombatSystem::isWaitingForNextWave() const
{
	return m_waitingForNextWave;
}

std::size_t CombatSystem::getCurrentWaveNumber() const
{
	return getPrototypeEncounterWaveCount() == 0 ? 0 : m_currentWave + 1;
}

std::size_t CombatSystem::getWaveCount() const
{
	return getPrototypeEncounterWaveCount();
}

float CombatSystem::getNextWaveCountdown() const
{
	return m_waitingForNextWave ? std::max(0.f, m_nextWaveDelay) : 0.f;
}

Enemy& CombatSystem::spawnEnemy(EnemyArchetype archetype, sf::Vector2f position)
{
	m_enemies.emplace_back(position, archetype);
	return m_enemies.back();
}

void CombatSystem::spawnWave(std::size_t waveIndex, const World& world)
{
	const std::vector<EncounterWave>& waves = getPrototypeEncounterWaves();
	if (waveIndex >= waves.size())
	{
		return;
	}

	for (const EnemySpawn& spawn : waves[waveIndex].spawns)
	{
		const float radius = getEnemyDefinition(spawn.archetype).radius;
		const sf::Vector2f desired = m_arenaCenter + spawn.offset;
		spawnEnemy(spawn.archetype, findSpawnPosition(desired, radius, world));
	}
}

sf::Vector2f CombatSystem::findSpawnPosition(sf::Vector2f desired, float radius, const World& world) const
{
	const auto isValid = [&world, radius](sf::Vector2f position)
	{
		const float margin = radius + 8.f;
		return position.x >= margin && position.x <= World::Width - margin &&
			position.y >= margin && position.y <= World::Height - margin &&
			!world.collidesCircle(position, radius + 2.f);
	};

	if (isValid(desired))
	{
		return desired;
	}

	constexpr float RingStep = 52.f;
	for (int ring = 1; ring <= 8; ++ring)
	{
		for (int step = 0; step < 8; ++step)
		{
			const float angle = 6.2831853072f * static_cast<float>(step) / 8.f;
			const sf::Vector2f candidate{
				desired.x + std::cos(angle) * RingStep * static_cast<float>(ring),
				desired.y + std::sin(angle) * RingStep * static_cast<float>(ring)
			};
			if (isValid(candidate))
			{
				return candidate;
			}
		}
	}

	return world.constrainPosition(desired, radius + 8.f);
}

void CombatSystem::spawnEnemyProjectile(const Enemy& enemy, sf::Vector2f direction)
{
	const EnemyDefinition& definition = enemy.getDefinition();
	if (definition.projectileDamage <= 0.f || definition.projectileSpeed <= 0.f || definition.projectileRange <= 0.f)
	{
		return;
	}

	const sf::Vector2f normalizedDirection = normalized(direction, {1.f, 0.f});
	const sf::Vector2f spawnPosition(
		enemy.getPosition().x + normalizedDirection.x * (enemy.getRadius() + Projectile::Radius + 3.f),
		enemy.getPosition().y + normalizedDirection.y * (enemy.getRadius() + Projectile::Radius + 3.f));

	m_projectiles.emplace_back(
		spawnPosition,
		normalizedDirection,
		definition.projectileSpeed,
		definition.projectileRange,
		definition.projectileDamage,
		1.f,
		ProjectileFaction::Enemy);
}

void CombatSystem::resolveEnemySeparation(const World& world)
{
	for (std::size_t first = 0; first < m_enemies.size(); ++first)
	{
		if (!m_enemies[first].isAlive())
		{
			continue;
		}

		for (std::size_t second = first + 1; second < m_enemies.size(); ++second)
		{
			if (!m_enemies[second].isAlive())
			{
				continue;
			}

			Enemy& a = m_enemies[first];
			Enemy& b = m_enemies[second];
			const sf::Vector2f delta = b.getPosition() - a.getPosition();
			const float minimumDistance = a.getRadius() + b.getRadius();
			const float distanceSquaredValue = delta.x * delta.x + delta.y * delta.y;
			if (distanceSquaredValue >= minimumDistance * minimumDistance)
			{
				continue;
			}

			sf::Vector2f normal{1.f, 0.f};
			float distance = 0.f;
			if (distanceSquaredValue > 0.0001f)
			{
				distance = std::sqrt(distanceSquaredValue);
				normal = {delta.x / distance, delta.y / distance};
			}

			const float push = (minimumDistance - distance) * 0.5f;
			a.pushApart({-normal.x * push, -normal.y * push}, world);
			b.pushApart({normal.x * push, normal.y * push}, world);
		}
	}
}

void CombatSystem::updateEncounter(float deltaSeconds, const World& world)
{
	if (!m_encounterActive || !m_enemies.empty())
	{
		return;
	}

	const std::size_t waveCount = getPrototypeEncounterWaveCount();
	if (m_currentWave + 1 >= waveCount)
	{
		m_encounterActive = false;
		m_encounterComplete = true;
		m_waitingForNextWave = false;
		m_nextWaveDelay = 0.f;
		return;
	}

	if (!m_waitingForNextWave)
	{
		m_waitingForNextWave = true;
		m_nextWaveDelay = WaveSpawnDelay;
	}

	m_nextWaveDelay -= deltaSeconds;
	if (m_nextWaveDelay <= 0.f)
	{
		++m_currentWave;
		spawnWave(m_currentWave, world);
		m_waitingForNextWave = false;
		m_nextWaveDelay = 0.f;
	}
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

Enemy& CombatSystem::getEnemy(std::size_t index)
{
	return m_enemies.at(index);
}

const Enemy& CombatSystem::getEnemy(std::size_t index) const
{
	return m_enemies.at(index);
}

std::size_t CombatSystem::getProjectileCount() const
{
	return m_projectiles.size();
}
