#include "StatusEffectComponent.h"

#include <algorithm>

void StatusEffectComponent::update(float deltaSeconds)
{
	if (deltaSeconds > 0.f)
	{
		m_markTimeRemaining = std::max(0.f, m_markTimeRemaining - deltaSeconds);
	}
}

void StatusEffectComponent::applyAbyssalMark(float durationSeconds)
{
	m_markTimeRemaining = std::max(m_markTimeRemaining, std::max(0.f, durationSeconds));
}

bool StatusEffectComponent::isMarked() const
{
	return m_markTimeRemaining > 0.f;
}

float StatusEffectComponent::getMarkTimeRemaining() const
{
	return m_markTimeRemaining;
}

float StatusEffectComponent::calculateDamage(float baseDamage, float markedDamageMultiplier) const
{
	const float multiplier = isMarked() ? std::max(1.f, markedDamageMultiplier) : 1.f;
	return baseDamage * multiplier;
}
