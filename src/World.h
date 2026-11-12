#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

#include <vector>

class World
{
public:
	static constexpr float Width = 3000.f;
	static constexpr float Height = 3000.f;

	World();

	sf::Vector2f getSize() const;
	sf::Vector2f constrainPosition(sf::Vector2f position, float margin) const;
	sf::Vector2f moveWithCollisions(sf::Vector2f position, sf::Vector2f displacement, float halfExtent) const;
	bool collides(const sf::FloatRect& bounds) const;
	const std::vector<sf::FloatRect>& getObstacles() const;
	void render(sf::RenderTarget& target, const sf::View& view) const;

private:
	enum class Axis
	{
		Horizontal,
		Vertical
	};

	float sweepAxis(sf::Vector2f position, float delta, float halfExtent, Axis axis) const;

	std::vector<sf::FloatRect> m_obstacles;
};
