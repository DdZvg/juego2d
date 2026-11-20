#pragma once

#include "Enemy.h"
#include "EnemyArchetype.h"
#include "Projectile.h"
#include "Weapon.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

#include <cstddef>
#include <vector>

class Player;
class World;

class CombatSystem
{
public:
	static constexpr float MeleeRange = 112.f;
	static constexpr float MeleeDamage = 40.f;
	static constexpr float MeleeCooldown = 0.45f;
	static constexpr float ProjectileCooldown = 0.18f;
	static constexpr float MeleeArcDotThreshold = 0.5f;
	static constexpr float AbyssalMarkRange = 500.f;
	static constexpr float AbyssalMarkDuration = 4.f;
	static constexpr float AbyssalMarkCooldown = 6.f;
	static constexpr float WaveSpawnDelay = 1.5f;

	explicit CombatSystem(sf::Vector2f enemyStartPosition);
	CombatSystem() = default;

	void update(float deltaSeconds, Player& player, const World& world);
	bool tryMelee(Player& player, sf::Vector2f aimWorldPosition);
	bool tryShoot(const Player& player, sf::Vector2f aimWorldPosition);
	bool tryAbyssalMark(sf::Vector2f playerPosition, sf::Vector2f aimWorldPosition);
	bool equipWeaponSlot(std::size_t slot);
	const WeaponInventory& getInventory() const;
	float getAbyssalMarkCooldownRemaining() const;
	void render(sf::RenderTarget& target) const;

	void startEncounter(sf::Vector2f arenaCenter, const World& world);
	void restartEncounter(sf::Vector2f arenaCenter, const World& world);
	bool isEncounterActive() const;
	bool isEncounterComplete() const;
	bool isWaitingForNextWave() const;
	std::size_t getCurrentWaveNumber() const;
	std::size_t getWaveCount() const;
	float getNextWaveCountdown() const;
	Enemy& spawnEnemy(EnemyArchetype archetype, sf::Vector2f position);

	std::size_t getEnemyCount() const;
	Enemy& getEnemy(std::size_t index);
	const Enemy& getEnemy(std::size_t index) const;
	std::size_t getProjectileCount() const;

private:
	void spawnWave(std::size_t waveIndex, const World& world);
	sf::Vector2f findSpawnPosition(sf::Vector2f desired, float radius, const World& world) const;
	void spawnEnemyProjectile(const Enemy& enemy, sf::Vector2f direction);
	void resolveEnemySeparation(const World& world);
	void updateEncounter(float deltaSeconds, const World& world);

	std::vector<Enemy> m_enemies;
	std::vector<Projectile> m_projectiles;
	WeaponInventory m_inventory;
	float m_meleeCooldown = 0.f;
	float m_projectileCooldown = 0.f;
	float m_abyssalMarkCooldown = 0.f;
	float m_meleeFlash = 0.f;
	sf::Vector2f m_meleeOrigin;
	sf::Vector2f m_meleeDirection{1.f, 0.f};

	bool m_encounterActive = false;
	bool m_encounterComplete = false;
	bool m_waitingForNextWave = false;
	std::size_t m_currentWave = 0;
	float m_nextWaveDelay = 0.f;
	sf::Vector2f m_arenaCenter;
};
