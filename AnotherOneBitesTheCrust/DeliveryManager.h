#pragma once
#include <map>
#include "Vehicle.h"
#include "Map.h"
#include "PizzaBox.h"
#include <time.h>
#include <iostream>
#include <sigslot.h>
#include <string>

struct Delivery {
	Tile* location;
	double time; // In milliseconds

	Delivery() {
		location = nullptr;
	}
};

class DeliveryManager :
	public sigslot::has_slots<>
{
public:
	DeliveryManager(void);
	~DeliveryManager(void);

	Map* map;
	std::vector<Tile*> freeLocations;
	std::vector<Vehicle*> players;
	std::map<Vehicle*, Delivery> deliveries;
	std::map<Vehicle*, int> scores;
	GLuint deliverTexture;
	
	void addDeliveryLocation(Tile* location);
	void addPlayer(Vehicle* player);
	void assignDeliveries();
	std::string getDeliveryText(Vehicle* player);
	int getScore(Vehicle* player);
	void timePassed(double timeMs);
	Delivery newDelivery();
	void pizzaLanded(PizzaBox* pizza);
	void refillPizza(Vehicle* player);
};

