#include "AIEngine.h"
#include <iostream>
#include <stdlib.h>

const int MIN_DIST = 6;

AIEngine::AIEngine(void)
{
	srand((int)time(0));
}

std::vector<glm::vec3> AIEngine::aStar(graphNode * start, graphNode * destination, vector<graphNode *> allNodes)
{
	std::map<graphNode *, double> distances;
	std::map<graphNode *, double> heuristic;
	std::map<graphNode *, graphNode *> previous;
	std::set<graphNode *> toVisit;

	toVisit.insert(start);
	for(graphNode * n : allNodes)
	{
		distances[n] = DBL_MAX;
		heuristic[n] = DBL_MAX;
	}

	
	distances[start] = 0;
	heuristic[start] = glm::length(start->getPosition() - destination->getPosition());

	previous[start] = nullptr;

	while(!toVisit.empty())
	{

		std::set<graphNode*>::iterator position = toVisit.begin();
		double minDist = DBL_MAX;
		for(std::set<graphNode*>::iterator i = toVisit.begin(); i != toVisit.end(); i++)
		{
			if(heuristic[*i] < minDist)
			{
				position = i;
				minDist = distances[*i];
			}
		}
		
		graphNode * current = *position;
		toVisit.erase(position);
		if(current == destination)
			break;

		for(graphNode * neighbour : current->getNeighbours())
		{
			// If the node has been visited, don't check
			if(distances[neighbour] != DBL_MAX)
				continue;

			double distance = 0;

			distance += distances[current] + glm::length(neighbour->getPosition() - current->getPosition());

			if(toVisit.find(neighbour) == toVisit.end())
				toVisit.insert(neighbour);
			else if(distance >= distances[neighbour])
				continue;

			previous[neighbour] = current;
			distances[neighbour] = distance;
			heuristic[neighbour] = distances[neighbour] + glm::length(neighbour->getPosition() - destination->getPosition());
			
		}
	}

	std::vector<glm::vec3> path;
	graphNode * current = destination;
	while(current != nullptr)
	{	
		path.push_back(current->getPosition());
		current = previous[current];
	}
	std::reverse(path.begin(), path.end());

	return path; 
}

// TODO: add logic for firing pizzas
void AIEngine::goToPoint(Vehicle* driver, const glm::vec3 & desiredPos, const float & distanceToGoal)
{
	VehicleInput* input = &driver->input;
	
	glm::vec3 desiredDirection = desiredPos - driver->getPosition();
	float distance = glm::length(desiredDirection);
	desiredDirection = glm::normalize(desiredDirection);
	glm::vec3 forward(glm::normalize(driver->getModelMatrix() * glm::vec4(0,0,1,0)));
	glm::vec3 left(glm::normalize(driver->getModelMatrix() * glm::vec4(1,0,0,0)));
	float cosAngle = glm::dot(desiredDirection, forward);
	float leftCosAngle = glm::dot(desiredDirection, left);

	float speed = driver->getPhysicsVehicle()->computeForwardSpeed();
	float ratio = glm::acos(cosAngle) / glm::pi<float>();

	float gas = glm::clamp(distanceToGoal / 100, (float)0, (float)1);
	printf("%f\n", gas);

	if (gas > 0.3)
	{
		input->forward = gas;
		input->backward = 0;
	}
	else
	{
		input->backward = gas;
		input->forward = 0;
	}
	
	input->handBrake = false;

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

inline bool sphereIntersect(const glm::vec3& raydir, const glm::vec3& rayorig, const glm::vec3& pos, const float& radius)
{
	glm::vec3 posToRay = rayorig - pos;

	float radius2 = radius * radius;
	float a = glm::dot(raydir,raydir);
	float b = glm::dot(raydir,  (2.0f * posToRay));
	float c = glm::dot(posToRay, posToRay) - radius2;
	float D = b * b - (4.0 * a * c);

	// If ray can not intersect then stop
	if (D < 0)
			return false;

	return true;
}

bool AIEngine::tooClose(Vehicle * avoider, PhysicsEntity * avoiding)
{
	// Should be able to get this from the vehicle / house somehow
	float carLength = 2.5; 
	float objectWidth = 4.5;

	glm::vec3 forward(glm::normalize(avoider->getModelMatrix() * glm::vec4(0,0,1,0)));

	if(sphereIntersect(forward, avoider->getPosition(), avoiding->getPosition(), objectWidth))
	{
		float distanceToCenter = glm::length(avoider->getPosition() - avoiding->getPosition()) - carLength;
		float objectCarRadius = (carLength + objectWidth);

		if(distanceToCenter < objectCarRadius)
			return true;

	}

	return false;
}

void AIEngine::avoid(Vehicle * driver, PhysicsEntity * obstacle, graphNode * destinationNode)
{
	glm::vec3 desiredDirection = glm::normalize(destinationNode->getPosition() - driver->getPosition());
	glm::vec3 left(glm::normalize(driver->getModelMatrix() * glm::vec4(1,0,0,0)));
	float leftCosAngle = glm::dot(desiredDirection, left);

	driver->input.backward = 1.0;
	driver->input.forward = 0.0;

	if(leftCosAngle > 0)
	{
		driver->input.steer = -1.0;
	}
	else
	{
		
		driver->input.steer = 1.0;
	}
}

void AIEngine::updatePath(Vehicle* toUpdate, Delivery destination, Map & map)
{

	Tile * currentTile = map.getTile(toUpdate->getPosition());
	// Should be 'goal node' of this tile
	graphNode * destinationNode = destination.location->nodes.at(0);

	
	if(glm::length(destinationNode->getPosition() - toUpdate->getPosition()) <= MIN_DIST)
		toUpdate->currentPath.push_back(destinationNode->getPosition());
	else
	{

		
		// Find closest node
		graphNode * closest = currentTile->nodes.at(0);
		if(!closest)
			return;
		double minDist = DBL_MAX;
		for(graphNode * n : currentTile->nodes)
		{
			double currentDist = glm::length(toUpdate->getPosition() - closest->getPosition());
			if(currentDist < minDist)
			{
				closest = n;
				minDist = currentDist;
			}
		}
		toUpdate->currentPath = aStar(closest, destinationNode, map.allNodes);
		//toUpdate->currentPath.push_back(destinationNode->getPosition());
	}
}

inline bool equals(glm::vec3 x, glm::vec3 y)
{
	return x.x == y.x && x.y == y.y && x.z == y.z;
}

void AIEngine::updateAI(Vehicle* toUpdate, Delivery destination, Map & map) 
{ 
	
	if(toUpdate->currentPath.empty())
	{
		updatePath(toUpdate, destination, map);
		if(toUpdate->currentPath.empty())
			return;
	}

	// Should be goal node
	if(!equals(toUpdate->getDestination(), destination.location->nodes.at(0)->getPosition()))
	{
		toUpdate->currentPath.clear();
		return;
	}

	// May want to consider something more efficient, uses square root in here
	float distanceToNext = glm::length(toUpdate->currentPath.at(0) - toUpdate->getPosition());

	if(distanceToNext < MIN_DIST)
	{
		//std::cout << "Waypoint get! Position: "<< toUpdate->currentPath.at(0).x << "," << toUpdate->currentPath.at(0).y << ", " << toUpdate->currentPath.at(0).z << std::endl;
		toUpdate->currentPath.erase(toUpdate->currentPath.begin());

		if(toUpdate->currentPath.empty())
		{
			toUpdate->input.forward = 0;
			toUpdate->input.backward = 1;
			toUpdate->input.handBrake = true;
			return;
		} 
	}

	Tile * currentTile = map.getTile(toUpdate->getPosition());
	// Should be 'goal node' of this tile
	graphNode * destinationNode = destination.location->nodes.at(0);
	for(PhysicsEntity * obstacle : currentTile->staticEntities)
	{
		if(tooClose(toUpdate, obstacle))
		{
			avoid(toUpdate, obstacle, destinationNode);
			return;
		}
	}
	goToPoint(toUpdate, toUpdate->currentPath.at(0), glm::length(destinationNode->getPosition() - toUpdate->getPosition()));
}

AIEngine::~AIEngine(void)
{
}
