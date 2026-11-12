#pragma once

#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

class Camera
{
public:
	static constexpr float FollowSpeed = 7.f;

	Camera(sf::Vector2f viewportSize, sf::Vector2f initialTarget);

	const sf::View& getView() const;
	void setViewportSize(sf::Vector2f viewportSize);
	void update(sf::Vector2f target, float deltaSeconds, sf::Vector2f worldSize);

private:
	void clampCenter(sf::Vector2f worldSize);

	sf::View m_view;
};
