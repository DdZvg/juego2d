#include "Game.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>

namespace
{
bool loadPlayerSprites(Player& player)
{
	const std::array<std::string, 4> candidates{
		"assets/sprites/player",
		"../assets/sprites/player",
		"../../assets/sprites/player",
		"../../../assets/sprites/player"
	};

	for (const std::string& candidate : candidates)
	{
		const std::filesystem::path folder(candidate);
		if (!std::filesystem::exists(folder / "sheet_idle.png"))
		{
			continue;
		}

		if (player.loadAssets(folder.string()))
		{
			return true;
		}
	}

	return false;
}

void loadWeaponData()
{
	const std::array<std::string, 4> candidates{
		"assets/data/weapons.txt",
		"../assets/data/weapons.txt",
		"../../assets/data/weapons.txt",
		"../../../assets/data/weapons.txt"
	};

	for (const std::string& candidate : candidates)
	{
		if (std::filesystem::exists(candidate) && loadWeaponDefinitions(candidate))
		{
			return;
		}
	}
}

void loadEnemyData()
{
	const std::array<std::string, 4> candidates{
		"assets/data/enemies.txt",
		"../assets/data/enemies.txt",
		"../../assets/data/enemies.txt",
		"../../../assets/data/enemies.txt"
	};

	for (const std::string& candidate : candidates)
	{
		if (std::filesystem::exists(candidate) && loadEnemyDefinitions(candidate))
		{
			return;
		}
	}
}
}

Game::Game()
	: m_window(sf::VideoMode({WindowWidth, WindowHeight}), "Ecos del Abismo - Prototipo Fase 4"),
	  m_player({World::Width / 2.f, World::Height / 2.f}),
	  m_camera({static_cast<float>(WindowWidth), static_cast<float>(WindowHeight)}, m_player.getPosition()),
	  m_combat({World::Width / 2.f + 250.f, World::Height / 2.f})
{
	m_window.setVerticalSyncEnabled(true);

	loadWeaponData();
	loadEnemyData();
	m_hasHudFont = loadHudFont();

	if (!loadPlayerSprites(m_player))
	{
		std::cerr << "Aviso: no se encontraron las hojas de sprites del jugador. "
					 "Se usara el dibujo geometrico provisional.\n";
	}

	m_combat.startEncounter(m_player.getPosition(), m_world);
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
	if (m_input.consumeRestartPressed())
	{
		m_combat.restartEncounter(m_player.getPosition(), m_world);
		m_player.restoreFullHealth();
	}

	m_player.update(m_input.getMovementDirection(), deltaSeconds, m_world);
	m_camera.update(m_player.getPosition(), deltaSeconds, m_world.getSize());

	const sf::Vector2f aimWorldPosition = m_input.getMouseWorldPosition(m_window, m_camera.getView());
	m_player.setAimTarget(aimWorldPosition);

	if (const auto weaponSlot = m_input.consumeWeaponSlotRequest())
	{
		m_combat.equipWeaponSlot(*weaponSlot);
	}

	m_combat.update(deltaSeconds, m_player, m_world);

	if (m_player.isAlive() && m_input.consumeAbilityPressed())
	{
		m_combat.tryAbyssalMark(m_player.getPosition(), aimWorldPosition);
	}
	else if (!m_player.isAlive())
	{
		m_input.consumeAbilityPressed();
	}

	if (m_player.isAlive() && m_input.consumeMeleePressed())
	{
		if (m_combat.tryMelee(m_player, aimWorldPosition))
		{
			m_player.triggerMeleeAttack();
		}
	}
	if (m_player.isAlive() && m_input.consumeShootPressed())
	{
		if (m_combat.tryShoot(m_player, aimWorldPosition))
		{
			m_player.triggerRangedAttack();
		}
	}
	else if (!m_player.isAlive())
	{
		m_input.consumeMeleePressed();
		m_input.consumeShootPressed();
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

	constexpr float SlotSize = 24.f;
	constexpr float SlotGap = 8.f;
	const WeaponInventory& inventory = m_combat.getInventory();
	for (std::size_t slot = 0; slot < inventory.size(); ++slot)
	{
		const bool equipped = slot == inventory.getEquippedSlot();
		sf::RectangleShape weaponSlot({SlotSize, SlotSize});
		weaponSlot.setPosition({Margin + static_cast<float>(slot) * (SlotSize + SlotGap), Margin + BarHeight + SlotGap});
		weaponSlot.setFillColor(inventory.getWeaponAt(slot) == WeaponId::RustyBlade
			? sf::Color(177, 115, 77)
			: sf::Color(131, 105, 190));
		weaponSlot.setOutlineColor(equipped ? sf::Color(250, 220, 150) : sf::Color(78, 74, 91));
		weaponSlot.setOutlineThickness(equipped ? 3.f : 1.f);
		m_window.draw(weaponSlot);
	}

	const float abilityReadyRatio = 1.f - m_combat.getAbyssalMarkCooldownRemaining() / CombatSystem::AbyssalMarkCooldown;
	sf::RectangleShape abilityBackground({BarWidth, 6.f});
	abilityBackground.setPosition({Margin, Margin + BarHeight + SlotSize + SlotGap * 2.f});
	abilityBackground.setFillColor(sf::Color(55, 48, 69));
	m_window.draw(abilityBackground);

	sf::RectangleShape abilityCooldown({BarWidth * std::clamp(abilityReadyRatio, 0.f, 1.f), 6.f});
	abilityCooldown.setPosition(abilityBackground.getPosition());
	abilityCooldown.setFillColor(sf::Color(159, 119, 205));
	m_window.draw(abilityCooldown);

	if (m_hasHudFont)
	{
		std::string status;
		if (m_combat.isEncounterComplete())
		{
			status = "Encuentro completado - pulsa R para reiniciar";
		}
		else if (m_combat.isWaitingForNextWave())
		{
			status = "Oleada " + std::to_string(m_combat.getCurrentWaveNumber()) + "/" +
				std::to_string(m_combat.getWaveCount()) + " despejada - siguiente en " +
				std::to_string(static_cast<int>(std::ceil(m_combat.getNextWaveCountdown()))) + " s";
		}
		else
		{
			status = "Oleada " + std::to_string(m_combat.getCurrentWaveNumber()) + "/" +
				std::to_string(m_combat.getWaveCount()) + " - Enemigos: " +
				std::to_string(m_combat.getEnemyCount());
		}

		const float statusY = Margin + BarHeight + SlotSize + SlotGap * 2.f + 10.f;
		sf::Text statusText(m_hudFont, status, 15);
		statusText.setPosition({Margin, statusY});
		statusText.setFillColor(sf::Color(220, 212, 236));
		m_window.draw(statusText);

		sf::Text hintText(m_hudFont, "R: reiniciar encuentro", 13);
		hintText.setPosition({Margin, statusY + 22.f});
		hintText.setFillColor(sf::Color(150, 142, 168));
		m_window.draw(hintText);
	}
}

bool Game::loadHudFont()
{
	const std::array<std::string, 5> candidates{
		"assets/fonts/hud.ttf",
		"../assets/fonts/hud.ttf",
		"../../assets/fonts/hud.ttf",
		"C:/Windows/Fonts/segoeui.ttf",
		"C:/Windows/Fonts/arial.ttf"
	};

	for (const std::string& candidate : candidates)
	{
		if (std::filesystem::exists(candidate) && m_hudFont.openFromFile(candidate))
		{
			return true;
		}
	}

	return false;
}
