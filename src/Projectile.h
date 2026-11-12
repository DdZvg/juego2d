#pragma once

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

class Projectile
{
public:
	static constexpr float Radius = 6.f;
	static constexpr float Speed = 700.f;
	static constexpr float MaximumRange = 700.f;
	static constexpr float Damage = 25.f;

	Projectile(sf::Vector2f position, sf::Vector2f direction);

	sf::Vector2f getPosition() const;
	bool isExpired() const;
	void update(float deltaSeconds);
	bool intersectsCircle(sf::Vector2f center, float radius) const;
	void render(sf::RenderTarget& target) const;

private:
	sf::Vector2f m_previousPosition;
	sf::Vector2f m_position;
	sf::Vector2f m_direction;
	float m_travelled = 0.f;
	sf::CircleShape m_body;
};
