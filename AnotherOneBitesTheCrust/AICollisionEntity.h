#pragma once
#include <glm.hpp>
#include "Entity.h"

struct AICollisionEntity
{
	glm::vec3 pos;
	float radius;

	Entity* entity;
};
