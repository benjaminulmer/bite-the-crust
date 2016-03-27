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
			if (!players[i]->isAI)
				d->location->ground->setTexture(d->location->groundTexture);
			deliveries[players[i]] = newDelivery(players[i]);
			freeLocations.push_back(d->location); // Free the tile back up, since it wasn't claimed
			// Free the location after assigning delivery, so you don't get the same location twice
		}
	}
}

Delivery DeliveryManager::newDelivery(Vehicle* player) {
	Delivery d;
	if (freeLocations.size() == 0) {
		gameOverSignal(scores);
		return d;
	}
	std::uniform_int_distribution<int> dist(0, freeLocations.size()-1);
	int randomTile = dist(generator);
	d.location = freeLocations[randomTile];
	d.time = 1000.0 * 20.0; // 20 seconds
	// Only draw delivery tile for player
	if (!player->isAI)
		d.location->ground->setTexture(deliveryTextures[player]);
	player->newDestination = true;
	if (!player->isAI)
		deliveryLocationUpdate(map->getTileLocation(d.location));
	return d;
}

void DeliveryManager::pizzaLanded(PizzaBox* pizza) {
	Tile* tile = map->getTile(pizza->getPosition());
	if (tile == nullptr) // The pizza right now can land outside the tiles
		return;
	if (tile == deliveries[pizza->owner].location) {
		if (!pizza->owner->isAI)
			tile->ground->setTexture(tile->groundTexture);
		tile->house->setTexture(pizza->owner->houseTexture);
		int score = 5 + (int)(ceil(deliveries[pizza->owner].time / 1000.0f / 3)); // Bonus of remaining time in seconds, divided by 3
		scores[pizza->owner] += score;
		freeLocations.erase(std::remove(freeLocations.begin(), freeLocations.end(), deliveries[pizza->owner].location), freeLocations.end());
		deliveries[pizza->owner] = newDelivery(pizza->owner);
	}
	else
		pizza->owner->pizzaDelivered = false;
}

void DeliveryManager::refillPizza(Vehicle* player) {
	if (player->pizzaCount < Vehicle::MAX_PIZZAS)
		pizzasRefilled(player);
	player->pizzaCount = Vehicle::MAX_PIZZAS;
}