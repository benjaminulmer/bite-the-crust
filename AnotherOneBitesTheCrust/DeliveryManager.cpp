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
			scores[players[i]]--; // Decrement score for now, while testing things out
			deliveries[players[i]] = newDelivery();
		}
	}
}

Delivery DeliveryManager::newDelivery() {
	Delivery d;
	int randomTile = rand() % freeLocations.size();
	d.location = freeLocations[randomTile];
	d.time = 1000.0 * 10.0; // 10 seconds
	d.location->ground->setTexture(deliverTexture);
	return d;
}

void DeliveryManager::pizzaLanded(PizzaBox* pizza) {
	Tile* tile = map->getTile(pizza->getPosition());
	if (tile == nullptr) // The pizza right now can land outside the tiles
		return;
	if (tile == deliveries[pizza->owner].location) {
		tile->ground->setTexture(tile->groundTexture);
		scores[pizza->owner]++;
		deliveries[pizza->owner] = newDelivery();
		// Anyone who was delivering to this location needs a new delivery
		for (int i = 0; i < (int)players.size(); i++) {
			Tile* t = deliveries[players[i]].location;
			if (t == tile) {
				t->ground->setTexture(t->groundTexture);
				deliveries[players[i]] = newDelivery();
			}
		}
	}
}

void DeliveryManager::refillPizza(Vehicle* player) {
	player->pizzaCount = 3;
}