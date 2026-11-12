#include "Camera.h"

#include <algorithm>
#include <cmath>

Camera::Camera(sf::Vector2f viewportSize, sf::Vector2f initialTarget)
	: m_view(sf::FloatRect({0.f, 0.f}, viewportSize))
{
	m_view.setCenter(initialTarget);
}

const sf::View& Camera::getView() const
{
	return m_view;
}

void Camera::setViewportSize(sf::Vector2f viewportSize)
{
	if (viewportSize.x > 0.f && viewportSize.y > 0.f)
	{
		m_view.setSize(viewportSize);
	}
}

void Camera::update(sf::Vector2f target, float deltaSeconds, sf::Vector2f worldSize)
{
	const float blend = 1.f - std::exp(-FollowSpeed * deltaSeconds);
	const sf::Vector2f currentCenter = m_view.getCenter();
	m_view.setCenter({
		currentCenter.x + (target.x - currentCenter.x) * blend,
		currentCenter.y + (target.y - currentCenter.y) * blend
	});
	clampCenter(worldSize);
}

void Camera::clampCenter(sf::Vector2f worldSize)
{
	const sf::Vector2f viewSize = m_view.getSize();
	sf::Vector2f center = m_view.getCenter();

	if (viewSize.x >= worldSize.x)
	{
		center.x = worldSize.x / 2.f;
	}
	else
	{
		center.x = std::clamp(center.x, viewSize.x / 2.f, worldSize.x - viewSize.x / 2.f);
	}

	if (viewSize.y >= worldSize.y)
	{
		center.y = worldSize.y / 2.f;
	}
	else
	{
		center.y = std::clamp(center.y, viewSize.y / 2.f, worldSize.y - viewSize.y / 2.f);
	}

	m_view.setCenter(center);
}
