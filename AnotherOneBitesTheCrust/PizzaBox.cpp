#include "PizzaBox.h"

PizzaBox::PizzaBox(Vehicle* owner) : owner(owner)
{
	type = EntityType::PIZZABOX;
}

PizzaBox::~PizzaBox(void)
{
}
