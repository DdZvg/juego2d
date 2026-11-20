#pragma once

#include "EnemyArchetype.h"

#include <SFML/System/Vector2.hpp>

#include <cstddef>
#include <vector>

struct EnemySpawn
{
	EnemyArchetype archetype;
	sf::Vector2f offset;
};

struct EncounterWave
{
	std::vector<EnemySpawn> spawns;
};

const std::vector<EncounterWave>& getPrototypeEncounterWaves();

std::size_t getPrototypeEncounterWaveCount();
