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
	std::vector<TileEntity> entityTemplates;
	std::vector<PhysicsEntity*> staticEntities;
	std::vector<graphNode*> nodes;
	glm::vec3 goal;
	Entity* ground;
	GLuint groundTexture;
	int groundRotationDeg;
	bool deliverable;
	Entity* house; // null if the tile doesn't have a house
	bool pickup;
};

class Map
{
public:
	Map(void);
	~Map(void);

	int depth, width, tileSize;

	std::vector<graphNode *> allNodes;
	std::vector<std::vector<Tile>> tiles;
	glm::vec3 pickup;

	Tile* getTile(glm::vec3 position);
	//void offsetPosition(Tile* tile, glm::vec3 &position);
};

