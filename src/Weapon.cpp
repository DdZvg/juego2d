#include "Weapon.h"

#include <algorithm>
#include <stdexcept>

const WeaponDefinition& getWeaponDefinition(WeaponId id)
{
	static constexpr WeaponDefinition rustyBlade{
		WeaponId::RustyBlade, "Hoja herrumbrada",
		40.f, 112.f, 0.45f,
		25.f, 700.f, 700.f, 0.18f,
		1.25f
	};
	static constexpr WeaponDefinition echoWand{
		WeaponId::EchoWand, "Vara de ecos",
		24.f, 78.f, 0.60f,
		18.f, 880.f, 900.f, 0.25f,
		1.5f
	};

	switch (id)
	{
	case WeaponId::RustyBlade:
		return rustyBlade;
	case WeaponId::EchoWand:
		return echoWand;
	}

	throw std::invalid_argument("Identificador de arma desconocido.");
}

WeaponInventory::WeaponInventory()
	: m_weapons{WeaponId::RustyBlade, WeaponId::EchoWand}
{
}

bool WeaponInventory::add(WeaponId weapon)
{
	getWeaponDefinition(weapon);
	if (std::find(m_weapons.begin(), m_weapons.end(), weapon) != m_weapons.end())
	{
		return false;
	}

	m_weapons.push_back(weapon);
	return true;
}

bool WeaponInventory::equipSlot(std::size_t slot)
{
	if (slot >= m_weapons.size())
	{
		return false;
	}

	m_equippedSlot = slot;
	return true;
}

bool WeaponInventory::equip(WeaponId weapon)
{
	for (std::size_t slot = 0; slot < m_weapons.size(); ++slot)
	{
		if (m_weapons[slot] == weapon)
		{
			m_equippedSlot = slot;
			return true;
		}
	}

	return false;
}

std::size_t WeaponInventory::size() const
{
	return m_weapons.size();
}

std::size_t WeaponInventory::getEquippedSlot() const
{
	return m_equippedSlot;
}

WeaponId WeaponInventory::getWeaponAt(std::size_t slot) const
{
	return m_weapons.at(slot);
}

const WeaponDefinition& WeaponInventory::getEquippedWeapon() const
{
	return getWeaponDefinition(m_weapons.at(m_equippedSlot));
}
