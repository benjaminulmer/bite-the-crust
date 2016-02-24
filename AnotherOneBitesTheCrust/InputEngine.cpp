#include "InputEngine.h"
#include <iostream>
#include <sigslot.h>

InputEngine::InputEngine(void) {
	openControllers();
	for (unsigned int i = 0; i < MAX_NUM_CONTROLLERS; i++)
	{
		inputs[i] = & dummyInput;
	}
	deadzonePercent = 0.25f;
	deadzoneSize = (int) (MAX_AXIS_VALUE * deadzonePercent);
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

void InputEngine::controllerAxisMotion(SDL_Event e)
{
	if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
	{
		if (e.caxis.value < -deadzoneSize)
		{
			inputs[e.cdevice.which]->rightSteer = (float)(e.caxis.value + deadzoneSize)/(MIN_AXIS_VALUE + deadzoneSize);
			inputs[e.cdevice.which]->leftSteer = 0;
		} 
		else if (e.caxis.value > deadzoneSize)
		{
			inputs[e.cdevice.which]->leftSteer = (float)(e.caxis.value - deadzoneSize)/(MAX_AXIS_VALUE - deadzoneSize);
			inputs[e.cdevice.which]->rightSteer = 0;
		}
		else
		{
			inputs[e.cdevice.which]->rightSteer = 0;
			inputs[e.cdevice.which]->leftSteer = 0;
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

void InputEngine::controllerButtonDown(SDL_Event e)
{
	if (e.cbutton.button == SDL_CONTROLLER_BUTTON_X)
	{
		inputs[e.cdevice.which]->shootPizza = true;
	}
	else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
	{
		inputs[e.cdevice.which]->handBrake = true;
	}
}

void InputEngine::controllerButtonUp(SDL_Event e)
{
	if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
	{
		inputs[e.cdevice.which]->handBrake = false;
	}
	/*else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_X)
	{
		inputs[e.cdevice.which]->shootPizza = false;
	}*/
}

void InputEngine::setInputStruct(VehicleInput* input, int controllerNum) {
	if (controllerNum > MAX_NUM_CONTROLLERS)
	{
		return;
	}
	inputs[controllerNum] = input;
}

InputEngine::~InputEngine(void)
{
}
