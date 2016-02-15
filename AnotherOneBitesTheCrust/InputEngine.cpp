#include "InputEngine.h"
#include <iostream>
#include <sigslot.h>

InputEngine::InputEngine(void) {
	openControllers();
}

// Opens up to 4 currently connected controllers
void InputEngine::openControllers()
{
	for (int i = 0; i < SDL_NumJoysticks() && i < MAX_NUM_CONTROLLERS; i++)
	{
		controllers[i] = SDL_GameControllerOpen(i);
		inputs[i].forward = 0;
		inputs[i].backward = 0;
		inputs[i].leftSteer = 0;
		inputs[i].rightSteer = 0;
		inputs[i].handBrake = false;
		inputs[i].shootPizza = false;
	}
	std::cout << "NUM CONTROLLERS: " << SDL_NumJoysticks() << std::endl;
	deadzoneSize = 8192;
}

void InputEngine::processControllerEvent(SDL_Event e)
{
	// Controller button events
	if (e.type == SDL_CONTROLLERBUTTONDOWN)
	{
		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_X)
		{
			inputs[e.cdevice.which].shootPizza = true;
			FireSignal();
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
		{
			inputs[e.cdevice.which].handBrake = true;
		}
	}
	else if (e.type == SDL_CONTROLLERBUTTONUP) 
	{
		if (e.cbutton.button == SDL_CONTROLLER_BUTTON_X)
		{
			inputs[e.cdevice.which].shootPizza = false;
		}
		else if (e.cbutton.button == SDL_CONTROLLER_BUTTON_A)
		{
			inputs[e.cdevice.which].handBrake = false;
		}
	}
	// Controller axis events
	else if (e.type == SDL_CONTROLLERAXISMOTION)
	{
		if (e.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
		{
			if (e.caxis.value < -deadzoneSize)
			{
				inputs[e.cdevice.which].rightSteer = (float)(e.caxis.value + deadzoneSize)/(MIN_AXIS_VALUE + deadzoneSize);
				inputs[e.cdevice.which].leftSteer = 0;
			} 
			else if (e.caxis.value > deadzoneSize)
			{
				inputs[e.cdevice.which].leftSteer = (float)(e.caxis.value - deadzoneSize)/(MAX_AXIS_VALUE - deadzoneSize);
				inputs[e.cdevice.which].rightSteer = 0;
			}
			else
			{
				inputs[e.cdevice.which].rightSteer = 0;
				inputs[e.cdevice.which].leftSteer = 0;
			}
		}
		else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
		{
			inputs[e.cdevice.which].backward = (float)e.caxis.value/MAX_AXIS_VALUE;
		}
		else if (e.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
		{
			inputs[e.cdevice.which].forward = (float)e.caxis.value/MAX_AXIS_VALUE;
		}
	}
	// Controller added or removed
	else if (e.type == SDL_CONTROLLERDEVICEREMOVED || e.type == SDL_CONTROLLERDEVICEADDED)
	{
		openControllers();
	}
}

DrivingInput* InputEngine::getInput()
{
	return &inputs[0];
}

InputEngine::~InputEngine(void)
{
}
