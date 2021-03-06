#include "GameState.h"
#include "PlayerControl.h"

#include <SFML/Graphics/RenderWindow.hpp>

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, world(*context.window, *context.fonts)
	, player(*context.player)
{
	context.music->play(MusicID::MissionTheme);
}


void GameState::draw()
{
	world.draw();
}

bool GameState::update(sf::Time dt)
{
	world.update(dt);

	if (!world.hasAlivePlayer())
	{
		player.setMissionStatus(PlayerControl::MissionStatus::MissionFailure);
		requestStackPush(StateID::GameOver);
	}
	else if (world.hasPlayerReachedEnd())
	{
		player.setMissionStatus(PlayerControl::MissionStatus::MissionSuccess);
		requestStackPush(StateID::GameOver);
	}

	CommandQueue& commands = world.getCommandQueue();
	player.handleRealTimeInput(commands);

	return true;
}

bool GameState::handleEvent(const sf::Event& event)
{
	//Game input handling
	CommandQueue& commands = world.getCommandQueue();
	player.handleEvent(event, commands);

	//escape pressed, trigger the pause screen
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		requestStackPush(StateID::Pause);
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G)
		requestStackPush(StateID::Gex);
	return true;
}
//