#include "Camera.h"
#include "CombatSystem.h"
#include "HealthComponent.h"
#include "InputManager.h"
#include "Player.h"
#include "Projectile.h"
#include "World.h"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include <cmath>
#include <iostream>
#include <string>

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
	Player horizontal({1500.f, 1500.f});
	horizontal.update({1.f, 0.f}, 1.f, world);
	check(near(horizontal.getPosition().x, 1500.f + Player::MoveSpeed), "horizontal movement uses speed times delta time");

	Player thirtyFps({1500.f, 1500.f});
	Player sixtyFps({1500.f, 1500.f});
	for (int frame = 0; frame < 30; ++frame)
	{
		thirtyFps.update({1.f, 0.f}, 1.f / 30.f, world);
	}
	for (int frame = 0; frame < 60; ++frame)
	{
		sixtyFps.update({1.f, 0.f}, 1.f / 60.f, world);
	}
	check(near(thirtyFps.getPosition().x, sixtyFps.getPosition().x), "movement distance is independent of 30 versus 60 FPS");

	Player diagonal({1500.f, 1500.f});
	diagonal.update({1.f, -1.f}, 1.f, world);
	const sf::Vector2f diagonalOffset = diagonal.getPosition() - sf::Vector2f(1500.f, 1500.f);
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

void testMeleeRangeArcAndCooldown()
{
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

	combat.update(CombatSystem::MeleeCooldown + 0.01f, player);
	check(combat.tryMelee(player, {1300.f, 1000.f}), "melee attack becomes available after its cooldown");
	combat.update(CombatSystem::MeleeCooldown + 0.01f, player);
	check(combat.tryMelee(player, {1300.f, 1000.f}), "melee attack can be repeated after each cooldown");
	combat.update(0.f, player);
	check(combat.getEnemyCount() == 0, "enemy is removed after its health reaches zero");
}

void testProjectileTrajectoryRangeAndCollision()
{
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
	combat.update(0.1f, player);
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth), "projectile deals no damage before reaching the enemy");
	combat.update(0.1f, player);
	check(near(combat.getEnemy(0).getCurrentHealth(), Enemy::MaximumHealth - Projectile::Damage),
		"projectile collision applies damage while crossing an enemy");
	check(combat.getProjectileCount() == 0, "projectile is removed after impact");

	for (int shot = 0; shot < 3; ++shot)
	{
		check(combat.tryShoot(player, {1400.f, 1000.f}), "projectile can be fired after its cooldown");
		combat.update(0.2f, player);
	}
	check(combat.getEnemyCount() == 0, "projectile damage can kill and remove an enemy");
}

void testEnemyContactDamageCooldown()
{
	Player player({1000.f, 1000.f});
	CombatSystem combat({1050.f, 1000.f});
	combat.update(0.1f, player);
	check(near(player.getCurrentHealth(), Player::MaximumHealth), "enemy does not damage the player before contact");
	combat.update(0.1f, player);
	check(near(player.getCurrentHealth(), Player::MaximumHealth - Enemy::ContactDamage), "enemy damages the player on contact");
	combat.update(0.1f, player);
	check(near(player.getCurrentHealth(), Player::MaximumHealth - Enemy::ContactDamage),
		"enemy contact damage respects its cooldown");
}
}

int main()
{
	testMovementAndWorldBounds();
	testCameraSmoothingAndBounds();
	testMouseCombatInput();
	testHealthAndAim();
	testMeleeRangeArcAndCooldown();
	testProjectileTrajectoryRangeAndCollision();
	testEnemyContactDamageCooldown();

	if (failures != 0)
	{
		std::cerr << failures << " test(s) failed.\n";
		return 1;
	}

	std::cout << "All gameplay tests passed.\n";
	return 0;
}
