#pragma once
#include <SDL.h>
#include <sigslot.h>
#include "Vehicle.h"
#include "Camera.h"
#include "GameState.h"

enum class InputMode
{
	NORMAL,
	DEBUG
};

class InputEngine
{
public:
	InputEngine(void);
	~InputEngine(void);

	static const int MAX_NUM_CONTROLLERS = 4;
	static const int MAX_AXIS_VALUE = 32767; // -2^15
	static const int MIN_AXIS_VALUE = -32768; // 2^15 - 1

	InputMode mode;

	void setInputStruct(VehicleInput* input, int controllerNum);
	void setCamera(Camera* camera, int controllerNum);

	void controllerAxisMotion(SDL_Event e, GameState state);
	void controllerButtonDown(SDL_Event e, GameState state);
	void controllerButtonUp(SDL_Event e, GameState state);
	void openControllers();

	sigslot::signal0<> unFucker;
	sigslot::signal0<> menuUp;
	sigslot::signal0<> menuDown;
	sigslot::signal0<> menuEnter;
	sigslot::signal0<> menuBack;

private:
	SDL_GameController* controllers[MAX_NUM_CONTROLLERS];
	VehicleInput* inputs[MAX_NUM_CONTROLLERS];
	Camera* cameras[MAX_NUM_CONTROLLERS];

	float deadzonePercent;
	int deadzoneSize;
};

