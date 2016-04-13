#include "InputEngine.h"
#include <iostream>
#include <sigslot.h>

const int InputEngine::KONAMI[InputEngine::BUFFER_SIZE] = {SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_UP, SDL_CONTROLLER_BUTTON_DPAD_DOWN,
SDL_CONTROLLER_BUTTON_DPAD_DOWN, SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT, SDL_CONTROLLER_BUTTON_DPAD_LEFT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
SDL_CONTROLLER_BUTTON_B, SDL_CONTROLLER_BUTTON_A};

InputEngine::InputEngine(void) {
	openControllers();
	for (unsigned int i = 0; i < MAX_NUM_CONTROLLERS; i++)
	{
		inputs[i] = nullptr;
		cameras[i] = nullptr;
		pastInputsIndex[i] = 0;
	}
	deadzonePercent = 0.25f;
	deadzoneSize = (int) (MAX_AXIS_VALUE * deadzonePercent);
	mode = InputMode::NORMAL;
}

// Opens up to 4 currently connected controllers
void InputEngine::openControllers()
{
	for (int i = 0; i < SDL_NumJoysticks() && i < MAX_NUM_CONTROLLERS; i++)
	{
		controllers[i] = SDL_GameControllerOpen(i);
	}
	std::cout << "NUM CONTROLLERS: " << SDL_NumJoysticks() << std::endl;
}

int InputEngine::numControllers()
{
	return SDL_NumJoysticks();
}

void InputEngine::controllerAxisMotion(SDL_Event e, GameState state)
{
	if (state == GameState::PLAY || state == GameState::END)
	{
		if (inputs[e.cdevice.which] == nullptr || cameras[e.cdevice.which] == nullptr) return;

		if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
		{
			if (e.caxis.value < -deadzoneSize)
			{
				inputs[e.cdevice.which]->steer = (float)(e.caxis.value + deadzoneSize)/(MIN_AXIS_VALUE + deadzoneSize);
			} 
			else if (e.caxis.value > deadzoneSize)
			{
				inputs[e.cdevice.which]->steer = -(float)(e.caxis.value - deadzoneSize)/(MAX_AXIS_VALUE - deadzoneSize);
			}
			else
			{
				inputs[e.cdevice.which]->steer = 0;
			}
		}
		else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
		{
			inputs[e.cdevice.which]->backward = (float)e.caxis.value/MAX_AXIS_VALUE;
		}
		else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
		{
			inputs[e.cdevice.which]->forward = (float)e.caxis.value/MAX_AXIS_VALUE;
		}
	}
	else if (state == GameState::MENU)
	{
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
		{
			if (e.caxis.value >= MAX_AXIS_VALUE - 5)
			{
				menuInput(InputType::DOWN);
			}
			else if (e.caxis.value <= MIN_AXIS_VALUE + 5)
			{
				menuInput(InputType::UP);
			}
		}
		else if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
		{
			if (e.caxis.value >= MAX_AXIS_VALUE - 5)
			{
				menuInput(InputType::RIGHT);
			}
			else if (e.caxis.value <= MIN_AXIS_VALUE + 5)
			{
				menuInput(InputType::LEFT);
			}
		}
	}
	else if (state == GameState::PAUSE)
	{
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
		{
			if (e.caxis.value >= MAX_AXIS_VALUE - 5)
			{
				pauseInput(InputType::DOWN);
			}
			else if (e.caxis.value <= MIN_AXIS_VALUE + 5)
			{
				pauseInput(InputType::UP);
			}
		}
	}
}

void InputEngine::controllerButtonDown(SDL_Event e, GameState state)
{
	if (state == GameState::PLAY || state == GameState::END)
	{
		if (inputs[e.cdevice.which] == nullptr || cameras[e.cdevice.which] == nullptr) return;

		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_X)
		{
			inputs[e.cdevice.which]->shootPizza = true;
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
		{
			inputs[e.cdevice.which]->handBrake = true;
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_B)
		{
			cameras[e.cdevice.which]->setReverseCam(true);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_RIGHTSTICK)
		{
			//unFucker();
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_Y)
		{
			inputs[e.cdevice.which]->jump = true;
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_START)
		{
			if (isCheatCode(e.cdevice.which))
			{
				std::cout << "You're really good at pressing 'A'. Good job!" << std::endl;
			}
			(state == GameState::PLAY) ? setGameState(GameState::PAUSE) : setGameState(GameState::BACK_TO_MENU);
		}

		pastInputs[e.cdevice.which][pastInputsIndex[e.cdevice.which]] = e.cbutton.button;
		pastInputsIndex[e.cdevice.which] = (pastInputsIndex[e.cdevice.which] + 1) % BUFFER_SIZE;
	}
	else if (state == GameState::MENU)
	{
		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)
		{
			menuInput(InputType::UP);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
		{
			menuInput(InputType::DOWN);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT)
		{
			menuInput(InputType::LEFT);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
		{
			menuInput(InputType::RIGHT);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
		{
			menuInput(InputType::ENTER);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_B)
		{
			menuInput(InputType::BACK);
		}
	}
	else if (state == GameState::PAUSE)
	{
		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_UP)
		{
			pauseInput(InputType::UP);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_DOWN)
		{
			pauseInput(InputType::DOWN);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
		{
			pauseInput(InputType::ENTER);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_B)
		{
			pauseInput(InputType::BACK);
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_START)
		{
			pauseInput(InputType::BACK);
		}
	}
}

bool InputEngine::isCheatCode(int index)
{
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		int test = (pastInputsIndex[index]+i)%BUFFER_SIZE;
		int test2 = pastInputs[index][(pastInputsIndex[i]+i)%BUFFER_SIZE];
		if (pastInputs[index][(pastInputsIndex[index]+i)%BUFFER_SIZE] != KONAMI[i])
		{
			return false;
		}
	}
	return true;
}

void InputEngine::controllerButtonUp(SDL_Event e, GameState state)
{
	if (state == GameState::PLAY || state == GameState::END)
	{
		if (inputs[e.cdevice.which] == nullptr || cameras[e.cdevice.which] == nullptr) return;

		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
		{
			inputs[e.cdevice.which]->handBrake = false;
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_B)
		{
			cameras[e.cdevice.which]->setReverseCam(false);
		}
	}
}

void InputEngine::setInputStruct(VehicleInput* input, int controllerNum) {
	if (controllerNum > MAX_NUM_CONTROLLERS)
	{
		return;
	}
	inputs[controllerNum] = input;
}

void InputEngine::setCamera(Camera* camera, int controllerNum) {
	if (controllerNum > MAX_NUM_CONTROLLERS)
	{
		return;
	}
	cameras[controllerNum] = camera;
}

InputEngine::~InputEngine(void)
{
}
