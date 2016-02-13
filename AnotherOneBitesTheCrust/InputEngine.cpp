#include "InputEngine.h"
#include <iostream>
#include <sigslot.h>

InputEngine::InputEngine(void) {
	openControllers();
}

// Opens up to 4 currently connected controllers
void InputEngine::openControllers() {
	for(int i = 0; i < SDL_NumJoysticks() && i < MAX_NUM_CONTROLLERS; i++) {
		controllers[i] = SDL_GameControllerOpen(i);
		inputs[i].accel = 0;
		inputs[i].brake = 0;
		inputs[i].leftSteer = 0;
		inputs[i].rightSteer = 0;
	}
	std::cout << "NUM CONTROLLERS: " << SDL_NumJoysticks() << std::endl;
	deadzoneSize = 6553;
}

void InputEngine::processControllerEvent(SDL_Event event) {
	// Controller button events
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
			FireSignal();
		}
	}
	// Controller axis events
	else if (event.type == SDL_CONTROLLERAXISMOTION) {
		if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
			if (event.caxis.value < -deadzoneSize) {
				inputs[0].rightSteer = (float)(event.caxis.value + deadzoneSize)/(MIN_AXIS_VALUE + deadzoneSize);
				inputs[0].leftSteer = 0;
			} 
			else if (event.caxis.value > deadzoneSize) {
				inputs[0].leftSteer = (float)(event.caxis.value - deadzoneSize)/(MAX_AXIS_VALUE - deadzoneSize);
				inputs[0].rightSteer = 0;
			}
			else {
				inputs[0].rightSteer = 0;
				inputs[0].leftSteer = 0;
			}
		}
		else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
			inputs[0].brake = (float)event.caxis.value/MAX_AXIS_VALUE;
		}
		else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
			inputs[0].accel = (float)event.caxis.value/MAX_AXIS_VALUE;
		}
		DrivingSignal(&inputs[0]);
	}
	// Controller added or removed
	else if (event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEADDED) {
		openControllers();
	}

	//std::cout << "event received" << std::endl;
}

DrivingInput* InputEngine::getInput() {
	return &inputs[0];
}

InputEngine::~InputEngine(void)
{
}
