#include "InputManager.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

void InputManager::processEvent(const sf::Event& event)
{
	if (event.is<sf::Event::Closed>())
	{
		m_closeRequested = true;
	}

	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->code == sf::Keyboard::Key::Q)
		{
			m_abilityPressed = true;
		}
		else if (keyPressed->code == sf::Keyboard::Key::Num1)
		{
			m_weaponSlotRequest = 0;
		}
		else if (keyPressed->code == sf::Keyboard::Key::Num2)
		{
			m_weaponSlotRequest = 1;
		}
	}

	if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>())
	{
		if (mousePressed->button == sf::Mouse::Button::Right)
		{
			m_meleePressed = true;
		}
		else if (mousePressed->button == sf::Mouse::Button::Left)
		{
			m_shootPressed = true;
		}
	}
}

bool InputManager::shouldClose() const
{
	return m_closeRequested;
}

bool InputManager::consumeMeleePressed()
{
	const bool pressed = m_meleePressed;
	m_meleePressed = false;
	return pressed;
}

bool InputManager::consumeShootPressed()
{
	const bool pressed = m_shootPressed;
	m_shootPressed = false;
	return pressed;
}

bool InputManager::consumeAbilityPressed()
{
	const bool pressed = m_abilityPressed;
	m_abilityPressed = false;
	return pressed;
}

std::optional<std::size_t> InputManager::consumeWeaponSlotRequest()
{
	const std::optional<std::size_t> slot = m_weaponSlotRequest;
	m_weaponSlotRequest.reset();
	return slot;
}

sf::Vector2f InputManager::getMovementDirection() const
{
	sf::Vector2f direction{};

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		direction.x -= 1.f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		direction.x += 1.f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
	{
		direction.y -= 1.f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
	{
		direction.y += 1.f;
	}

	return direction;
}

sf::Vector2f InputManager::getMouseWorldPosition(const sf::RenderWindow& window, const sf::View& view) const
{
	return window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
}
