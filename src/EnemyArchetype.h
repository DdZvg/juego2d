#pragma once

#include <SFML/Graphics/Color.hpp>

#include <cstddef>
#include <string>
#include <string_view>

enum class EnemyArchetype
{
	Carronero = 0,
	ArqueroEcos = 1,
	Baluarte = 2,
	Nodriza = 3
};

inline constexpr std::size_t EnemyArchetypeCount = 4;

enum class EnemyBehavior
{
	Melee,
	Ranged,
	Support
};

struct EnemyDefinition
{
	EnemyArchetype archetype;
	std::string_view displayName;
	EnemyBehavior behavior;
	float maximumHealth;
	float radius;
	float moveSpeed;
	float contactDamage;
	float contactCooldown;
	float damageReduction;
	float attackRange;
	float preferredRange;
	float projectileDamage;
	float projectileSpeed;
	float projectileRange;
	float attackCooldown;
	float supportRadius;
	float supportHeal;
	float supportCooldown;
	sf::Color fillColor;
	sf::Color outlineColor;
};

const EnemyDefinition& getEnemyDefinition(EnemyArchetype archetype);

bool loadEnemyDefinitions(const std::string& filePath);
void resetEnemyDefinitions();
