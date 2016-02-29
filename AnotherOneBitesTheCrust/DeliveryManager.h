#pragma once
#include <map>
#include "Vehicle.h"
#include "Map.h"
#include <time.h>
#include <iostream>
#include <sigslot.h>

struct Delivery {
	Tile* location;
	tm time;
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
	
	void addDeliveryLocation(Tile* location);
	void addPlayer(Vehicle* player);
	void assignDeliveries();
	void pizzaShot(Vehicle* player);
};

