#pragma once
#include <SDL.h>
#include "DrivingInput.h"
#include <sigslot.h>

class InputEngine
{
public:
	InputEngine(void);
	~InputEngine(void);

	static const int MAX_NUM_CONTROLLERS = 4;
	static const int MAX_AXIS_VALUE = 32767; // -2^15
	static const int MIN_AXIS_VALUE = -32768; // 2^15 - 1

	void processControllerEvent(SDL_Event event);
	DrivingInput* getInput();

	sigslot::signal1<DrivingInput*> DrivingSignal;
	sigslot::signal0<> FireSignal;
private:
	SDL_GameController* controllers[MAX_NUM_CONTROLLERS];
	DrivingInput inputs[MAX_NUM_CONTROLLERS];
	float deadzonePercent;
	int deadzoneSize;

	void openControllers();
};

