#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

class World
{
public:
	static constexpr float Width = 3000.f;
	static constexpr float Height = 3000.f;

	sf::Vector2f getSize() const;
	sf::Vector2f constrainPosition(sf::Vector2f position, float margin) const;
	void render(sf::RenderTarget& target, const sf::View& view) const;
};
