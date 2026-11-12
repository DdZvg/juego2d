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
}
