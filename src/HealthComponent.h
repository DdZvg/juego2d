#pragma once

class HealthComponent
{
public:
	explicit HealthComponent(float maximumHealth);

	float getCurrentHealth() const;
	float getMaximumHealth() const;
	bool isAlive() const;
	void applyDamage(float amount);

private:
	float m_maximumHealth;
	float m_currentHealth;
};
