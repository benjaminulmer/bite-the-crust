#pragma once
#include <glm.hpp>
#include "Entity.h"

struct AICollisionEntity
{
	glm::vec3 pos;
	float radius;
	float distance;

	Entity* entity;
};
