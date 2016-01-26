#pragma once
#include <SDL.h>

class InputEngine
{
public:
	InputEngine(void);
	~InputEngine(void);

	void processControllerEvent(SDL_Event event);
	void getInput();

private:
	SDL_GameController* controllers[4];
};

