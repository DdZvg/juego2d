#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

enum class WeaponId
{
	RustyBlade,
	EchoWand
};

struct WeaponDefinition
{
	WeaponId id;
	std::string_view displayName;
	float meleeDamage;
	float meleeRange;
	float meleeCooldown;
	float projectileDamage;
	float projectileSpeed;
	float projectileRange;
	float projectileCooldown;
	float markedDamageMultiplier;
};

const WeaponDefinition& getWeaponDefinition(WeaponId id);

class WeaponInventory
{
public:
	WeaponInventory();

	bool add(WeaponId weapon);
	bool equipSlot(std::size_t slot);
	bool equip(WeaponId weapon);

	std::size_t size() const;
	std::size_t getEquippedSlot() const;
	WeaponId getWeaponAt(std::size_t slot) const;
	const WeaponDefinition& getEquippedWeapon() const;

private:
	std::vector<WeaponId> m_weapons;
	std::size_t m_equippedSlot = 0;
};