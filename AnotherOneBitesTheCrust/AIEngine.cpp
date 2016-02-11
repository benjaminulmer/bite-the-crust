#include "AIEngine.h"


AIEngine::AIEngine(void)
{
}

DrivingInput AIEngine::updateAI() 
{ 
	DrivingInput input;
	input.accel = 1.0;
	input.leftSteer = 1.0;
	input.rightSteer = 0.0;
	input.brake = 0.0;

	return input;
}

AIEngine::~AIEngine(void)
{
}
