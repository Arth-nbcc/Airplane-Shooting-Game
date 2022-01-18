#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Time.hpp>

#include <vector>

//
// An animaiton is a collection of frames. 
// 

using Frame = sf::IntRect;

class Animation2
{
public:
	Animation2(bool repeat = true);
	~Animation2();

	void						addFrame(const Frame& frame);
	void						setDurationAsSeconds(float d);
	void						start();

	int							getNumFrames() const; // depricated
	int							getNumberOfFrames() const;
	Frame						getFrameByNumber(int n) const;
	Frame						getCurrentFrame() const;
	Frame						update(sf::Time dt);


private:
	std::vector<Frame>			frames;
	std::size_t					currentFrame;
	sf::Time					elapsedTime;
	sf::Time					duration;
	//sf::Time					timePerFrame;
	bool						repeat;

};
