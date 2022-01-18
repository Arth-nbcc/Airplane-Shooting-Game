#pragma once

#include "Command.h"
#include <SFML/Window/Event.hpp>
#include <map>
//#include "CommandQueue.h"

class CommandQueue;

class PlayerControl
{
public:

	enum class Action
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		Fire,
		LaunchMissile,

		RotLeft,
		RotRight,
		ActionCount
	};

	enum class MissionStatus
	{
		MissionRunning,
		MissionSuccess,
		MissionFailure
	};

public:
	//handle event and real time input
	PlayerControl();
	PlayerControl(PlayerControl&) = delete;

	void				handleEvent(const sf::Event& event, CommandQueue& commands);
	void				handleRealTimeInput(CommandQueue& commands);

	void				assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key	getAssignedKey(Action action) const;

	void				setMissionStatus(MissionStatus status);
	MissionStatus		getMissionStatus() const;

private:
	void				initializeKeys();
	void				initializeActions();
	static bool			isRealTimeAction(Action action);

private:
	std::map<sf::Keyboard::Key, Action>			keyBindings;
	std::map<Action, Command>					actionBindings;
	MissionStatus								currentMissionStatus;

};

