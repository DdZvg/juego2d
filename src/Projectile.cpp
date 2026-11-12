#include "Projectile.h"

#include <SFML/Graphics/Color.hpp>

#include <algorithm>
#include <cmath>

Projectile::Projectile(sf::Vector2f position, sf::Vector2f direction)
	: m_previousPosition(position),
	  m_position(position),
	  m_direction(direction),
	  m_body(Radius)
{
	const float lengthSquared = m_direction.x * m_direction.x + m_direction.y * m_direction.y;
	if (lengthSquared > 0.f)
	{
		const float inverseLength = 1.f / std::sqrt(lengthSquared);
		m_direction.x *= inverseLength;
		m_direction.y *= inverseLength;
	}

	m_body.setOrigin({Radius, Radius});
	m_body.setFillColor(sf::Color(246, 202, 119));
	m_body.setOutlineColor(sf::Color(255, 232, 173));
	m_body.setOutlineThickness(2.f);
	m_body.setPosition(m_position);
}

sf::Vector2f Projectile::getPosition() const
{
	return m_position;
}

bool Projectile::isExpired() const
{
	return m_travelled >= MaximumRange || (m_direction.x == 0.f && m_direction.y == 0.f);
}

void Projectile::update(float deltaSeconds)
{
	m_previousPosition = m_position;
	const float step = std::min(Speed * deltaSeconds, MaximumRange - m_travelled);
	m_position.x += m_direction.x * step;
	m_position.y += m_direction.y * step;
	m_travelled += step;
	m_body.setPosition(m_position);
}

bool Projectile::intersectsCircle(sf::Vector2f center, float radius) const
{
	const sf::Vector2f segment = m_position - m_previousPosition;
	const float segmentLengthSquared = segment.x * segment.x + segment.y * segment.y;
	float projection = 0.f;

	if (segmentLengthSquared > 0.f)
	{
		const sf::Vector2f toCenter = center - m_previousPosition;
		projection = std::clamp((toCenter.x * segment.x + toCenter.y * segment.y) / segmentLengthSquared, 0.f, 1.f);
	}

	const sf::Vector2f closestPoint = m_previousPosition + segment * projection;
	const sf::Vector2f offset = center - closestPoint;
	const float combinedRadius = radius + Radius;
	return offset.x * offset.x + offset.y * offset.y <= combinedRadius * combinedRadius;
}

void Projectile::render(sf::RenderTarget& target) const
{
	target.draw(m_body);
}
