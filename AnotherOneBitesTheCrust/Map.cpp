#include "Map.h"


Map::Map(void)
{
}


Map::~Map(void)
{
}

Tile* Map::getTile(glm::vec3 position) {
	int width = position.x / tileSize;
	int depth = position.z / tileSize;
	return &tiles[width][depth];
}