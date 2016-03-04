#include "Map.h"


Map::Map(void)
{
}


Map::~Map(void)
{
	for(std::vector<Tile> v : tiles)
	{
		for(Tile t : v)
		{
			for(graphNode * n : t.nodes)
			{
				// This doesn't work right now because multiple tiles have the same graphNode pointers
				//delete n;
			}
		}
	}
}

Tile* Map::getTile(glm::vec3 position){
	int width = (int)position.x / tileSize;
	int depth = (int)position.z / tileSize;
	return &tiles[width][depth];
}