#include "NonPhysicsEntity.h"

NonPhysicsEntity::NonPhysicsEntity(void)
{
	modelMatrix = glm::mat4(1);
	type = EntityType::DECORATION;
}

void NonPhysicsEntity::rotate(float angle, glm::vec3 axis)
{
	rotMatrix = glm::rotate(rotMatrix, angle, axis);
	modelMatrix = transMatrix * rotMatrix;
}

void NonPhysicsEntity::translate(glm::vec3 pos)
{
	transMatrix = glm::translate(transMatrix, pos);
	modelMatrix = transMatrix * rotMatrix;
}

glm::vec3 NonPhysicsEntity::getPosition()
{
	return glm::vec3(modelMatrix * glm::vec4(0, 0, 0, 1));
}

glm::mat4 NonPhysicsEntity::getModelMatrix()
{
	return modelMatrix;
}

NonPhysicsEntity::~NonPhysicsEntity(void)
{
}
