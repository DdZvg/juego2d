#pragma once

#include "HealthComponent.h"
#include "StatusEffectComponent.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

class Enemy
{
public:
	static constexpr float Radius = 22.f;
	static constexpr float MaximumHealth = 100.f;
	static constexpr float MoveSpeed = 85.f;
	static constexpr float ContactDamage = 12.f;
	static constexpr float ContactCooldown = 0.8f;

	explicit Enemy(sf::Vector2f position);

	sf::Vector2f getPosition() const;
	float getCurrentHealth() const;
	float getMaximumHealth() const;
	bool isAlive() const;
	bool isMarked() const;
	float getMarkTimeRemaining() const;
	void update(float deltaSeconds, sf::Vector2f targetPosition);
	void applyDamage(float amount, float markedDamageMultiplier = 1.f);
	void applyAbyssalMark(float durationSeconds);
	bool tryContactAttack();
	void render(sf::RenderTarget& target) const;

private:
	void updateAppearance();

	sf::Vector2f m_position;
	HealthComponent m_health;
	StatusEffectComponent m_statusEffects;
	float m_contactCooldown = 0.f;
	sf::CircleShape m_body;
};
