#pragma once

#include "Animation.h"
#include "HealthComponent.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>

#include <array>
#include <cstddef>
#include <string>

class World;

enum class PlayerAnimationState
{
	Idle,
	Run,
	AttackMelee,
	AttackRanged,
	Hurt,
	Death
};

enum class FacingDirection
{
	Down,
	Up,
	Right,
	Left,
	UpRight,
	DownRight,
	DownLeft,
	UpLeft
};

class Player
{
public:
	static constexpr float Radius = 18.f;
	static constexpr float MoveSpeed = 280.f;
	static constexpr float MaximumHealth = 100.f;
	static constexpr float SpriteHeight = 58.f;

	explicit Player(sf::Vector2f startPosition);

	bool loadAssets(const std::string& folder);

	sf::Vector2f getPosition() const;
	sf::Vector2f getAimDirection() const;
	float getCurrentHealth() const;
	float getMaximumHealth() const;
	bool isAlive() const;
	bool hasSprites() const;
	PlayerAnimationState getAnimationState() const;
	FacingDirection getFacing() const;
	void setAimTarget(sf::Vector2f worldPosition);
	void setAimDirection(sf::Vector2f direction);
	void applyDamage(float amount);
	void triggerMeleeAttack();
	void triggerRangedAttack();
	void update(sf::Vector2f movementDirection, float deltaSeconds, const World& world);
	void render(sf::RenderTarget& target) const;

private:
	enum class SheetId
	{
		Idle = 0,
		Locomotion = 1,
		Combat = 2,
		States = 3,
		LocomotionEight = 4,
		IdleEight = 5
	};

	static constexpr std::size_t AnimationCount = 6;
	static constexpr int DirectionalFrameSize = 128;

	bool loadSheetWithAlphaKey(SheetId id, const std::string& filePath);
	void setupAnimations();
	void changeState(PlayerAnimationState newState);
	void updateFacingFromAim();
	void updateFacingFromDirection(sf::Vector2f direction);
	const sf::Texture& getActiveTexture() const;
	Animation& getAnimation(PlayerAnimationState state);
	const Animation& getAnimation(PlayerAnimationState state) const;

	sf::Vector2f m_position;
	sf::Vector2f m_aimDirection{1.f, 0.f};
	HealthComponent m_health;
	PlayerAnimationState m_state = PlayerAnimationState::Idle;
	FacingDirection m_facing = FacingDirection::Down;
	bool m_hasSprites = false;
	bool m_hasDirectionalRunSprites = false;
	bool m_hasDirectionalIdleSprites = false;
	bool m_isMoving = false;

	std::array<sf::Texture, 6> m_textures;
	std::array<Animation, AnimationCount> m_animations;
	sf::Sprite m_sprite;

	sf::CircleShape m_body;
	sf::CircleShape m_highlight;
	sf::CircleShape m_aimMarker;
};
