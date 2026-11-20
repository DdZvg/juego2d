#include "Weapon.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace
{
constexpr std::array<WeaponDefinition, 2> DefaultDefinitions{{
	{WeaponId::RustyBlade, "Hoja herrumbrada",
		40.f, 112.f, 0.45f,
		25.f, 700.f, 700.f, 0.18f,
		1.25f},
	{WeaponId::EchoWand, "Vara de ecos",
		24.f, 78.f, 0.60f,
		18.f, 880.f, 900.f, 0.25f,
		1.5f},
}};

std::vector<WeaponDefinition>& currentDefinitions()
{
	static std::vector<WeaponDefinition> definitions;
	static std::vector<std::string> displayNames;
	if (definitions.empty())
	{
		definitions.assign(DefaultDefinitions.begin(), DefaultDefinitions.end());
	}
	return definitions;
}

std::vector<std::string>& displayNameStorage()
{
	static std::vector<std::string> storage;
	return storage;
}

bool parseWeaponId(const std::string& value, WeaponId& id)
{
	if (value == "RustyBlade")
	{
		id = WeaponId::RustyBlade;
		return true;
	}
	if (value == "EchoWand")
	{
		id = WeaponId::EchoWand;
		return true;
	}
	return false;
}

bool parseFloat(const std::string& value, float& result)
{
	try
	{
		const float parsed = std::stof(value);
		if (!std::isfinite(parsed))
		{
			return false;
		}
		result = parsed;
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}
}

const WeaponDefinition& getWeaponDefinition(WeaponId id)
{
	const std::vector<WeaponDefinition>& definitions = currentDefinitions();
	const auto found = std::find_if(definitions.begin(), definitions.end(),
		[id](const WeaponDefinition& definition) { return definition.id == id; });
	if (found != definitions.end())
	{
		return *found;
	}

	throw std::invalid_argument("Identificador de arma desconocido.");
}

void resetWeaponDefinitions()
{
	currentDefinitions().assign(DefaultDefinitions.begin(), DefaultDefinitions.end());
}

bool loadWeaponDefinitions(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return false;
	}

	std::vector<WeaponDefinition> loaded = currentDefinitions();
	std::vector<std::string> names(loaded.size());
	std::vector<bool> hasName(loaded.size(), false);

	std::string line;
	bool inWeapon = false;
	WeaponId weaponId = WeaponId::RustyBlade;
	std::size_t weaponIndex = loaded.size();
	std::size_t schemaVersion = 0;

	auto apply = [&](std::size_t index, const std::string& key, const std::string& value)
	{
		WeaponDefinition& definition = loaded[index];
		float parsed = 0.f;
		if (key == "display_name")
		{
			names[index] = value;
			hasName[index] = true;
		}
		else if (parseFloat(value, parsed))
		{
			if (key == "melee_damage" && parsed > 0.f) definition.meleeDamage = parsed;
			else if (key == "melee_range" && parsed >= 0.f) definition.meleeRange = parsed;
			else if (key == "melee_cooldown" && parsed >= 0.f) definition.meleeCooldown = parsed;
			else if (key == "projectile_damage" && parsed >= 0.f) definition.projectileDamage = parsed;
			else if (key == "projectile_speed" && parsed >= 0.f) definition.projectileSpeed = parsed;
			else if (key == "projectile_range" && parsed >= 0.f) definition.projectileRange = parsed;
			else if (key == "projectile_cooldown" && parsed >= 0.f) definition.projectileCooldown = parsed;
			else if (key == "marked_damage_multiplier" && parsed >= 1.f) definition.markedDamageMultiplier = parsed;
		}
	};

	while (std::getline(file, line))
	{
		const std::size_t comment = line.find('#');
		if (comment != std::string::npos)
		{
			line = line.substr(0, comment);
		}

		const auto first = line.find_first_not_of(" \t\r");
		if (first == std::string::npos)
		{
			continue;
		}
		const auto last = line.find_last_not_of(" \t\r");
		const std::string trimmed = line.substr(first, last - first + 1);

		if (!trimmed.empty() && trimmed.front() == '[')
		{
			inWeapon = trimmed == "[weapon]";
			continue;
		}

		const auto separator = trimmed.find('=');
		if (separator == std::string::npos)
		{
			continue;
		}

		const std::string key = trimmed.substr(0, separator);
		const std::string value = trimmed.substr(separator + 1);
		const auto keyFirst = key.find_first_not_of(" \t");
		const auto valueFirst = value.find_first_not_of(" \t");
		if (keyFirst == std::string::npos || valueFirst == std::string::npos)
		{
			continue;
		}
		const std::string cleanKey = key.substr(keyFirst, key.find_last_not_of(" \t") - keyFirst + 1);
		const std::string cleanValue = value.substr(valueFirst, value.find_last_not_of(" \t") - valueFirst + 1);

		if (cleanKey == "schema_version")
		{
			unsigned long parsed = 0;
			try
			{
				parsed = std::stoul(cleanValue);
			}
			catch (const std::exception&)
			{
				continue;
			}
			if (parsed != 1)
			{
				return false;
			}
			schemaVersion = parsed;
			continue;
		}

		if (!inWeapon)
		{
			continue;
		}

		if (cleanKey == "id")
		{
			if (!parseWeaponId(cleanValue, weaponId))
			{
				inWeapon = false;
				continue;
			}
			weaponIndex = static_cast<std::size_t>(weaponId);
			continue;
		}

		if (weaponIndex >= loaded.size())
		{
			continue;
		}

		apply(weaponIndex, cleanKey, cleanValue);
	}

	if (schemaVersion != 1)
	{
		return false;
	}

	std::vector<std::string>& storage = displayNameStorage();
	storage.clear();
	for (std::size_t index = 0; index < loaded.size(); ++index)
	{
		storage.push_back(hasName[index] ? names[index] : std::string(DefaultDefinitions[index].displayName));
		loaded[index].displayName = storage[index];
	}

	currentDefinitions() = std::move(loaded);
	return true;
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
