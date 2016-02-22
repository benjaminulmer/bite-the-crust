#pragma once

#include <vector>
#include <ctime>
#include <glm.hpp>
#include <gtc\constants.hpp>
#include "DrivingInput.h"
#include "Vehicle.h"

using std::vector;
using glm::vec3;

struct Player
{
	vec3 position;

	// TODO: Need a way to store current path + how long since last updated
};

struct TileAI
{
	static const int SIZE = 10;

	// Should these coordinates be relative to the tile or the map?
	// If it's relative to the tile, need a method to get player space -> tile space
	vec3 entrance, exit;
};

struct MapAI
{
	
	static const int MAP_SIZE = 70;
	static const int WIDTH = MAP_SIZE/TileAI::SIZE;
	static const int DEPTH = MAP_SIZE/TileAI::SIZE;

	// Vectors for ease of use, could just as easily be fixed size arrays
	TileAI tiles[WIDTH][DEPTH];

};

class AIEngine
{
public:
	AIEngine(void);
	~AIEngine(void);

	DrivingInput updateAI(Vehicle*);

private:
	void updatePath(Vehicle*);
	DrivingInput goToPoint(Vehicle*, glm::vec3);
};

