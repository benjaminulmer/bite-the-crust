#include "InputEngine.h"
#include <iostream>
#include <sigslot.h>

InputEngine::InputEngine(void) {
	openControllers();
	for (unsigned int i = 0; i < MAX_NUM_CONTROLLERS; i++)
	{
		inputs[i] = nullptr;
		cameras[i] = nullptr;
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

void InputEngine::controllerAxisMotion(SDL_Event e, GameState state)
{
	if (inputs[e.cdevice.which] == nullptr || cameras[e.cdevice.which] == nullptr) return;

	if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
	{
		if (mode == InputMode::NORMAL)
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

	}
	else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
	{
		inputs[e.cdevice.which]->backward = (float)e.caxis.value/MAX_AXIS_VALUE;
	}
	else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
	{
		inputs[e.cdevice.which]->forward = (float)e.caxis.value/MAX_AXIS_VALUE;
	}

	if (mode == InputMode::DEBUG) 
	{

	}
}

void InputEngine::controllerButtonDown(SDL_Event e, GameState state)
{

	if (state == GameState::PLAY)
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
			unFucker();
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_Y)
		{
			inputs[e.cdevice.which]->jump = true;
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_START)
		{
			pausePressed(GameState::PAUSE);
		}
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
	else if(state == GameState::END)
	if (e.cbutton.button == SDL_CONTROLLER_BUTTON_START)
	{
		endInput(InputType::BACK);
	}

	/*else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
	{
		mode = (mode == InputMode::NORMAL) ? InputMode::DEBUG : InputMode::NORMAL; 
	}*/
}

void InputEngine::controllerButtonUp(SDL_Event e, GameState state)
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
