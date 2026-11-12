#include "World.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <algorithm>
#include <cmath>

namespace
{
constexpr float GridSpacing = 64.f;
constexpr float GridThickness = 1.f;
const sf::Color BackgroundColor(23, 22, 30);
const sf::Color GridColor(39, 37, 48);
const sf::Color WallColor(62, 58, 78);
const sf::Color WallOutlineColor(96, 90, 116);
}

World::World()
	: m_obstacles{
		sf::FloatRect({1200.f, 1250.f}, {600.f, 48.f}),
		sf::FloatRect({1800.f, 1350.f}, {48.f, 400.f}),
		sf::FloatRect({1050.f, 1450.f}, {180.f, 180.f}),
		sf::FloatRect({1350.f, 1800.f}, {500.f, 48.f}),
		sf::FloatRect({1650.f, 1600.f}, {64.f, 64.f})
	}
{
}

sf::Vector2f World::getSize() const
{
	return {Width, Height};
}

sf::Vector2f World::constrainPosition(sf::Vector2f position, float margin) const
{
	position.x = std::clamp(position.x, margin, Width - margin);
	position.y = std::clamp(position.y, margin, Height - margin);
	return position;
}

sf::Vector2f World::moveWithCollisions(sf::Vector2f position, sf::Vector2f displacement, float halfExtent) const
{
	position.x = sweepAxis(position, displacement.x, halfExtent, Axis::Horizontal);
	position.y = sweepAxis(position, displacement.y, halfExtent, Axis::Vertical);
	return constrainPosition(position, halfExtent);
}

bool World::collides(const sf::FloatRect& bounds) const
{
	for (const sf::FloatRect& obstacle : m_obstacles)
	{
		const bool overlaps =
			bounds.position.x < obstacle.position.x + obstacle.size.x &&
			bounds.position.x + bounds.size.x > obstacle.position.x &&
			bounds.position.y < obstacle.position.y + obstacle.size.y &&
			bounds.position.y + bounds.size.y > obstacle.position.y;

		if (overlaps)
		{
			return true;
		}
	}

	return false;
}

const std::vector<sf::FloatRect>& World::getObstacles() const
{
	return m_obstacles;
}

float World::sweepAxis(sf::Vector2f position, float delta, float halfExtent, Axis axis) const
{
	if (delta == 0.f)
	{
		return axis == Axis::Horizontal ? position.x : position.y;
	}

	float target = (axis == Axis::Horizontal ? position.x : position.y) + delta;

	for (const sf::FloatRect& obstacle : m_obstacles)
	{
		const float obstacleLeft = obstacle.position.x;
		const float obstacleRight = obstacle.position.x + obstacle.size.x;
		const float obstacleTop = obstacle.position.y;
		const float obstacleBottom = obstacle.position.y + obstacle.size.y;

		if (axis == Axis::Horizontal)
		{
			const bool overlapsVertically =
				position.y + halfExtent > obstacleTop &&
				position.y - halfExtent < obstacleBottom;

			if (!overlapsVertically)
			{
				continue;
			}

			if (delta > 0.f)
			{
				if (position.x + halfExtent <= obstacleLeft && target + halfExtent > obstacleLeft)
				{
					target = std::min(target, obstacleLeft - halfExtent);
				}
			}
			else if (position.x - halfExtent >= obstacleRight && target - halfExtent < obstacleRight)
			{
				target = std::max(target, obstacleRight + halfExtent);
			}
		}
		else
		{
			const bool overlapsHorizontally =
				position.x + halfExtent > obstacleLeft &&
				position.x - halfExtent < obstacleRight;

			if (!overlapsHorizontally)
			{
				continue;
			}

			if (delta > 0.f)
			{
				if (position.y + halfExtent <= obstacleTop && target + halfExtent > obstacleTop)
				{
					target = std::min(target, obstacleTop - halfExtent);
				}
			}
			else if (position.y - halfExtent >= obstacleBottom && target - halfExtent < obstacleBottom)
			{
				target = std::max(target, obstacleBottom + halfExtent);
			}
		}
	}

	return target;
}

void World::render(sf::RenderTarget& target, const sf::View& view) const
{
	sf::RectangleShape background({Width, Height});
	background.setFillColor(BackgroundColor);
	target.draw(background);

	const sf::Vector2f viewCenter = view.getCenter();
	const sf::Vector2f viewSize = view.getSize();
	const float left = std::max(0.f, viewCenter.x - viewSize.x / 2.f);
	const float right = std::min(Width, viewCenter.x + viewSize.x / 2.f);
	const float top = std::max(0.f, viewCenter.y - viewSize.y / 2.f);
	const float bottom = std::min(Height, viewCenter.y + viewSize.y / 2.f);

	const int firstVertical = static_cast<int>(std::ceil(left / GridSpacing));
	const int lastVertical = static_cast<int>(std::floor(right / GridSpacing));
	for (int index = firstVertical; index <= lastVertical; ++index)
	{
		sf::RectangleShape line({GridThickness, Height});
		line.setPosition({static_cast<float>(index) * GridSpacing, 0.f});
		line.setFillColor(GridColor);
		target.draw(line);
	}

	const int firstHorizontal = static_cast<int>(std::ceil(top / GridSpacing));
	const int lastHorizontal = static_cast<int>(std::floor(bottom / GridSpacing));
	for (int index = firstHorizontal; index <= lastHorizontal; ++index)
	{
		sf::RectangleShape line({Width, GridThickness});
		line.setPosition({0.f, static_cast<float>(index) * GridSpacing});
		line.setFillColor(GridColor);
		target.draw(line);
	}

	for (const sf::FloatRect& obstacle : m_obstacles)
	{
		sf::RectangleShape wall(obstacle.size);
		wall.setPosition(obstacle.position);
		wall.setFillColor(WallColor);
		wall.setOutlineColor(WallOutlineColor);
		wall.setOutlineThickness(2.f);
		target.draw(wall);
	}
}
