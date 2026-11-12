#include "Player.h"

#include "World.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace
{
std::size_t animationIndex(PlayerAnimationState state)
{
	return static_cast<std::size_t>(state);
}

std::size_t directionalRow(FacingDirection facing)
{
	switch (facing)
	{
		case FacingDirection::Up: return 0;
		case FacingDirection::UpRight: return 1;
		case FacingDirection::Right: return 2;
		case FacingDirection::DownRight: return 3;
		case FacingDirection::Down: return 4;
		case FacingDirection::DownLeft: return 5;
		case FacingDirection::Left: return 6;
		case FacingDirection::UpLeft: return 7;
	}

	return 0;
}

bool isFacingUp(FacingDirection facing)
{
	return facing == FacingDirection::Up || facing == FacingDirection::UpRight ||
		facing == FacingDirection::UpLeft;
}

bool isFacingSide(FacingDirection facing)
{
	return facing == FacingDirection::Right || facing == FacingDirection::Left;
}
}

Player::Player(sf::Vector2f startPosition)
	: m_position(startPosition),
	  m_health(MaximumHealth),
	  m_sprite(m_textures[static_cast<std::size_t>(SheetId::Idle)]),
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

	setupAnimations();
}

bool Player::loadSheetWithAlphaKey(SheetId id, const std::string& filePath)
{
	sf::Image image;
	if (!image.loadFromFile(filePath))
	{
		return false;
	}

	const sf::Vector2u size = image.getSize();
	bool hasTransparency = false;
	for (unsigned int y = 0; y < size.y && !hasTransparency; ++y)
	{
		for (unsigned int x = 0; x < size.x; ++x)
		{
			if (image.getPixel({x, y}).a < 255)
			{
				hasTransparency = true;
				break;
			}
		}
	}

	if (!hasTransparency)
	{
		for (unsigned int y = 0; y < size.y; ++y)
		{
			for (unsigned int x = 0; x < size.x; ++x)
			{
				sf::Color pixel = image.getPixel({x, y});
				const int red = pixel.r;
				const int green = pixel.g;
				const int blue = pixel.b;
				const int maximum = std::max({red, green, blue});
				const int minimum = std::min({red, green, blue});
				const int luminance = (red + green + blue) / 3;
				if ((maximum - minimum) <= 12 && luminance >= 36 && luminance <= 68)
				{
					pixel.a = 0;
					image.setPixel({x, y}, pixel);
				}
			}
		}
	}

	sf::Texture& texture = m_textures[static_cast<std::size_t>(id)];
	if (!texture.loadFromImage(image))
	{
		return false;
	}

	texture.setSmooth(true);
	return true;
}

bool Player::loadAssets(const std::string& folder)
{
	bool loaded = true;
	loaded &= loadSheetWithAlphaKey(SheetId::Idle, folder + "/sheet_idle.png");
	loaded &= loadSheetWithAlphaKey(SheetId::Locomotion, folder + "/sheet_locomotion.png");
	loaded &= loadSheetWithAlphaKey(SheetId::Combat, folder + "/sheet_combat.png");
	loaded &= loadSheetWithAlphaKey(SheetId::States, folder + "/sheet_states.png");

	m_hasDirectionalRunSprites = loadSheetWithAlphaKey(SheetId::LocomotionEight, folder + "/sheet_locomotion_8dir.png");
	m_hasDirectionalIdleSprites = loadSheetWithAlphaKey(SheetId::IdleEight, folder + "/sheet_idle_8dir.png");
	m_hasSprites = loaded;
	return loaded;
}

void Player::setupAnimations()
{
	Animation idle;
	idle.addFrame(sf::IntRect({0, 0}, {DirectionalFrameSize, DirectionalFrameSize}), 0.45f);
	idle.setLoop(true);
	m_animations[animationIndex(PlayerAnimationState::Idle)] = idle;

	Animation run;
	for (int frame = 0; frame < 6; ++frame)
	{
		run.addFrame(sf::IntRect({frame * DirectionalFrameSize, 0}, {DirectionalFrameSize, DirectionalFrameSize}), 0.10f);
	}
	run.setLoop(true);
	m_animations[animationIndex(PlayerAnimationState::Run)] = run;

	Animation melee;
	melee.addFrame(sf::IntRect({57, 54}, {378, 308}), 0.08f);
	melee.addFrame(sf::IntRect({493, 54}, {389, 308}), 0.12f);
	melee.addFrame(sf::IntRect({57, 421}, {378, 286}), 0.10f);
	melee.setLoop(false);
	m_animations[animationIndex(PlayerAnimationState::AttackMelee)] = melee;

	Animation ranged;
	ranged.addFrame(sf::IntRect({493, 421}, {389, 286}), 0.10f);
	ranged.addFrame(sf::IntRect({940, 421}, {378, 286}), 0.14f);
	ranged.setLoop(false);
	m_animations[animationIndex(PlayerAnimationState::AttackRanged)] = ranged;

	Animation hurt;
	hurt.addFrame(sf::IntRect({511, 54}, {370, 630}), 0.22f);
	hurt.setLoop(false);
	m_animations[animationIndex(PlayerAnimationState::Hurt)] = hurt;

	Animation death;
	death.addFrame(sf::IntRect({963, 54}, {355, 630}), 0.50f);
	death.setLoop(false);
	m_animations[animationIndex(PlayerAnimationState::Death)] = death;
}

Animation& Player::getAnimation(PlayerAnimationState state)
{
	return m_animations[animationIndex(state)];
}

const Animation& Player::getAnimation(PlayerAnimationState state) const
{
	return m_animations[animationIndex(state)];
}

void Player::changeState(PlayerAnimationState newState)
{
	if (m_state == newState)
	{
		return;
	}

	m_state = newState;
	getAnimation(m_state).reset();
}

void Player::updateFacingFromAim()
{
	updateFacingFromDirection(m_aimDirection);
}

void Player::updateFacingFromDirection(sf::Vector2f direction)
{
	constexpr float DiagonalThreshold = 0.41421356f;
	const float absoluteX = std::abs(direction.x);
	const float absoluteY = std::abs(direction.y);
	if (absoluteX <= 0.0001f && absoluteY <= 0.0001f)
	{
		return;
	}

	if (absoluteX < absoluteY * DiagonalThreshold)
	{
		m_facing = direction.y < 0.f ? FacingDirection::Up : FacingDirection::Down;
	}
	else if (absoluteY < absoluteX * DiagonalThreshold)
	{
		m_facing = direction.x < 0.f ? FacingDirection::Left : FacingDirection::Right;
	}
	else if (direction.x < 0.f)
	{
		m_facing = direction.y < 0.f ? FacingDirection::UpLeft : FacingDirection::DownLeft;
	}
	else
	{
		m_facing = direction.y < 0.f ? FacingDirection::UpRight : FacingDirection::DownRight;
	}
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

bool Player::hasSprites() const
{
	return m_hasSprites;
}

PlayerAnimationState Player::getAnimationState() const
{
	return m_state;
}

FacingDirection Player::getFacing() const
{
	return m_facing;
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

	if (!m_isMoving || m_state != PlayerAnimationState::Run)
	{
		updateFacingFromAim();
	}
	m_aimMarker.setPosition({m_position.x + m_aimDirection.x * 26.f, m_position.y + m_aimDirection.y * 26.f});
}

void Player::applyDamage(float amount)
{
	if (!isAlive())
	{
		return;
	}

	m_health.applyDamage(amount);
	if (!isAlive())
	{
		changeState(PlayerAnimationState::Death);
	}
	else if (amount > 0.f)
	{
		changeState(PlayerAnimationState::Hurt);
	}
}

void Player::triggerMeleeAttack()
{
	if (m_state == PlayerAnimationState::Death)
	{
		return;
	}

	changeState(PlayerAnimationState::AttackMelee);
	updateFacingFromAim();
}

void Player::triggerRangedAttack()
{
	if (m_state == PlayerAnimationState::Death)
	{
		return;
	}

	changeState(PlayerAnimationState::AttackRanged);
	updateFacingFromAim();
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

	if (isAlive())
	{
		const sf::Vector2f displacement{
			movementDirection.x * MoveSpeed * deltaSeconds,
			movementDirection.y * MoveSpeed * deltaSeconds
		};
		m_position = world.moveWithCollisions(m_position, displacement, Radius);
	}

	m_body.setPosition(m_position);
	m_highlight.setPosition({m_position.x - 5.f, m_position.y - 6.f});
	m_aimMarker.setPosition({m_position.x + m_aimDirection.x * 26.f, m_position.y + m_aimDirection.y * 26.f});

	const bool moving = lengthSquared > 0.0001f;
	m_isMoving = isAlive() && moving;
	const bool actionUsesAim = m_state == PlayerAnimationState::AttackMelee ||
		m_state == PlayerAnimationState::AttackRanged || m_state == PlayerAnimationState::Hurt;
	if (m_isMoving && !actionUsesAim)
	{
		updateFacingFromDirection(movementDirection);
	}
	else
	{
		updateFacingFromAim();
	}
	const PlayerAnimationState baseState = moving ? PlayerAnimationState::Run : PlayerAnimationState::Idle;

	if (m_state == PlayerAnimationState::Death)
	{
		getAnimation(PlayerAnimationState::Death).update(deltaSeconds);
	}
	else if (m_state == PlayerAnimationState::AttackMelee ||
			 m_state == PlayerAnimationState::AttackRanged ||
			 m_state == PlayerAnimationState::Hurt)
	{
		Animation& animation = getAnimation(m_state);
		animation.update(deltaSeconds);
		if (animation.isFinished())
		{
			changeState(baseState);
			getAnimation(m_state).update(deltaSeconds);
		}
	}
	else
	{
		changeState(baseState);
		getAnimation(m_state).update(deltaSeconds);
	}

	if (m_hasSprites)
	{
		const sf::IntRect& animatedRect = getAnimation(m_state).getCurrentRect();
		sf::IntRect frameRect = animatedRect;
		const bool useDirectionalRun = m_state == PlayerAnimationState::Run && m_hasDirectionalRunSprites;
		const bool useDirectionalIdle = m_state == PlayerAnimationState::Idle && m_hasDirectionalIdleSprites;
		const bool useDirectionalFrame = useDirectionalRun || useDirectionalIdle;

		if (useDirectionalRun)
		{
			const int frameX = static_cast<int>(getAnimation(m_state).getCurrentFrameIndex()) * DirectionalFrameSize;
			const int frameY = static_cast<int>(directionalRow(m_facing)) * DirectionalFrameSize;
			frameRect = sf::IntRect({frameX, frameY}, {DirectionalFrameSize, DirectionalFrameSize});
		}
		else if (useDirectionalIdle)
		{
			const int frameY = static_cast<int>(directionalRow(m_facing)) * DirectionalFrameSize;
			frameRect = sf::IntRect({0, frameY}, {DirectionalFrameSize, DirectionalFrameSize});
		}
		else if (m_state == PlayerAnimationState::Run)
		{
			static constexpr std::array<sf::IntRect, 2> LegacyRunFrames{
				sf::IntRect({80, 390}, {210, 290}),
				sf::IntRect({295, 390}, {260, 290})
			};
			frameRect = LegacyRunFrames[getAnimation(m_state).getCurrentFrameIndex() % LegacyRunFrames.size()];
		}
		else if (m_state == PlayerAnimationState::Idle)
		{
			if (isFacingUp(m_facing))
			{
				frameRect = sf::IntRect({510, 54}, {371, 630});
			}
			else if (isFacingSide(m_facing))
			{
				frameRect = sf::IntRect({963, 54}, {355, 630});
			}
		}

		m_sprite.setTexture(getActiveTexture(), true);
		m_sprite.setTextureRect(frameRect);
		if (useDirectionalFrame)
		{
			m_sprite.setOrigin({DirectionalFrameSize * 0.5f, DirectionalFrameSize - 4.f});
		}
		else
		{
			m_sprite.setOrigin({frameRect.size.x * 0.5f, frameRect.size.y * 0.88f});
		}
		m_sprite.setPosition(m_position);

		const float scale = SpriteHeight / static_cast<float>(frameRect.size.y);
		const bool facingLeft = m_facing == FacingDirection::Left || m_facing == FacingDirection::UpLeft ||
			m_facing == FacingDirection::DownLeft;
		const bool flip = !useDirectionalFrame && facingLeft;
		m_sprite.setScale({flip ? -scale : scale, scale});
		m_sprite.setColor(sf::Color::White);
	}
}

const sf::Texture& Player::getActiveTexture() const
{
	switch (m_state)
	{
		case PlayerAnimationState::AttackMelee:
		case PlayerAnimationState::AttackRanged:
			return m_textures[static_cast<std::size_t>(SheetId::Combat)];
		case PlayerAnimationState::Hurt:
		case PlayerAnimationState::Death:
			return m_textures[static_cast<std::size_t>(SheetId::States)];
		case PlayerAnimationState::Run:
			return m_textures[static_cast<std::size_t>(m_hasDirectionalRunSprites ? SheetId::LocomotionEight : SheetId::Locomotion)];
		case PlayerAnimationState::Idle:
		default:
			return m_textures[static_cast<std::size_t>(m_hasDirectionalIdleSprites ? SheetId::IdleEight : SheetId::Idle)];
	}
}

void Player::render(sf::RenderTarget& target) const
{
	sf::CircleShape shadow(16.f);
	shadow.setScale({1.2f, 0.55f});
	shadow.setOrigin({16.f, 16.f});
	shadow.setPosition({m_position.x, m_position.y + 10.f});
	shadow.setFillColor(sf::Color(0, 0, 0, 120));
	target.draw(shadow);

	if (m_hasSprites)
	{
		target.draw(m_sprite);
	}
	else
	{
		target.draw(m_body);
		target.draw(m_highlight);
	}

	target.draw(m_aimMarker);
}
