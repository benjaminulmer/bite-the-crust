#pragma once

#include <vector>
#include <ctime>
#include <map>
#include <limits>
#include <glm.hpp>
#include <gtc\constants.hpp>
#include "Vehicle.h"
#include "Map.h"
#include "DeliveryManager.h"

using std::vector;
using glm::vec3;

struct Player
{
	vec3 position;

	// TODO: Need a way to store current path + how long since last updated
};

struct TileAI
{
	static const int SIZE = 2;

	// Should these coordinates be relative to the tile or the map?
	// If it's relative to the tile, need a method to get player space -> tile space
	vec3 entrance, exit;
};

struct MapAI
{
	
	static const int MAP_SIZE = 70;
	static const int WIDTH = MAP_SIZE/TileAI::SIZE;
	static const int DEPTH = MAP_SIZE/TileAI::SIZE;

	TileAI tiles[WIDTH][DEPTH];

};

class AIEngine
{
public:
	AIEngine(void);
	~AIEngine(void);

	void updateAI(Vehicle*, Delivery, Map &);
	

private:
	void updatePath(Vehicle*, Delivery, Map &);
	void goToPoint(Vehicle*, const glm::vec3 &);
	void avoid(Vehicle *, PhysicsEntity*, graphNode*);
	bool tooClose(Vehicle*, PhysicsEntity*);

	std::vector<glm::vec3> aStar(graphNode *, graphNode *, std::vector<graphNode*>);
};

