#pragma once

#include "HealthComponent.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

class World;

class Player
{
public:
	static constexpr float Radius = 18.f;
	static constexpr float MoveSpeed = 280.f;
	static constexpr float MaximumHealth = 100.f;

	explicit Player(sf::Vector2f startPosition);

	sf::Vector2f getPosition() const;
	sf::Vector2f getAimDirection() const;
	float getCurrentHealth() const;
	float getMaximumHealth() const;
	bool isAlive() const;
	void setAimTarget(sf::Vector2f worldPosition);
	void setAimDirection(sf::Vector2f direction);
	void applyDamage(float amount);
	void update(sf::Vector2f movementDirection, float deltaSeconds, const World& world);
	void render(sf::RenderTarget& target) const;

private:
sf::Vector2f m_position;
sf::Vector2f m_aimDirection{1.f, 0.f};
HealthComponent m_health;
sf::CircleShape m_body;
sf::CircleShape m_highlight;
sf::CircleShape m_aimMarker;
};
