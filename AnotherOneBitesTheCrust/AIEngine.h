#pragma once

#include <vector>
#include <glm\glm.hpp>
#include "DrivingInput.h"

using std::vector;
using glm::vec3;

struct Player
{
	vec3 position;

	// TODO: Need a way to store current path + how long since last updated
};

struct ControllerInput
{
	// TODO: Need to store currently pressed buttons, triggers, and analog sticks
};

struct Tile
{
	// Should these coordinates be relative to the tile or the map?
	// If it's relative to the tile, need a method to get player space -> tile space
	vec3 entrance, exit;
};

struct Map
{
	// Vectors for ease of use, could just as easily be fixed size arrays
	vector<vector<Tile>> tiles;

};

class AIEngine
{
public:
	AIEngine(void);
	~AIEngine(void);

	// Returns the ControllerInput for the AI player pointed to by Player
	DrivingInput updateAI();
};

