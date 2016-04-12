#pragma once
enum class GameState
{
	STARTING_GAME,
	BACK_TO_MENU,
	RESET,

	INTRO,
	MENU,
	PLAY,
	PAUSE,
	END,
	EXIT
};

enum class InputType
{
	UP,
	DOWN,
	LEFT,
	RIGHT,
	ENTER,
	BACK
};