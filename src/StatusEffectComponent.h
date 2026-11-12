#pragma once

class StatusEffectComponent
{
public:
	void update(float deltaSeconds);
	void applyAbyssalMark(float durationSeconds);
	bool isMarked() const;
	float getMarkTimeRemaining() const;
	float calculateDamage(float baseDamage, float markedDamageMultiplier) const;

private:
	float m_markTimeRemaining = 0.f;
};
