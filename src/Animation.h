#pragma once

#include <SFML/Graphics/Rect.hpp>

#include <cstddef>
#include <vector>

struct AnimationFrame
{
	sf::IntRect rect;
	float duration{0.1f};
};

class Animation
{
public:
	Animation() = default;

	void addFrame(const sf::IntRect& rect, float duration);
	void setLoop(bool loop);
	bool isLoop() const;

	void reset();
	void update(float deltaSeconds);

	const sf::IntRect& getCurrentRect() const;
	std::size_t getCurrentFrameIndex() const;
	std::size_t getFrameCount() const;
	bool isFinished() const;
	bool isEmpty() const;

private:
	std::vector<AnimationFrame> m_frames;
	std::size_t m_currentFrame = 0;
	float m_elapsed = 0.f;
	bool m_loop = true;
	bool m_finished = false;
};
