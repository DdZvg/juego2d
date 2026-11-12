#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>

#include <cstddef>
#include <optional>

class InputManager
{
public:
	void processEvent(const sf::Event& event);
	bool shouldClose() const;
	bool consumeMeleePressed();
	bool consumeShootPressed();
	bool consumeAbilityPressed();
	std::optional<std::size_t> consumeWeaponSlotRequest();
	sf::Vector2f getMovementDirection() const;
	sf::Vector2f getMouseWorldPosition(const sf::RenderWindow& window, const sf::View& view) const;

private:
	bool m_closeRequested = false;
	bool m_meleePressed = false;
	bool m_shootPressed = false;
	bool m_abilityPressed = false;
	std::optional<std::size_t> m_weaponSlotRequest;
};
