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

glm::vec3 Map::getTileLocation(Tile *tile) {
	for (unsigned int i = 0; i < tiles.size(); i++)
	{
		for (unsigned int j = 0; j < tiles[i].size(); j++)
		{
			if (tile == &tiles[i][j]) {
				return glm::vec3(j*tileSize + tileSize/2, 0, i*tileSize + tileSize/2);
			}
		}
	}
	return glm::vec3();
}