#pragma once

#include "EnemyArchetype.h"
#include "HealthComponent.h"
#include "StatusEffectComponent.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

class World;

enum class EnemyState
{
	Idle,
	Chase,
	Attack,
	Reposition,
	Support
};

struct EnemyAction
{
	bool fireProjectile = false;
	sf::Vector2f fireDirection{1.f, 0.f};
	bool supportPulse = false;
};

class Enemy
{
public:
	// Valores del arquetipo Carronero; las definiciones cargadas pueden ajustarlos.
	static constexpr float Radius = 22.f;
	static constexpr float MaximumHealth = 100.f;
	static constexpr float MoveSpeed = 85.f;
	static constexpr float ContactDamage = 12.f;
	static constexpr float ContactCooldown = 0.8f;

	explicit Enemy(sf::Vector2f position, EnemyArchetype archetype = EnemyArchetype::Carronero);

	EnemyArchetype getArchetype() const;
	const EnemyDefinition& getDefinition() const;
	float getRadius() const;
	EnemyState getState() const;
	sf::Vector2f getPosition() const;
	float getCurrentHealth() const;
	float getMaximumHealth() const;
	bool isAlive() const;
	bool isMarked() const;
	float getMarkTimeRemaining() const;
	EnemyAction update(float deltaSeconds, sf::Vector2f targetPosition, const World& world, bool targetAlive);
	void applyDamage(float amount, float markedDamageMultiplier = 1.f);
	void heal(float amount);
	void applyAbyssalMark(float durationSeconds);
	bool tryContactAttack();
	void pushApart(sf::Vector2f displacement, const World& world);
	void render(sf::RenderTarget& target) const;

private:
	void steer(sf::Vector2f desiredDirection, float speed, float deltaSeconds, float probeDistance, const World& world);
	void updateAppearance();
	float distanceTo(sf::Vector2f target) const;

	EnemyArchetype m_archetype;
	sf::Vector2f m_position;
	HealthComponent m_health;
	StatusEffectComponent m_statusEffects;
	EnemyState m_state = EnemyState::Idle;
	float m_contactCooldown = 0.f;
	float m_attackCooldown = 0.f;
	float m_supportCooldown = 0.f;
	float m_supportFlash = 0.f;
	int m_strafeSign = 1;
	sf::Vector2f m_detourDirection{1.f, 0.f};
	float m_detourTime = 0.f;
	sf::CircleShape m_body;
};
