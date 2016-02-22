#pragma once
#include <SDL.h>
#include <sigslot.h>
#include "Vehicle.h"

class InputEngine
{
public:
	InputEngine(void);
	~InputEngine(void);

	static const int MAX_NUM_CONTROLLERS = 4;
	static const int MAX_AXIS_VALUE = 32767; // -2^15
	static const int MIN_AXIS_VALUE = -32768; // 2^15 - 1

	void setInputStruct(VehicleInput* input, int controllerNum);
	void processControllerEvent(SDL_Event event);

private:
	SDL_GameController* controllers[MAX_NUM_CONTROLLERS];
	VehicleInput* inputs[MAX_NUM_CONTROLLERS];
	VehicleInput dummyInput; // default for each controller until an input has been set
	float deadzonePercent;
	int deadzoneSize;

	void openControllers();
};

