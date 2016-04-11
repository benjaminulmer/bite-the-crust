#pragma once

#include <vector>
#include <ctime>
#include <map>
#include <limits>
#include <glm.hpp>
#include <gtc\constants.hpp>
#include "Vehicle.h"
#include "Map.h"
#include "DeliveryManager.h"
#include "AICollisionEntity.h"

using std::vector;
using glm::vec3;

class AIEngine
{
public:
	AIEngine(void);
	~AIEngine(void);

	void updateAI(Vehicle*, Delivery, Map &, AICollisionEntity &);
	

private:
	void updatePath(Vehicle*, Delivery, Map &);
	void goToPoint(Vehicle*, const glm::vec3 &, const float &);
	void facePoint(Vehicle *, const glm::vec3&);
	void brake(Vehicle*, const float &);
	void trimPath(Vehicle*);
	void fireAt(Vehicle*, const glm::vec3 &);
	bool isStuck(Vehicle *);
	float angleToGoal(Vehicle*, const glm::vec3&);
	void shootPizza(Vehicle*);
	void refillPizzas(Vehicle*, Map &);
	void avoid(Vehicle*, const glm::vec3&);

	std::vector<glm::vec3> aStar(graphNode *, graphNode *, std::vector<graphNode*>, glm::vec3);
};

