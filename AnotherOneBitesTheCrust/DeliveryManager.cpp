#include "DeliveryManager.h"


DeliveryManager::DeliveryManager(void)
{
	srand((unsigned int)time(NULL));
}


DeliveryManager::~DeliveryManager(void)
{
}

void DeliveryManager::addDeliveryLocation(Tile* location) {
	freeLocations.push_back(location);
}

void DeliveryManager::addPlayer(Vehicle* player) {
	players.push_back(player);
	scores[player] = 0;
}



void DeliveryManager::assignDeliveries() {
	if (freeLocations.size() != 0) {
		for (int i = 0; i < (int)players.size(); i++) {
			deliveries[players[i]] = newDelivery();
		}
	}
}

Delivery DeliveryManager::newDelivery() {
	Delivery d;
	int randomTile = rand() % freeLocations.size();
	std::cout << "Deliver to tile: " << randomTile << std::endl;
	d.location = freeLocations[randomTile];
	d.time.tm_min = 2;
	d.time.tm_sec = 0;
	return d;
}

void DeliveryManager::pizzaShot(Vehicle* player) {
	Tile* tile = map->getTile(player->getPosition());
	if (tile == deliveries[player].location) {
		std::cout << "Delivered!" << std::endl;
		scores[player]++;
		deliveries[player] = newDelivery();
		// Anyone who was delivering to this location needs a new delivery
		for (int i = 0; i < (int)players.size(); i++) {
			if (deliveries[players[i]].location == tile)
				deliveries[players[i]] = newDelivery();
		}
	}
}