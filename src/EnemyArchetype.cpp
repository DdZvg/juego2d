#include "EnemyArchetype.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
constexpr std::array<EnemyDefinition, EnemyArchetypeCount> DefaultDefinitions{{
	{EnemyArchetype::Carronero, "Carronero", EnemyBehavior::Melee,
		100.f, 22.f, 85.f, 12.f, 0.8f, 0.f,
		0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		sf::Color(168, 74, 64), sf::Color(232, 146, 110)},
	{EnemyArchetype::ArqueroEcos, "Arquero de ecos", EnemyBehavior::Ranged,
		60.f, 18.f, 95.f, 6.f, 1.f, 0.f,
		440.f, 300.f,
		12.f, 430.f, 560.f, 1.6f,
		0.f, 0.f, 0.f,
		sf::Color(74, 104, 160), sf::Color(140, 184, 232)},
	{EnemyArchetype::Baluarte, "Baluarte", EnemyBehavior::Melee,
		280.f, 30.f, 55.f, 20.f, 1.2f, 0.35f,
		0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		sf::Color(104, 92, 70), sf::Color(186, 166, 122)},
	{EnemyArchetype::Nodriza, "Nodriza abisal", EnemyBehavior::Support,
		80.f, 22.f, 100.f, 6.f, 1.f, 0.f,
		0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		300.f, 10.f, 2.5f,
		sf::Color(76, 132, 104), sf::Color(140, 214, 170)},
}};

std::vector<EnemyDefinition>& currentDefinitions()
{
	static std::vector<EnemyDefinition> definitions;
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

bool parseArchetype(const std::string& value, EnemyArchetype& archetype)
{
	if (value == "Carronero")
	{
		archetype = EnemyArchetype::Carronero;
		return true;
	}
	if (value == "ArqueroEcos")
	{
		archetype = EnemyArchetype::ArqueroEcos;
		return true;
	}
	if (value == "Baluarte")
	{
		archetype = EnemyArchetype::Baluarte;
		return true;
	}
	if (value == "Nodriza")
	{
		archetype = EnemyArchetype::Nodriza;
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

const EnemyDefinition& getEnemyDefinition(EnemyArchetype archetype)
{
	const std::vector<EnemyDefinition>& definitions = currentDefinitions();
	const auto found = std::find_if(definitions.begin(), definitions.end(),
		[archetype](const EnemyDefinition& definition) { return definition.archetype == archetype; });
	if (found != definitions.end())
	{
		return *found;
	}

	throw std::invalid_argument("Identificador de enemigo desconocido.");
}

void resetEnemyDefinitions()
{
	currentDefinitions().assign(DefaultDefinitions.begin(), DefaultDefinitions.end());
}

bool loadEnemyDefinitions(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		return false;
	}

	std::vector<EnemyDefinition> loaded = currentDefinitions();
	std::vector<std::string> names(loaded.size());
	std::vector<bool> hasName(loaded.size(), false);

	std::string line;
	bool inEnemy = false;
	std::size_t enemyIndex = loaded.size();
	std::size_t schemaVersion = 0;

	auto apply = [&](std::size_t index, const std::string& key, const std::string& value)
	{
		EnemyDefinition& definition = loaded[index];
		float parsed = 0.f;
		if (key == "display_name")
		{
			names[index] = value;
			hasName[index] = true;
		}
		else if (parseFloat(value, parsed))
		{
			if (key == "maximum_health" && parsed > 0.f) definition.maximumHealth = parsed;
			else if (key == "radius" && parsed > 0.f) definition.radius = parsed;
			else if (key == "move_speed" && parsed >= 0.f) definition.moveSpeed = parsed;
			else if (key == "contact_damage" && parsed >= 0.f) definition.contactDamage = parsed;
			else if (key == "contact_cooldown" && parsed >= 0.f) definition.contactCooldown = parsed;
			else if (key == "damage_reduction" && parsed >= 0.f && parsed <= 0.95f) definition.damageReduction = parsed;
			else if (key == "attack_range" && parsed >= 0.f) definition.attackRange = parsed;
			else if (key == "preferred_range" && parsed >= 0.f) definition.preferredRange = parsed;
			else if (key == "projectile_damage" && parsed >= 0.f) definition.projectileDamage = parsed;
			else if (key == "projectile_speed" && parsed >= 0.f) definition.projectileSpeed = parsed;
			else if (key == "projectile_range" && parsed >= 0.f) definition.projectileRange = parsed;
			else if (key == "attack_cooldown" && parsed >= 0.f) definition.attackCooldown = parsed;
			else if (key == "support_radius" && parsed >= 0.f) definition.supportRadius = parsed;
			else if (key == "support_heal" && parsed >= 0.f) definition.supportHeal = parsed;
			else if (key == "support_cooldown" && parsed >= 0.f) definition.supportCooldown = parsed;
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
			inEnemy = trimmed == "[enemy]";
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

		if (!inEnemy)
		{
			continue;
		}

		if (cleanKey == "id")
		{
			EnemyArchetype archetype = EnemyArchetype::Carronero;
			if (!parseArchetype(cleanValue, archetype))
			{
				inEnemy = false;
				continue;
			}
			enemyIndex = static_cast<std::size_t>(archetype);
			continue;
		}

		if (enemyIndex >= loaded.size())
		{
			continue;
		}

		apply(enemyIndex, cleanKey, cleanValue);
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
