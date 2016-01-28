#pragma once
#include <SDL.h>

class InputEngine
{
public:
	static const int MAX_NUM_CONTROLLERS = 4;

	InputEngine(void);
	~InputEngine(void);

	void processControllerEvent(SDL_Event event);
	void getInput();

private:
	SDL_GameController* controllers[MAX_NUM_CONTROLLERS];

	void openControllers();
};

