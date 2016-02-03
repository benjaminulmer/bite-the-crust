#pragma once
#include <SDL.h>

class InputEngine
{
public:
	InputEngine(void);
	~InputEngine(void);

	static const int MAX_NUM_CONTROLLERS = 4;

	void processControllerEvent(SDL_Event event);
	void getInput();

private:
	SDL_GameController* controllers[MAX_NUM_CONTROLLERS];

	void openControllers();
};

