#include "Map.h"


Map::Map(void)
{
}


Map::~Map(void)
{
}

Tile* Map::getTile(glm::vec3 position) {
	int width = (int)position.x / tileSize;
	int depth = (int)position.z / tileSize;
	return &tiles[width][depth];
}