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
	int width = glm::clamp((int)position.z / tileSize, 0, (int)tiles.size() - 1 );
	int depth = glm::clamp((int)position.x / tileSize, 0, (int)tiles.at(0).size() - 1);
	return &tiles[width][depth];
}