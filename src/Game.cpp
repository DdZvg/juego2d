#include "Game.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>

#include <algorithm>

Game::Game()
	: m_window(sf::VideoMode({WindowWidth, WindowHeight}), "Ecos del Abismo - Prototipo Fase 2"),
	  m_player({World::Width / 2.f, World::Height / 2.f}),
	  m_camera({static_cast<float>(WindowWidth), static_cast<float>(WindowHeight)}, m_player.getPosition()),
	  m_combat({World::Width / 2.f + 250.f, World::Height / 2.f})
{
	m_window.setVerticalSyncEnabled(true);
}

void Game::run()
{
	while (m_window.isOpen())
	{
		processEvents();
		if (!m_window.isOpen())
		{
			break;
		}

		const float deltaSeconds = std::min(m_clock.restart().asSeconds(), 0.1f);
		update(deltaSeconds);
		render();
	}
}

void Game::processEvents()
{
	while (const std::optional<sf::Event> event = m_window.pollEvent())
	{
		m_input.processEvent(*event);

		if (const auto* resized = event->getIf<sf::Event::Resized>())
		{
			m_camera.setViewportSize({
				static_cast<float>(resized->size.x),
				static_cast<float>(resized->size.y)
			});
		}

		if (m_input.shouldClose())
		{
			m_window.close();
		}
	}
}

void Game::update(float deltaSeconds)
{
	m_player.update(m_input.getMovementDirection(), deltaSeconds, m_world);
	m_camera.update(m_player.getPosition(), deltaSeconds, m_world.getSize());

	const sf::Vector2f aimWorldPosition = m_input.getMouseWorldPosition(m_window, m_camera.getView());
	m_player.setAimTarget(aimWorldPosition);
	m_combat.update(deltaSeconds, m_player);

	if (m_input.consumeMeleePressed())
	{
		m_combat.tryMelee(m_player, aimWorldPosition);
	}
	if (m_input.consumeShootPressed())
	{
		m_combat.tryShoot(m_player, aimWorldPosition);
	}
}

void Game::render()
{
	m_window.clear(sf::Color(18, 16, 24));
	m_window.setView(m_camera.getView());
	m_world.render(m_window, m_camera.getView());
	m_combat.render(m_window);
	m_player.render(m_window);
	m_window.setView(m_window.getDefaultView());
	renderHud();
	m_window.display();
}

void Game::renderHud()
{
	constexpr float BarWidth = 220.f;
	constexpr float BarHeight = 16.f;
	constexpr float Margin = 18.f;

	sf::RectangleShape background({BarWidth, BarHeight});
	background.setPosition({Margin, Margin});
	background.setFillColor(sf::Color(55, 34, 40));
	m_window.draw(background);

	const float healthRatio = m_player.getCurrentHealth() / m_player.getMaximumHealth();
	sf::RectangleShape healthBar({BarWidth * healthRatio, BarHeight});
	healthBar.setPosition({Margin, Margin});
	healthBar.setFillColor(sf::Color(104, 166, 103));
	m_window.draw(healthBar);
}
