#pragma once

#include "Enemy.h"
#include "Projectile.h"
#include "Weapon.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

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

	explicit CombatSystem(sf::Vector2f enemyStartPosition);

	void update(float deltaSeconds, Player& player, const World& world);
	bool tryMelee(Player& player, sf::Vector2f aimWorldPosition);
	bool tryShoot(const Player& player, sf::Vector2f aimWorldPosition);
	bool tryAbyssalMark(sf::Vector2f playerPosition, sf::Vector2f aimWorldPosition);
	bool equipWeaponSlot(std::size_t slot);
	const WeaponInventory& getInventory() const;
	float getAbyssalMarkCooldownRemaining() const;
	void render(sf::RenderTarget& target) const;

	std::size_t getEnemyCount() const;
	const Enemy& getEnemy(std::size_t index) const;
	std::size_t getProjectileCount() const;

private:
	std::vector<Enemy> m_enemies;
	std::vector<Projectile> m_projectiles;
	WeaponInventory m_inventory;
	float m_meleeCooldown = 0.f;
	float m_projectileCooldown = 0.f;
	float m_abyssalMarkCooldown = 0.f;
	float m_meleeFlash = 0.f;
	sf::Vector2f m_meleeOrigin;
	sf::Vector2f m_meleeDirection{1.f, 0.f};
};
