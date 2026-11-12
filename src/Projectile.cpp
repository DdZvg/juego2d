#include "Projectile.h"

#include <SFML/Graphics/Color.hpp>

#include <algorithm>
#include <cmath>

Projectile::Projectile(
	sf::Vector2f position,
	sf::Vector2f direction,
	float speed,
	float maximumRange,
	float damage,
	float markedDamageMultiplier)
	: m_previousPosition(position),
	  m_position(position),
	  m_direction(direction),
	  m_speed(std::max(0.f, speed)),
	  m_maximumRange(std::max(0.f, maximumRange)),
	  m_damage(std::max(0.f, damage)),
	  m_markedDamageMultiplier(std::max(1.f, markedDamageMultiplier)),
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

float Projectile::getDamage() const
{
	return m_damage;
}

float Projectile::getMarkedDamageMultiplier() const
{
	return m_markedDamageMultiplier;
}

bool Projectile::isExpired() const
{
	return m_travelled >= m_maximumRange || (m_direction.x == 0.f && m_direction.y == 0.f) || m_speed == 0.f;
}

void Projectile::update(float deltaSeconds)
{
	m_previousPosition = m_position;
	const float step = std::min(m_speed * std::max(0.f, deltaSeconds), m_maximumRange - m_travelled);
	m_position.x += m_direction.x * step;
	m_position.y += m_direction.y * step;
	m_travelled += step;
	m_body.setPosition(m_position);
}

std::optional<float> Projectile::getCircleHitFraction(sf::Vector2f center, float radius) const
{
	const sf::Vector2f segment = m_position - m_previousPosition;
	const sf::Vector2f toStart = m_previousPosition - center;
	const float combinedRadius = radius + Radius;
	const float a = segment.x * segment.x + segment.y * segment.y;
	const float c = toStart.x * toStart.x + toStart.y * toStart.y - combinedRadius * combinedRadius;

	if (a <= 0.000001f)
	{
		return c <= 0.f ? std::optional<float>(0.f) : std::nullopt;
	}

	const float b = 2.f * (toStart.x * segment.x + toStart.y * segment.y);
	const float discriminant = b * b - 4.f * a * c;
	if (discriminant < 0.f)
	{
		return std::nullopt;
	}

	const float sqrtDiscriminant = std::sqrt(discriminant);
	const float entry = (-b - sqrtDiscriminant) / (2.f * a);
	if (entry >= 0.f && entry <= 1.f)
	{
		return entry;
	}

	if (c <= 0.f)
	{
		return 0.f;
	}

	return std::nullopt;
}

std::optional<float> Projectile::getRectHitFraction(const sf::FloatRect& rect) const
{
	const float left = rect.position.x - Radius;
	const float top = rect.position.y - Radius;
	const float right = rect.position.x + rect.size.x + Radius;
	const float bottom = rect.position.y + rect.size.y + Radius;

	const sf::Vector2f start = m_previousPosition;
	const sf::Vector2f segment = m_position - m_previousPosition;

	float entry = 0.f;
	float exit = 1.f;

	const auto clip = [&entry, &exit](float origin, float direction, float minimum, float maximum)
	{
		if (std::abs(direction) < 0.000001f)
		{
			return origin >= minimum && origin <= maximum;
		}

		float axisEntry = (minimum - origin) / direction;
		float axisExit = (maximum - origin) / direction;
		if (axisEntry > axisExit)
		{
			std::swap(axisEntry, axisExit);
		}

		entry = std::max(entry, axisEntry);
		exit = std::min(exit, axisExit);
		return entry <= exit;
	};

	if (!clip(start.x, segment.x, left, right) || !clip(start.y, segment.y, top, bottom))
	{
		return std::nullopt;
	}

	return entry;
}

bool Projectile::intersectsCircle(sf::Vector2f center, float radius) const
{
	return getCircleHitFraction(center, radius).has_value();
}

void Projectile::render(sf::RenderTarget& target) const
{
	target.draw(m_body);
}
