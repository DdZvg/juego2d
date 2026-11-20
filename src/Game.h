#pragma once

#include "Camera.h"
#include "CombatSystem.h"
#include "InputManager.h"
#include "Player.h"
#include "World.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>

class Game
{
public:
	Game();
	void run();

private:
	void processEvents();
	void update(float deltaSeconds);
	void render();
	void renderHud();
	bool loadHudFont();

	static constexpr unsigned int WindowWidth = 960;
	static constexpr unsigned int WindowHeight = 640;

	sf::RenderWindow m_window;
	World m_world;
	Player m_player;
	Camera m_camera;
	InputManager m_input;
	CombatSystem m_combat;
	sf::Clock m_clock;
	sf::Font m_hudFont;
	bool m_hasHudFont = false;
};
