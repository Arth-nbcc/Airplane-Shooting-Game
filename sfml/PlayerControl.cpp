#include "PlayerControl.h"
#include "Aircraft.h"
#include "Command.h"
#include "CommandQueue.h";

//#include <iostream>

PlayerControl::PlayerControl()
	:currentMissionStatus(MissionStatus::MissionRunning)
{
	initializeKeys();
	initializeActions();
}

void PlayerControl::initializeKeys()
{
	//set initial key bindings
	//arrow keys
	keyBindings[sf::Keyboard::Left] = Action::MoveLeft;
	keyBindings[sf::Keyboard::Right] = Action::MoveRight;
	keyBindings[sf::Keyboard::Up] = Action::MoveUp;
	keyBindings[sf::Keyboard::Down] = Action::MoveDown;
	//wasd
	keyBindings[sf::Keyboard::A] = Action::MoveLeft;
	keyBindings[sf::Keyboard::D] = Action::MoveRight;
	keyBindings[sf::Keyboard::W] = Action::MoveUp;
	keyBindings[sf::Keyboard::S] = Action::MoveDown;
	//rotate
	keyBindings[sf::Keyboard::R] = Action::RotRight;
	keyBindings[sf::Keyboard::L] = Action::RotLeft;

	//projectiles
	keyBindings[sf::Keyboard::Space] = Action::Fire;
	keyBindings[sf::Keyboard::M] = Action::LaunchMissile;
}

void PlayerControl::handleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		auto found = keyBindings.find(event.key.code);

		if (found != keyBindings.end() && !isRealTimeAction(found->second))
			commands.push(actionBindings[found->second]);

	}
}

void PlayerControl::handleRealTimeInput(CommandQueue& commands)
{
	for (auto [key, action] : keyBindings)
	{

		if (sf::Keyboard::isKeyPressed(key) && isRealTimeAction(action))
			commands.push(actionBindings[action]);
	}
}

void PlayerControl::assignKey(Action action, sf::Keyboard::Key key)
{
	//remove all keys that already map to action
	for (auto itr = keyBindings.begin(); itr != keyBindings.end();)
	{
		if (itr->second == action)
			keyBindings.erase(itr++);
		else
			++itr;
	}
	//insert new buinding
	keyBindings[key] = action;
}

sf::Keyboard::Key PlayerControl::getAssignedKey(Action action) const
{
	for (auto [key, boundAction] : keyBindings)
	{
		if (boundAction == action)
			return key;
	}
	return sf::Keyboard::Unknown;
}

void PlayerControl::setMissionStatus(MissionStatus status)
{
	currentMissionStatus = status;
}

PlayerControl::MissionStatus PlayerControl::getMissionStatus() const
{
	return currentMissionStatus;
}


void PlayerControl::initializeActions()
{
	const float speed = 200.f;

	actionBindings[Action::MoveLeft].action = derivedAction<Aircraft>([=](Aircraft& n, sf::Time dt) {n.accelerate(-speed, 0.f); });
	actionBindings[Action::MoveLeft].category = Category::PlayerAircraft;

	actionBindings[Action::MoveRight].action = derivedAction<Aircraft>([=](Aircraft& n, sf::Time dt) {n.accelerate(speed, 0.f); });
	actionBindings[Action::MoveRight].category = Category::PlayerAircraft;

	actionBindings[Action::MoveUp].action = derivedAction<Aircraft>([=](Aircraft& n, sf::Time dt) {n.accelerate(0.f, -speed); });
	actionBindings[Action::MoveUp].category = Category::PlayerAircraft;

	actionBindings[Action::MoveDown].action = derivedAction<Aircraft>([=](Aircraft& n, sf::Time dt) {n.accelerate(0.f, speed); });
	actionBindings[Action::MoveDown].category = Category::PlayerAircraft;

	actionBindings[Action::RotRight].action = derivedAction<Aircraft>([=](Aircraft& n, sf::Time dt) {n.angularAccelerate(10.f); });
	actionBindings[Action::RotRight].category = Category::EnemyAircraft;

	actionBindings[Action::RotLeft].action = derivedAction<Aircraft>([=](Aircraft& n, sf::Time dt) {n.angularAccelerate(-10.f); });
	actionBindings[Action::RotLeft].category = Category::EnemyAircraft;

	actionBindings[Action::Fire].action = derivedAction<Aircraft>([=](Aircraft& n, sf::Time dt) {n.fire(); });
	actionBindings[Action::Fire].category = Category::PlayerAircraft;

	actionBindings[Action::LaunchMissile].action = derivedAction<Aircraft>([=](Aircraft& n, sf::Time dt) {n.launchMissile(); });
	actionBindings[Action::LaunchMissile].category = Category::PlayerAircraft;
}

bool PlayerControl::isRealTimeAction(Action action)
{
	switch (action)
	{
	case Action::MoveDown:
	case Action::MoveUp:
	case Action::MoveLeft:
	case Action::MoveRight:
	case Action::RotRight:
	case Action::RotLeft:
	case Action::Fire:
		return true;
	default:
		return false;

	}
}
