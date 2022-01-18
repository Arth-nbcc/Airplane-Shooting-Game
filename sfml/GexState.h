#pragma once
#include "State.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class GexState : public State
{
public:
	GexState(StateStack& stack, Context context);

	void draw() override;
	bool update(sf::Time dt) override;
	bool handleEvent(const sf::Event& event) override;

private:
	sf::Sprite	backgroundSprite;
	sf::Text	pausedText;
	sf::Text	instructionText;
	sf::Text	GexText;
	sf::Text	MenuText;
	sf::Sprite  FaceSprite;
};

