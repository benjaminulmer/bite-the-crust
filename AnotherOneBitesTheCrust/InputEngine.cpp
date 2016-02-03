#include "InputEngine.h"
#include <iostream>

InputEngine::InputEngine(void) {
	openControllers();
}

// Opens up to 4 currently connected controllers
void InputEngine::openControllers() {
	for(int i = 0; i < SDL_NumJoysticks() && i < MAX_NUM_CONTROLLERS; i++) {
		controllers[i] = SDL_GameControllerOpen(i);
	}
	std::cout << "NUM CONTROLLERS: " << SDL_NumJoysticks() << std::endl;
}

void InputEngine::processControllerEvent(SDL_Event event) {
	// Controller button events
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
			std::cout << "a" << std::endl;
		}
	}
	// Controller axis events
	else if (event.type == SDL_CONTROLLERAXISMOTION) {
		if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
			std::cout << "left axis: " << event.caxis.value << std::endl;
		}
		else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
			std::cout << "left trig: " << event.caxis.value << std::endl;
		}
		else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
			std::cout << "righ trig: " << event.caxis.value << std::endl;
		}
	}
	// Controller added or removed
	else if (event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEADDED) {
		openControllers();
	}

	//std::cout << "event received" << std::endl;
}

void InputEngine::getInput() {
}

InputEngine::~InputEngine(void)
{
}
