#pragma once
#include "Vehicle.h"

class PizzaBox :
	public DynamicEntity
{
public:
	PizzaBox(Vehicle* owner);
	virtual ~PizzaBox(void);

	Vehicle* owner;
};

