#include "Animation.h"
#include "Camera.h"
#include "CombatSystem.h"
#include "HealthComponent.h"
#include "InputManager.h"
#include "Player.h"
#include "Projectile.h"
#include "StatusEffectComponent.h"
#include "Weapon.h"
#include "World.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
int failures = 0;

void check(bool condition, const std::string& message)
{
	if (!condition)
	{
		std::cerr << "FAIL: " << message << '\n';
		++failures;
	}
}

bool near(float actual, float expected, float tolerance = 0.05f)
{
	return std::abs(actual - expected) <= tolerance;
}

void testMovementAndWorldBounds()
{
	const World world;
	const sf::Vector2f clearStart{300.f, 300.f};
	Player horizontal(clearStart);
	horizontal.update({1.f, 0.f}, 1.f, world);
	check(near(horizontal.getPosition().x, clearStart.x + Player::MoveSpeed), "horizontal movement uses speed times delta time");

	Player thirtyFps(clearStart);
	Player sixtyFps(clearStart);
	for (int frame = 0; frame < 30; ++frame)
	{
		thirtyFps.update({1.f, 0.f}, 1.f / 30.f, world);
	}
	for (int frame = 0; frame < 60; ++frame)
	{
		sixtyFps.update({1.f, 0.f}, 1.f / 60.f, world);
	}
	check(near(thirtyFps.getPosition().x, sixtyFps.getPosition().x), "movement distance is independent of 30 versus 60 FPS");

	Player diagonal(clearStart);
	diagonal.update({1.f, -1.f}, 1.f, world);
	const sf::Vector2f diagonalOffset = diagonal.getPosition() - clearStart;
	check(near(std::sqrt(diagonalOffset.x * diagonalOffset.x + diagonalOffset.y * diagonalOffset.y), Player::MoveSpeed),
		"diagonal movement is normalized to the cardinal speed");

	Player topLeft({Player::Radius, Player::Radius});
	topLeft.update({-1.f, -1.f}, 1.f, world);
	check(near(topLeft.getPosition().x, Player::Radius) && near(topLeft.getPosition().y, Player::Radius),
		"player cannot leave the top or left world boundary");

	Player bottomRight({World::Width - Player::Radius, World::Height - Player::Radius});
	bottomRight.update({1.f, 1.f}, 1.f, world);
	check(near(bottomRight.getPosition().x, World::Width - Player::Radius) &&
		near(bottomRight.getPosition().y, World::Height - Player::Radius),
		"player cannot leave the bottom or right world boundary");
}

void testWallCollision()
{
	const World world;
	const std::vector<sf::FloatRect>& obstacles = world.getObstacles();
	check(!obstacles.empty(), "world contains test walls");

	const sf::FloatRect wall = obstacles.front();
	const float wallCenterX = wall.position.x + wall.size.x / 2.f;
	const float wallCenterY = wall.position.y + wall.size.y / 2.f;

	const sf::FloatRect spawnBounds(
		{World::Width / 2.f - Player::Radius, World::Height / 2.f - Player::Radius},
		{Player::Radius * 2.f, Player::Radius * 2.f});
	check(!world.collides(spawnBounds), "player spawn is clear of test walls");

	const sf::Vector2f fromLeft({wall.position.x - Player::Radius - 5.f, wallCenterY});
	const sf::Vector2f stoppedRight = world.moveWithCollisions(fromLeft, {1000.f, 0.f}, Player::Radius);
	check(near(stoppedRight.x, wall.position.x - Player::Radius) && near(stoppedRight.y, wallCenterY),
		"high-speed movement from the left cannot tunnel through a wall");

	const sf::Vector2f fromRight({wall.position.x + wall.size.x + Player::Radius + 5.f, wallCenterY});
	const sf::Vector2f stoppedLeft = world.moveWithCollisions(fromRight, {-1000.f, 0.f}, Player::Radius);
	check(near(stoppedLeft.x, wall.position.x + wall.size.x + Player::Radius),
		"high-speed movement from the right cannot tunnel through a wall");

	const sf::Vector2f fromAbove({wallCenterX, wall.position.y - Player::Radius - 5.f});
	const sf::Vector2f stoppedDown = world.moveWithCollisions(fromAbove, {0.f, 1000.f}, Player::Radius);
	check(near(stoppedDown.y, wall.position.y - Player::Radius),
		"movement from above stops at the wall");

	const sf::Vector2f fromBelow({wallCenterX, wall.position.y + wall.size.y + Player::Radius + 5.f});
	const sf::Vector2f stoppedUp = world.moveWithCollisions(fromBelow, {0.f, -1000.f}, Player::Radius);
	check(near(stoppedUp.y, wall.position.y + wall.size.y + Player::Radius),
		"movement from below stops at the wall");

	Player blockedPlayer(fromLeft);
	for (int frame = 0; frame < 20; ++frame)
	{
		blockedPlayer.update({1.f, 0.f}, 0.1f, world);
	}
	check(near(blockedPlayer.getPosition().x, wall.position.x - Player::Radius),
		"player remains blocked by a wall over multiple frames");

	Player slidingPlayer({wall.position.x - Player::Radius - 1.f, wallCenterY});
	const float startY = slidingPlayer.getPosition().y;
	slidingPlayer.update({1.f, 1.f}, 0.1f, world);
	check(near(slidingPlayer.getPosition().x, wall.position.x - Player::Radius) &&
			slidingPlayer.getPosition().y > startY,
		"player slides along a wall instead of sticking to it");
}

void testCameraSmoothingAndBounds()
{
	const World world;
	Camera camera({960.f, 640.f}, {1500.f, 1500.f});
	camera.update({1000.f, 1000.f}, 0.1f, world.getSize());
	check(camera.getView().getCenter().x < 1500.f && camera.getView().getCenter().x > 1000.f,
		"camera follows smoothly instead of snapping to its target");

	camera.update({0.f, 0.f}, 1.f, world.getSize());
	check(near(camera.getView().getCenter().x, 480.f) && near(camera.getView().getCenter().y, 320.f),
		"camera is clamped at the top-left world corner");

	camera.update({World::Width, World::Height}, 1.f, world.getSize());
	check(near(camera.getView().getCenter().x, World::Width - 480.f) &&
		near(camera.getView().getCenter().y, World::Height - 320.f),
		"camera is clamped at the bottom-right world corner");
}

void testMouseCombatInput()
{
	InputManager input;
	sf::Event leftClick(sf::Event::MouseButtonPressed{sf::Mouse::Button::Left, {120, 80}});
	input.processEvent(leftClick);
	check(input.consumeShootPressed(), "left mouse click requests a projectile shot");
	check(!input.consumeShootPressed() && !input.consumeMeleePressed(), "left click is consumed once and does not request melee");

	sf::Event rightClick(sf::Event::MouseButtonPressed{sf::Mouse::Button::Right, {120, 80}});
	input.processEvent(rightClick);
	check(input.consumeMeleePressed(), "right mouse click requests a melee attack");
	check(!input.consumeMeleePressed() && !input.consumeShootPressed(), "right click is consumed once and does not request a shot");

	sf::Event abilityKey(sf::Event::KeyPressed{sf::Keyboard::Key::Q});
	input.processEvent(abilityKey);
	check(input.consumeAbilityPressed() && !input.consumeAbilityPressed(), "Q requests the active ability once per key event");

	sf::Event secondWeaponKey(sf::Event::KeyPressed{sf::Keyboard::Key::Num2});
	input.processEvent(secondWeaponKey);
	const std::optional<std::size_t> slot = input.consumeWeaponSlotRequest();
	check(slot.has_value() && *slot == 1, "2 requests the second inventory weapon");
	check(!input.consumeWeaponSlotRequest().has_value(), "weapon selection request is consumed once");

	sf::Event firstWeaponKey(sf::Event::KeyPressed{sf::Keyboard::Key::Num1});
	input.processEvent(firstWeaponKey);
	const std::optional<std::size_t> firstSlot = input.consumeWeaponSlotRequest();
	check(firstSlot.has_value() && *firstSlot == 0, "1 requests the first inventory weapon");
}

void testHealthAndAim()
{
	HealthComponent health(100.f);
	health.applyDamage(35.f);
	check(near(health.getCurrentHealth(), 65.f) && health.isAlive(), "health component subtracts damage");
	health.applyDamage(1000.f);
	check(near(health.getCurrentHealth(), 0.f) && !health.isAlive(), "health cannot fall below zero and zero means dead");
	health.applyDamage(-10.f);
	check(near(health.getCurrentHealth(), 0.f), "negative damage does not restore health");

	Player player({1000.f, 1000.f});
	player.setAimTarget({1000.f, 1300.f});
	check(near(player.getAimDirection().x, 0.f) && near(player.getAimDirection().y, 1.f),
		"player aim direction is normalized toward a world-space target");
}

void testWeaponInventoryAndWeaponStats()
{
	WeaponInventory inventory;
	check(inventory.size() == 2, "expedition inventory starts with two distinct weapons");
	check(inventory.getEquippedWeapon().id == WeaponId::RustyBlade, "first weapon is equipped by default");
	check(inventory.getEquippedWeapon().meleeDamage != getWeaponDefinition(WeaponId::EchoWand).meleeDamage &&
		inventory.getEquippedWeapon().projectileSpeed != getWeaponDefinition(WeaponId::EchoWand).projectileSpeed,
		"weapon definitions configure different combat attributes");
	check(!inventory.add(WeaponId::RustyBlade), "inventory rejects duplicate weapons");
	check(inventory.equipSlot(1) && inventory.getEquippedWeapon().id == WeaponId::EchoWand,
		"inventory can equip the second weapon by slot");
	check(!inventory.equipSlot(2) && inventory.getEquippedSlot() == 1,
		"inventory rejects an invalid slot without changing the equipped weapon");

	const World world;
	Player bladePlayer({1000.f, 1000.f});
	CombatSystem bladeCombat({1110.f, 1000.f});
	check(bladeCombat.tryMelee(bladePlayer, {1300.f, 1000.f}) &&
		near(bladeCombat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth -
		getWeaponDefinition(WeaponId::RustyBlade).meleeDamage),
		"blade melee range and damage come from its weapon definition");

	Player wandPlayer({1000.f, 1000.f});
	CombatSystem wandCombat({1110.f, 1000.f});
	wandCombat.equipWeaponSlot(1);
	check(wandCombat.tryMelee(wandPlayer, {1300.f, 1000.f}) &&
		near(wandCombat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth),
		"shorter wand melee range does not hit an out-of-range enemy");

	Player cooldownPlayer({500.f, 500.f});
	CombatSystem bladeCooldown({650.f, 650.f});
	bladeCooldown.tryMelee(cooldownPlayer, {650.f, 650.f});
	bladeCooldown.update(0.5f, cooldownPlayer, world);
	check(bladeCooldown.tryMelee(cooldownPlayer, {650.f, 650.f}),
		"blade attack cooldown follows the configured duration");

	CombatSystem wandCooldown({650.f, 650.f});
	wandCooldown.equipWeaponSlot(1);
	wandCooldown.tryMelee(cooldownPlayer, {650.f, 650.f});
	wandCooldown.update(0.5f, cooldownPlayer, world);
	check(!wandCooldown.tryMelee(cooldownPlayer, {650.f, 650.f}),
		"wand attack remains on cooldown when its longer configured duration has not elapsed");
	wandCooldown.update(0.11f, cooldownPlayer, world);
	check(wandCooldown.tryMelee(cooldownPlayer, {650.f, 650.f}),
		"wand attack becomes available when its configured cooldown elapses");

	Player rangedPlayer({500.f, 500.f});
	CombatSystem bladeRangedCooldown({5000.f, 5000.f});
	bladeRangedCooldown.tryShoot(rangedPlayer, {1500.f, 500.f});
	bladeRangedCooldown.update(0.2f, rangedPlayer, world);
	check(bladeRangedCooldown.tryShoot(rangedPlayer, {1500.f, 500.f}),
		"blade projectile cooldown follows its configured duration");

	CombatSystem wandRangedCooldown({5000.f, 5000.f});
	wandRangedCooldown.equipWeaponSlot(1);
	wandRangedCooldown.tryShoot(rangedPlayer, {1500.f, 500.f});
	wandRangedCooldown.update(0.2f, rangedPlayer, world);
	check(!wandRangedCooldown.tryShoot(rangedPlayer, {1500.f, 500.f}),
		"wand projectile remains on cooldown when its longer configured duration has not elapsed");
	wandRangedCooldown.update(0.06f, rangedPlayer, world);
	check(wandRangedCooldown.tryShoot(rangedPlayer, {1500.f, 500.f}),
		"wand projectile becomes available when its configured cooldown elapses");
}

void testAbyssalMarkAndDamageSynergy()
{
	const World world;
	CombatSystem wrongDirection({1100.f, 1000.f});
	check(!wrongDirection.tryAbyssalMark({1000.f, 1000.f}, {1000.f, 1300.f}),
		"ability does not mark an enemy outside the aim cone");
	check(near(wrongDirection.getAbyssalMarkCooldownRemaining(), 0.f),
		"failed target selection does not consume the ability cooldown");
	CombatSystem outOfRange({1600.f, 1000.f});
	check(!outOfRange.tryAbyssalMark({1000.f, 1000.f}, {1600.f, 1000.f}),
		"ability does not mark an enemy beyond its configured range");

	Player player({1000.f, 1000.f});
	CombatSystem combat({1100.f, 1000.f});
	check(combat.tryAbyssalMark(player.getPosition(), {1300.f, 1000.f}), "Q ability marks an enemy in the aim cone");
	check(combat.getEnemy(0).isMarked() && near(combat.getEnemy(0).getMarkTimeRemaining(), CombatSystem::AbyssalMarkDuration),
		"abyssal mark starts with its configured duration");
	check(!combat.tryAbyssalMark(player.getPosition(), {1300.f, 1000.f}), "active ability observes its cooldown");
	check(near(combat.getAbyssalMarkCooldownRemaining(), CombatSystem::AbyssalMarkCooldown),
		"active ability exposes its remaining cooldown");

	check(combat.tryMelee(player, {1300.f, 1000.f}), "equipped weapon can attack a marked enemy");
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth -
		CombatSystem::MeleeDamage * getWeaponDefinition(WeaponId::RustyBlade).markedDamageMultiplier),
		"marked-target synergy modifies one hit's damage exactly once");

	StatusEffectComponent mark;
	mark.applyAbyssalMark(2.f);
	check(near(mark.calculateDamage(20.f, 1.5f), 30.f), "marked effect applies the configured damage multiplier");
	mark.update(1.5f);
	check(mark.isMarked(), "mark remains active before its duration expires");
	mark.update(0.6f);
	check(!mark.isMarked() && near(mark.calculateDamage(20.f, 1.5f), 20.f),
		"mark expires and no longer modifies damage");

	for (int frame = 0; frame < 60; ++frame)
	{
		combat.update(0.1f, player, world);
	}
	check(near(combat.getAbyssalMarkCooldownRemaining(), 0.f), "active ability cooldown expires using delta time");
	check(combat.tryAbyssalMark({900.f, 1000.f}, {1300.f, 1000.f}), "active ability can be used again after its cooldown");
}

void testMeleeRangeArcAndCooldown()
{
	const World world;
	Player player({1000.f, 1000.f});
	CombatSystem combat({1080.f, 1000.f});
	check(combat.tryMelee(player, {1300.f, 1000.f}), "first melee attack is accepted");
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth - CombatSystem::MeleeDamage),
		"melee attack damages an enemy inside its forward arc and range");
	check(!combat.tryMelee(player, {1300.f, 1000.f}), "melee cooldown prevents an immediate second attack");
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth - CombatSystem::MeleeDamage),
		"blocked melee attack does not apply extra damage");

	CombatSystem outOfRange({1200.f, 1000.f});
	Player outOfRangePlayer({1000.f, 1000.f});
	outOfRange.tryMelee(outOfRangePlayer, {1300.f, 1000.f});
	check(near(outOfRange.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth),
		"melee attack does not damage an enemy beyond its reach");

	CombatSystem behindPlayer({900.f, 1000.f});
	Player facingRight({1000.f, 1000.f});
	behindPlayer.tryMelee(facingRight, {1300.f, 1000.f});
	check(near(behindPlayer.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth),
		"melee attack does not hit an enemy behind the player");

	combat.update(CombatSystem::MeleeCooldown + 0.01f, player, world);
	check(combat.tryMelee(player, {1300.f, 1000.f}), "melee attack becomes available after its cooldown");
	combat.update(CombatSystem::MeleeCooldown + 0.01f, player, world);
	check(combat.tryMelee(player, {1300.f, 1000.f}), "melee attack can be repeated after each cooldown");
	combat.update(0.f, player, world);
	check(combat.getEnemyCount() == 0, "enemy is removed after its health reaches zero");
}

void testProjectileTrajectoryRangeAndCollision()
{
	const World world;
	Projectile projectile({0.f, 0.f}, {3.f, 4.f});
	projectile.update(0.1f);
	check(near(projectile.getPosition().x, 42.f) && near(projectile.getPosition().y, 56.f),
		"projectile normalizes its direction and applies speed using delta time");
	projectile.update(1.f);
	check(projectile.isExpired(), "projectile expires at its maximum travel range");

	Player player({1000.f, 1000.f});
	CombatSystem combat({1200.f, 1000.f});
	check(combat.tryShoot(player, {1400.f, 1000.f}), "first mouse-directed shot is accepted");
	check(!combat.tryShoot(player, {1400.f, 1000.f}), "projectile cooldown prevents an immediate second shot");
	combat.update(0.1f, player, world);
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth), "projectile deals no damage before reaching the enemy");
	combat.update(0.1f, player, world);
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth - Projectile::Damage),
		"projectile collision applies damage while crossing an enemy");
	check(combat.getProjectileCount() == 0, "projectile is removed after impact");

	for (int shot = 0; shot < 3; ++shot)
	{
		check(combat.tryShoot(player, {1400.f, 1000.f}), "projectile can be fired after its cooldown");
		combat.update(0.2f, player, world);
	}
	check(combat.getEnemyCount() == 0, "projectile damage can kill and remove an enemy");

	Player wandPlayer({1000.f, 1000.f});
	CombatSystem wandCombat({1300.f, 1000.f});
	check(wandCombat.equipWeaponSlot(1), "second weapon equips before shooting");
	check(wandCombat.tryShoot(wandPlayer, {1500.f, 1000.f}), "equipped wand fires its configured projectile");
	wandCombat.update(0.3f, wandPlayer, world);
	check(near(wandCombat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth -
		getWeaponDefinition(WeaponId::EchoWand).projectileDamage),
		"projectile uses the damage configured by the equipped weapon");
}

void testProjectileStopsAtWall()
{
	const World world;
	Player player({1700.f, 1500.f});
	CombatSystem combat({1900.f, 1500.f});
	check(combat.tryShoot(player, {1900.f, 1500.f}), "shot toward a wall is accepted");
	combat.update(0.1f, player, world);
	check(combat.getProjectileCount() == 0, "projectile is destroyed when it reaches a wall");
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth),
		"wall prevents the projectile from damaging an enemy behind it");
}

void testProjectileHitsEnemyBeforeWall()
{
	const World world;
	Player player({1600.f, 1500.f});
	CombatSystem combat({1770.f, 1500.f});
	check(combat.tryShoot(player, {1770.f, 1500.f}), "shot toward an enemy in front of a wall is accepted");
	for (int frame = 0; frame < 5; ++frame)
	{
		combat.update(0.05f, player, world);
	}
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth - Projectile::Damage),
		"enemy in front of a wall receives the projectile damage");
}

void testAnimationAdvanceLoopAndFinish()
{
	Animation looping;
	looping.addFrame(sf::IntRect({0, 0}, {10, 10}), 0.1f);
	looping.addFrame(sf::IntRect({10, 0}, {10, 10}), 0.1f);
	check(looping.getFrameCount() == 2, "animation stores all configured frames");
	check(looping.getCurrentFrameIndex() == 0, "animation starts on its first frame");

	looping.update(0.05f);
	check(looping.getCurrentFrameIndex() == 0, "animation keeps its frame before the duration elapses");
	looping.update(0.06f);
	check(looping.getCurrentFrameIndex() == 1, "animation advances to the next frame after the duration");
	looping.update(0.1f);
	check(looping.getCurrentFrameIndex() == 0 && !looping.isFinished(),
		"looping animation returns to its first frame instead of finishing");

	Animation once;
	once.addFrame(sf::IntRect({0, 0}, {10, 10}), 0.1f);
	once.addFrame(sf::IntRect({10, 0}, {10, 10}), 0.1f);
	once.setLoop(false);
	once.update(0.25f);
	check(once.getCurrentFrameIndex() == 1 && once.isFinished(),
		"non-looping animation stops on its last frame");
	check(once.getCurrentRect() == sf::IntRect({10, 0}, {10, 10}),
		"finished animation exposes its last frame rectangle");

	once.reset();
	check(once.getCurrentFrameIndex() == 0 && !once.isFinished(),
		"resetting an animation restores its initial state");

	Animation empty;
	bool threw = false;
	try
	{
		empty.getCurrentRect();
	}
	catch (const std::runtime_error&)
	{
		threw = true;
	}
	check(threw, "requesting a rectangle from an empty animation throws");
}

void testPlayerEightDirectionRunFacing()
{
	const World world;
	Player player({1500.f, 1500.f});
	const std::array<std::pair<sf::Vector2f, FacingDirection>, 8> directions{{
		{{0.f, -1.f}, FacingDirection::Up},
		{{1.f, -1.f}, FacingDirection::UpRight},
		{{1.f, 0.f}, FacingDirection::Right},
		{{1.f, 1.f}, FacingDirection::DownRight},
		{{0.f, 1.f}, FacingDirection::Down},
		{{-1.f, 1.f}, FacingDirection::DownLeft},
		{{-1.f, 0.f}, FacingDirection::Left},
		{{-1.f, -1.f}, FacingDirection::UpLeft}
	}};

	for (const auto& [direction, facing] : directions)
	{
		player.update(direction, 0.01f, world);
		check(player.getFacing() == facing && player.getAnimationState() == PlayerAnimationState::Run,
			"running animation selects the matching one of eight movement directions");
	}

	player.setAimDirection({1.f, -1.f});
	player.update({0.f, 0.f}, 0.01f, world);
	check(player.getFacing() == FacingDirection::UpRight && player.getAnimationState() == PlayerAnimationState::Idle,
		"idle facing returns to the eight-direction mouse aim");

	player.update({1.f, 0.f}, 0.01f, world);
	player.setAimDirection({-1.f, -1.f});
	player.triggerRangedAttack();
	check(player.getFacing() == FacingDirection::UpLeft,
		"attack animation faces the aim direction after running");
	player.update({1.f, 0.f}, 0.01f, world);
	check(player.getFacing() == FacingDirection::UpLeft,
		"moving during an attack does not override its aim-facing direction");
}

void testPlayerAnimationStates()
{
	const World world;
	Player player({300.f, 300.f});
	check(player.getAnimationState() == PlayerAnimationState::Idle, "player starts idle");
	check(!player.hasSprites(), "player falls back to geometric drawing when sprites are not loaded");

	player.setAimDirection({0.f, -1.f});
	check(player.getFacing() == FacingDirection::Up, "aiming upwards faces the player up");
	player.setAimDirection({-1.f, 0.f});
	check(player.getFacing() == FacingDirection::Left, "aiming left faces the player left");

	player.update({1.f, 0.f}, 0.01f, world);
	check(player.getAnimationState() == PlayerAnimationState::Run, "moving switches the player to the run animation");
	player.update({0.f, 0.f}, 0.01f, world);
	check(player.getAnimationState() == PlayerAnimationState::Idle, "stopping returns the player to idle");

	player.triggerMeleeAttack();
	check(player.getAnimationState() == PlayerAnimationState::AttackMelee, "melee trigger starts the melee animation");
	player.update({0.f, 0.f}, 0.5f, world);
	check(player.getAnimationState() == PlayerAnimationState::Idle, "melee animation returns to idle when it finishes");

	player.triggerRangedAttack();
	check(player.getAnimationState() == PlayerAnimationState::AttackRanged, "ranged trigger starts the ranged animation");
	player.update({0.f, 0.f}, 0.5f, world);
	check(player.getAnimationState() == PlayerAnimationState::Idle, "ranged animation returns to idle when it finishes");

	player.applyDamage(10.f);
	check(player.getAnimationState() == PlayerAnimationState::Hurt, "taking damage starts the hurt animation");
	player.update({0.f, 0.f}, 0.5f, world);
	check(player.getAnimationState() == PlayerAnimationState::Idle, "hurt animation returns to idle when it finishes");

	player.applyDamage(1000.f);
	check(!player.isAlive() && player.getAnimationState() == PlayerAnimationState::Death,
		"losing all health starts the death animation");
	player.update({0.f, 0.f}, 0.5f, world);
	check(player.getAnimationState() == PlayerAnimationState::Death, "death animation remains on its final state");
}

void testEnemyContactDamageCooldown()
{
	const World world;
	Player player({1000.f, 1000.f});
	CombatSystem combat({1050.f, 1000.f});
	combat.update(0.1f, player, world);
	check(near(player.getCurrentHealth(), Player::MaximumHealth), "enemy does not damage the player before contact");
	combat.update(0.1f, player, world);
	check(near(player.getCurrentHealth(), Player::MaximumHealth - Enemy::ContactDamage), "enemy damages the player on contact");
	combat.update(0.1f, player, world);
	check(near(player.getCurrentHealth(), Player::MaximumHealth - Enemy::ContactDamage),
		"enemy contact damage respects its cooldown");
}
}

int main()
{
	testMovementAndWorldBounds();
	testWallCollision();
	testCameraSmoothingAndBounds();
	testMouseCombatInput();
	testHealthAndAim();
	testMeleeRangeArcAndCooldown();
	testProjectileTrajectoryRangeAndCollision();
	testProjectileStopsAtWall();
	testProjectileHitsEnemyBeforeWall();
	testAnimationAdvanceLoopAndFinish();
	testPlayerEightDirectionRunFacing();
	testPlayerAnimationStates();
	testEnemyContactDamageCooldown();

	if (failures != 0)
	{
		std::cerr << failures << " test(s) failed.\n";
		return 1;
	}

	std::cout << "All gameplay tests passed.\n";
	return 0;
}
