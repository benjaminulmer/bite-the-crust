#include "DeliveryManager.h"


DeliveryManager::DeliveryManager(void)
{
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
			Delivery d;
			d.location = freeLocations[0];
			d.time.tm_min = 2;
			d.time.tm_sec = 0;
			deliveries[players[i]] = d;
		}
	}
}

void DeliveryManager::pizzaShot(Vehicle* player) {
	std::cout << "pizza shot" << std::endl;
	if (map->getTile(player->getPosition()) == deliveries[player].location) {
		std::cout << "Delivered!" << std::endl;
	}
}