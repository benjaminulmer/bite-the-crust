#pragma once
#include "Vehicle.h"

class PizzaBox :
	public DynamicEntity
{
public:
	PizzaBox(Vehicle* owner);
	~PizzaBox(void);

	Vehicle* owner;
};

