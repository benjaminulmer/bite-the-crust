#include "AIEngine.h"


AIEngine::AIEngine(void)
{
	srand(time(0));
}

DrivingInput AIEngine::goToPoint(Vehicle* driver, glm::vec3 desiredPos)
{
	DrivingInput input;
	input.accel = 1.0;
	input.brake = 0.0;
	
	glm::vec3 desiredDirection = glm::normalize(desiredPos - driver->getPosition());
	glm::vec3 forward(glm::normalize(driver->getModelMatrix() * glm::vec4(0,0,1,0)));

	glm::acos(glm::dot(desiredDirection, desiredPos));


	return input;
}

void AIEngine::updatePath(Vehicle* toUpdate)
{

	for(int i = 0; i < 10; i++)
	{
		glm::vec3 randPoint((rand()%Map::MAP_SIZE) - Map::MAP_SIZE/2, 0, (rand()%Map::MAP_SIZE) - Map::MAP_SIZE/2);
		toUpdate->currentPath.push_back(randPoint);
	}

}

DrivingInput AIEngine::updateAI(Vehicle* toUpdate) 
{ 
	if(toUpdate->currentPath.empty())
	{
		updatePath(toUpdate);
	}

	// May want to consider something more efficient, uses square root in here
	float distanceToNext = glm::length(toUpdate->currentPath.at(0) - toUpdate->getPosition());

	if(distanceToNext < 5)
		toUpdate->currentPath.erase(toUpdate->currentPath.begin());


	return goToPoint(toUpdate->getPosition(), toUpdate->currentPath.at(0));
}

AIEngine::~AIEngine(void)
{
}
