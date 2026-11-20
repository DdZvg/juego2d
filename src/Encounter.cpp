#include "Encounter.h"

const std::vector<EncounterWave>& getPrototypeEncounterWaves()
{
	static const std::vector<EncounterWave> waves{
		EncounterWave{{
			{EnemyArchetype::Carronero, {-280.f, -160.f}},
			{EnemyArchetype::Carronero, {-360.f, 180.f}},
			{EnemyArchetype::ArqueroEcos, {340.f, -200.f}}
		}},
		EncounterWave{{
			{EnemyArchetype::Carronero, {-320.f, -300.f}},
			{EnemyArchetype::Carronero, {-340.f, 260.f}},
			{EnemyArchetype::Carronero, {0.f, -320.f}},
			{EnemyArchetype::ArqueroEcos, {360.f, -260.f}},
			{EnemyArchetype::ArqueroEcos, {380.f, 300.f}},
			{EnemyArchetype::Baluarte, {0.f, 420.f}}
		}},
		EncounterWave{{
			{EnemyArchetype::Carronero, {-380.f, -280.f}},
			{EnemyArchetype::Carronero, {380.f, -280.f}},
			{EnemyArchetype::ArqueroEcos, {-400.f, 220.f}},
			{EnemyArchetype::ArqueroEcos, {400.f, 220.f}},
			{EnemyArchetype::Baluarte, {0.f, -380.f}},
			{EnemyArchetype::Nodriza, {0.f, 400.f}}
		}}
	};
	return waves;
}

std::size_t getPrototypeEncounterWaveCount()
{
	return getPrototypeEncounterWaves().size();
}
