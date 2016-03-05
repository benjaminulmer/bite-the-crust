#pragma once
#include <glm.hpp>
#include <vector>
#include "PhysicsEntity.h"
#include "graphNode.h"

// Representation of the entities in a tile, which will be turned into actual entities by the game engine
// I think we'll want to add some kind of 'type' field to distinguish between static and dynamic entities later
struct TileEntity 
{
	std::string name;
	glm::vec3 position;
	float rotationDeg;
};

struct Tile
{
	// Should these coordinates be relative to the tile or the map?
	// If it's relative to the tile, need a method to get player space -> tile space
	glm::vec3 entrance, exit;
	std::string groundModel;
	std::vector<TileEntity> entities;
	std::vector<graphNode*> nodes;
	Entity* ground;
	GLuint groundTexture;
	int groundRotationDeg;
	bool deliverable;
};

class Map
{
public:
	Map(void);
	~Map(void);

	//static const int MAP_SIZE = 70;
	//static const int WIDTH = MAP_SIZE/Tile::SIZE;
	//static const int DEPTH = MAP_SIZE/Tile::SIZE;
	int depth, width, tileSize; // Loading everything dynamically from the json now

	std::vector<graphNode *> allNodes;
	std::vector<std::vector<Tile>> tiles;

	Tile* getTile(glm::vec3 position);
	//void offsetPosition(Tile* tile, glm::vec3 &position);
};

