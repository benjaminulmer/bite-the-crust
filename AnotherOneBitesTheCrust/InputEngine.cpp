#include "InputEngine.h"
#include <iostream>

InputEngine::InputEngine(void) {
	for(int i = 0; i < SDL_NumJoysticks(); i++) {
		controllers[i] = SDL_GameControllerOpen(i);
	}
}

void InputEngine::processControllerEvent(SDL_Event event) {
	if (event.type == SDL_CONTROLLERBUTTONDOWN) {
		if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) {
			std::cout << "a" << std::endl;
		}
	}
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

	//std::cout << "event received" << std::endl;
}

void InputEngine::getInput() {
}

InputEngine::~InputEngine(void)
{
}
