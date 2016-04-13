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
	static const int BUFFER_SIZE = 10;
	static const int MAX_AXIS_VALUE = 32767; // -2^15
	static const int MIN_AXIS_VALUE = -32768; // 2^15 - 1
	static const int KONAMI[BUFFER_SIZE];

	InputMode mode;

	void setInputStruct(VehicleInput* input, int controllerNum);
	void setCamera(Camera* camera, int controllerNum);

	void controllerAxisMotion(SDL_Event e, GameState state);
	void controllerButtonDown(SDL_Event e, GameState state);
	void controllerButtonUp(SDL_Event e, GameState state);
	void openControllers();
	int numControllers();

	sigslot::signal0<> unFucker;
	sigslot::signal1<InputType> menuInput;
	sigslot::signal1<InputType> pauseInput;
	sigslot::signal1<GameState> setGameState;

private:
	SDL_GameController* controllers[MAX_NUM_CONTROLLERS];
	VehicleInput* inputs[MAX_NUM_CONTROLLERS];
	Camera* cameras[MAX_NUM_CONTROLLERS];
	int pastInputs[MAX_NUM_CONTROLLERS][BUFFER_SIZE];
	int pastInputsIndex[MAX_NUM_CONTROLLERS];

	float deadzonePercent;
	int deadzoneSize;

	bool isCheatCode(int index);
};

