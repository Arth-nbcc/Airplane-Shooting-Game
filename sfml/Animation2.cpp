#include "Animation2.h"

#include <cassert>



Animation2::Animation2(bool repeat) 
	: currentFrame(0)
	, elapsedTime(sf::Time::Zero)
	, duration(sf::Time::Zero)
	, repeat(repeat)
{}


Animation2::~Animation2()
{}

void Animation2::addFrame(const Frame& frame)
{
	frames.push_back(frame);
}

void Animation2::setDurationAsSeconds(float d)
{
	duration = sf::seconds(d);
}

void Animation2::start()
{
	assert(frames.size() > 0);
	currentFrame = 0;
	elapsedTime = sf::Time::Zero;
}

int Animation2::getNumFrames() const
{
	return frames.size();
}

int Animation2::getNumberOfFrames() const
{
	return frames.size();
}

Frame Animation2::getFrameByNumber(int n) const
{
	return frames[n];
}

Frame Animation2::getCurrentFrame() const
{
	assert(currentFrame < frames.size());
	return frames[currentFrame];
}

Frame Animation2::update(sf::Time dt)
{
	if (getNumberOfFrames() == 1)			// optimizaion for single frame
		return frames[currentFrame];

	sf::Time  timePerFrame = duration / static_cast<float>(frames.size());
	elapsedTime += dt;

	while (elapsedTime >= timePerFrame && currentFrame < frames.size())
	{
		++currentFrame;
		if (repeat)
			currentFrame %= frames.size();

		elapsedTime -= timePerFrame;
	}

	if (currentFrame >= frames.size())
		currentFrame = frames.size() - 1;		// be safe

	return frames[currentFrame];
}

