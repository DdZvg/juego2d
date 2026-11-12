#include "Animation.h"

#include <stdexcept>

void Animation::addFrame(const sf::IntRect& rect, float duration)
{
	m_frames.push_back({rect, duration});
}

void Animation::setLoop(bool loop)
{
	m_loop = loop;
}

bool Animation::isLoop() const
{
	return m_loop;
}

void Animation::reset()
{
	m_currentFrame = 0;
	m_elapsed = 0.f;
	m_finished = false;
}

void Animation::update(float deltaSeconds)
{
	if (m_frames.empty() || m_finished)
	{
		return;
	}

	m_elapsed += deltaSeconds;
	while (!m_finished && m_elapsed >= m_frames[m_currentFrame].duration)
	{
		m_elapsed -= m_frames[m_currentFrame].duration;
		if (m_currentFrame + 1 < m_frames.size())
		{
			++m_currentFrame;
		}
		else if (m_loop)
		{
			m_currentFrame = 0;
		}
		else
		{
			m_finished = true;
		}
	}
}

const sf::IntRect& Animation::getCurrentRect() const
{
	if (m_frames.empty())
	{
		throw std::runtime_error("Animation has no frames configured.");
	}

	return m_frames[m_currentFrame].rect;
}

std::size_t Animation::getCurrentFrameIndex() const
{
	return m_currentFrame;
}

std::size_t Animation::getFrameCount() const
{
	return m_frames.size();
}

bool Animation::isFinished() const
{
	return m_finished;
}

bool Animation::isEmpty() const
{
	return m_frames.empty();
}
