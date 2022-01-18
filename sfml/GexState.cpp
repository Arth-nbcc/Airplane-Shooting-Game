#include "GexState.h"
#include "Utility.h"
#include "CommandQueue.h"
#include"ResourceHolder.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

GexState::GexState(StateStack& stack, Context context)
	:State(stack, context)
	, FaceSprite()
	, pausedText()
	, instructionText()
	, GexText()
	, MenuText()
{
	sf::Vector2f viewSize = context.window->getView().getSize();

	pausedText.setFont(context.fonts->get(FontID::Main));
	pausedText.setString("Game Paused");
	pausedText.setCharacterSize(50);
	centerOrigin(pausedText);

	instructionText.setFont(context.fonts->get(FontID::Main));
	instructionText.setString("(Press Backspace to return to the main menu)");
	centerOrigin(instructionText);

	GexText.setFont(context.fonts->get(FontID::Main));
	GexText.setString("GEX State");
	centerOrigin(GexText);

	MenuText.setFont(context.fonts->get(FontID::Main));
	MenuText.setString("Press G to return to game");
	centerOrigin(MenuText);

	pausedText.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);
	instructionText.setPosition(0.5f * viewSize.x, 0.6f * viewSize.y);
	GexText.setPosition(0.5f * viewSize.x, 0.8f * viewSize.y);
	MenuText.setPosition(0.5f * viewSize.x, 0.10f * viewSize.y);

	//load face image	
	FaceSprite.setTexture(context.textures->get(TextureID::Face));

	//set transparency (alpha)
	FaceSprite.setColor(sf::Color(255, 255, 255, 100));

	//get size of sprite
	sf::FloatRect faceSize = FaceSprite.getLocalBounds();

	//position sprite
	FaceSprite.setPosition(0.5f * viewSize.x - faceSize.width / 2,
		0.5f * viewSize.y - faceSize.height / 2);
}

void GexState::draw()
{
	sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(200, 0, 0, 100));
	backgroundShape.setSize(window.getView().getSize());
	window.draw(backgroundShape);

	//alpha set in constructor
	window.draw(FaceSprite);

	window.draw(pausedText);
	window.draw(instructionText);
	window.draw(GexText);
	window.draw(MenuText);
}

bool GexState::update(sf::Time dt)
{
	return false;
}

bool GexState::handleEvent(const sf::Event& event)
{
	if (event.type != sf::Event::KeyPressed)
		return false;

	if (event.key.code == sf::Keyboard::Escape)
		requestStackPop();

	if (event.key.code == sf::Keyboard::G)
		requestStackPop();

	if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::BackSpace)
	{
		requestStateClear();
		requestStackPush(StateID::Menu);
	}
	return false;
}
