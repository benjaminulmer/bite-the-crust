#include "AIEngine.h"
#include <iostream>
#include <stdlib.h>

AIEngine::AIEngine(void)
{
	srand((int)time(0));
}

std::vector<glm::vec3> AIEngine::dijkstras(graphNode * start, graphNode * destination, vector<graphNode *> allNodes)
{
	std::map<graphNode *, double> distances;
	std::map<graphNode *, graphNode *> previous;
	std::list<graphNode *> toVisit;

	for(graphNode * n : allNodes)
		distances[n] = DBL_MAX;

	toVisit.push_front(start);
	distances[start] = 0;

	while(!toVisit.empty())
	{
		graphNode * current = toVisit.front();
		toVisit.pop_front();

		for(graphNode * neighbour : current->getNeighbours())
		{
			double distance = 0;

			distance += distances[neighbour] + glm::length(neighbour->getPosition() - current->getPosition());
			if(distance < distances[neighbour])
			{
				distances[neighbour] = distance;
				previous[neighbour] = current;
			}
		}
	}

	std::vector<glm::vec3> path;
	graphNode * current = destination;
	while(previous.find(current) != previous.end())
	{	
		path.push_back(current->getPosition());
		current = previous[current];
	}
	path.push_back(current->getPosition());
	std::reverse(path.begin(), path.end());

	return path; 
}

// TODO: add logic for firing pizzas
void AIEngine::goToPoint(Vehicle* driver, const glm::vec3 & desiredPos)
{
	VehicleInput* input = &driver->input;
	input->forward = 1.0;
	input->backward = 0.0;
	input->handBrake = false;
	
	glm::vec3 desiredDirection = glm::normalize(desiredPos - driver->getPosition());
	glm::vec3 forward(glm::normalize(driver->getModelMatrix() * glm::vec4(0,0,1,0)));
	glm::vec3 left(glm::normalize(driver->getModelMatrix() * glm::vec4(1,0,0,0)));
	float cosAngle = glm::dot(desiredDirection, forward);
	float leftCosAngle = glm::dot(desiredDirection, left);

	float ratio = glm::acos(cosAngle) / glm::pi<float>();

	// Stuff Ben added
	ratio *= 2;
	if (ratio > 1) ratio = 1;

	if(ratio > 0.1)
	{
		if(leftCosAngle > 0)
		{
			input->steer = ratio;
		}
		else
		{
			input->steer = -ratio;
		}
	}
	else
	{
		input->steer = 0;
		if (rand()%100 < 5)
		{
			input->shootPizza = true;
		}
	}
}

void AIEngine::updatePath(Vehicle* toUpdate, Delivery destination, Map & map)
{
	Tile * currentTile = map.getTile(toUpdate->getPosition());

	// Find closest node
	graphNode * closest = currentTile->nodes.at(0);
	if(!closest)
		return;
	double minDist = -1;
	for(graphNode * n : currentTile->nodes)
	{
		double currentDist = glm::length(toUpdate->getPosition() - closest->getPosition());
		if(currentDist < minDist)
		{
			closest = n;
			minDist = currentDist;
		}
	}

	toUpdate->currentPath = dijkstras(closest, destination.location->nodes.at(0), map.allNodes);
}

void AIEngine::updateAI(Vehicle* toUpdate, Delivery destination, Map & map) 
{ 
	if(toUpdate->currentPath.empty())
	{
		updatePath(toUpdate, destination, map);
		if(toUpdate->currentPath.empty())
			return;
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
