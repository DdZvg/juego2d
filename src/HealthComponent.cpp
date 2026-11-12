#include "HealthComponent.h"

#include <algorithm>

HealthComponent::HealthComponent(float maximumHealth)
	: m_maximumHealth(std::max(0.f, maximumHealth)),
	  m_currentHealth(m_maximumHealth)
{
}

float HealthComponent::getCurrentHealth() const
{
	return m_currentHealth;
}

float HealthComponent::getMaximumHealth() const
{
	return m_maximumHealth;
}

bool HealthComponent::isAlive() const
{
	return m_currentHealth > 0.f;
}

void HealthComponent::applyDamage(float amount)
{
	if (amount > 0.f)
	{
		m_currentHealth = std::max(0.f, m_currentHealth - amount);
	}
}
