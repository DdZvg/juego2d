#include "Player.h"

#include "World.h"

#include <SFML/Graphics/Color.hpp>

#include <cmath>

Player::Player(sf::Vector2f startPosition)
	: m_position(startPosition),
	  m_health(MaximumHealth),
	  m_body(Radius),
	  m_highlight(4.f),
	  m_aimMarker(4.f)
{
	m_body.setOrigin({Radius, Radius});
	m_body.setFillColor(sf::Color(179, 112, 78));
	m_body.setOutlineColor(sf::Color(226, 174, 123));
	m_body.setOutlineThickness(3.f);

	m_highlight.setOrigin({4.f, 4.f});
	m_highlight.setFillColor(sf::Color(250, 214, 159));
	m_aimMarker.setOrigin({4.f, 4.f});
	m_aimMarker.setFillColor(sf::Color(248, 200, 112));
	m_body.setPosition(m_position);
	m_highlight.setPosition({m_position.x - 5.f, m_position.y - 6.f});
	m_aimMarker.setPosition({m_position.x + m_aimDirection.x * 26.f, m_position.y + m_aimDirection.y * 26.f});
}

sf::Vector2f Player::getPosition() const
{
	return m_position;
}

sf::Vector2f Player::getAimDirection() const
{
	return m_aimDirection;
}

float Player::getCurrentHealth() const
{
	return m_health.getCurrentHealth();
}

float Player::getMaximumHealth() const
{
	return m_health.getMaximumHealth();
}

bool Player::isAlive() const
{
	return m_health.isAlive();
}

void Player::setAimTarget(sf::Vector2f worldPosition)
{
	setAimDirection({worldPosition.x - m_position.x, worldPosition.y - m_position.y});
}

void Player::setAimDirection(sf::Vector2f direction)
{
	const float lengthSquared = direction.x * direction.x + direction.y * direction.y;
	if (lengthSquared > 0.0001f)
	{
		const float inverseLength = 1.f / std::sqrt(lengthSquared);
		m_aimDirection = {direction.x * inverseLength, direction.y * inverseLength};
	}

	m_aimMarker.setPosition({m_position.x + m_aimDirection.x * 26.f, m_position.y + m_aimDirection.y * 26.f});
}

void Player::applyDamage(float amount)
{
	m_health.applyDamage(amount);
}

void Player::update(sf::Vector2f movementDirection, float deltaSeconds, const World& world)
{
	const float lengthSquared = movementDirection.x * movementDirection.x + movementDirection.y * movementDirection.y;
	if (lengthSquared > 1.f)
	{
		const float inverseLength = 1.f / std::sqrt(lengthSquared);
		movementDirection.x *= inverseLength;
		movementDirection.y *= inverseLength;
	}

	m_position.x += movementDirection.x * MoveSpeed * deltaSeconds;
	m_position.y += movementDirection.y * MoveSpeed * deltaSeconds;
	m_position = world.constrainPosition(m_position, Radius);
	m_body.setPosition(m_position);
	m_highlight.setPosition({m_position.x - 5.f, m_position.y - 6.f});
	m_aimMarker.setPosition({m_position.x + m_aimDirection.x * 26.f, m_position.y + m_aimDirection.y * 26.f});
}

void Player::render(sf::RenderTarget& target) const
{
	target.draw(m_body);
	target.draw(m_highlight);
	target.draw(m_aimMarker);
}
