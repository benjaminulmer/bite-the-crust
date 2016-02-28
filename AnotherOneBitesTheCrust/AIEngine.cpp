#include "AIEngine.h"
#include <iostream>
#include <stdlib.h>

AIEngine::AIEngine(void)
{
	srand((int)time(0));
}

void AIEngine::goToPoint(Vehicle* driver, glm::vec3 desiredPos)
{
	VehicleInput* input = &driver->input;
	input->forward = 1.0;
	input->backward = 0.0;
	input->handBrake = false;

	// Pizza shooting proof of concept
	//int pizzaRand = rand() % 100;
	//if (pizzaRand == 0)
	//{
	//	input->shootPizza = true;
	//}
	
	glm::vec3 desiredDirection = glm::normalize(desiredPos - driver->getPosition());
	glm::vec3 forward(glm::normalize(driver->getModelMatrix() * glm::vec4(0,0,1,0)));
	glm::vec3 left(glm::normalize(driver->getModelMatrix() * glm::vec4(1,0,0,0)));
	float cosAngle = glm::dot(desiredDirection, forward);
	float leftCosAngle = glm::dot(desiredDirection, left);

	float ratio = glm::acos(cosAngle) / glm::pi<float>();

	if(ratio > 0.1)
	{
		if(leftCosAngle > 0)
		{
			input->rightSteer = ratio;
			input->leftSteer = 0;
		}
		else
		{
			input->leftSteer = ratio;
			input->rightSteer = 0;
		}
	}
	else
	{
		input->rightSteer = 0;
		input->leftSteer = 0;
		if (rand()%100 < 10)
		{
			input->shootPizza = true;
		}
	}
}

void AIEngine::updatePath(Vehicle* toUpdate)
{
	toUpdate->currentPath.push_back(glm::vec3(-MapAI::MAP_SIZE/2, 0, MapAI::MAP_SIZE/2));
	toUpdate->currentPath.push_back(glm::vec3(MapAI::MAP_SIZE/2, 0, MapAI::MAP_SIZE/2));
	toUpdate->currentPath.push_back(glm::vec3(-MapAI::MAP_SIZE/2, 0, -MapAI::MAP_SIZE/2));
	toUpdate->currentPath.push_back(glm::vec3(MapAI::MAP_SIZE/2, 0, -MapAI::MAP_SIZE/2));
}

void AIEngine::updateAI(Vehicle* toUpdate) 
{ 
	if(toUpdate->currentPath.empty())
	{
		updatePath(toUpdate);
	}

	// May want to consider something more efficient, uses square root in here
	float distanceToNext = glm::length(toUpdate->currentPath.at(0) - toUpdate->getPosition());

	if(distanceToNext < 10)
	{
		std::cout << "Waypoint get! Position: "<< toUpdate->currentPath.at(0).x << "," << toUpdate->currentPath.at(0).y << ", " << toUpdate->currentPath.at(0).z << std::endl;
		toUpdate->currentPath.erase(toUpdate->currentPath.begin());

		if(toUpdate->currentPath.empty())
		{
			// return DrivingInput();
			return;
		} 
	}
	goToPoint(toUpdate, toUpdate->currentPath.at(0));
}

AIEngine::~AIEngine(void)
{
}
