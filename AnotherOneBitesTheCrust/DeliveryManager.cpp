#include "DeliveryManager.h"

DeliveryManager::DeliveryManager(void)
{
	std::random_device rd;
	generator.seed(rd());
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
			deliveries[players[i]] = newDelivery(players[i]);
		}
	}
}

std::string DeliveryManager::getDeliveryText(Vehicle* player) {
	int seconds = (int)(ceil(deliveries[player].time / 1000.0f));
	int minutes = seconds / 60;
	seconds = seconds % 60;
    std::string text = std::to_string(minutes);
	text = text + "M " + std::to_string(seconds) + "S";
	return text;
}

int DeliveryManager::getScore(Vehicle* player) {
	return scores[player];
}

void DeliveryManager::timePassed(double timeMs) {
	for (int i = 0; i < (int)players.size(); i++) {
		Delivery* d = &deliveries[players[i]];
		d->time = d->time - timeMs;
		if (d->time <= 0.0) {
			d->location->ground->setTexture(d->location->groundTexture);
			freeLocations.push_back(d->location); // Free the tile back up for now, not Splatoon yet
			scores[players[i]]--; // Decrement score for now, while testing things out
			deliveries[players[i]] = newDelivery(players[i]);
		}
	}
}

Delivery DeliveryManager::newDelivery(Vehicle* player) {
	Delivery d;
	std::uniform_int_distribution<int> dist(0, freeLocations.size()-1);
	int randomTile = dist(generator);
	d.location = freeLocations[randomTile];
	d.time = 1000.0 * 20.0; // 10 seconds
	d.location->ground->setTexture(deliveryTextures[player]);
	freeLocations.erase(freeLocations.begin() + randomTile);
	return d;
}

void DeliveryManager::pizzaLanded(PizzaBox* pizza) {
	Tile* tile = map->getTile(pizza->getPosition());
	if (tile == nullptr) // The pizza right now can land outside the tiles
		return;
	if (tile == deliveries[pizza->owner].location) {
		freeLocations.push_back(tile); // Free the tile back up for now, not Splatoon yet
		tile->ground->setTexture(tile->groundTexture);
		scores[pizza->owner]++;
		deliveries[pizza->owner] = newDelivery(pizza->owner);
	}
}

void DeliveryManager::refillPizza(Vehicle* player) {
	player->pizzaCount = 3;
}