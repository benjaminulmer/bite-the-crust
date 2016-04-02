#include "DecorationEntity.h"

DecorationEntity::DecorationEntity(void)
{
	modelMatrix = glm::mat4(1);
	type = EntityType::DECORATION;
}

void DecorationEntity::rotate(float angle, glm::vec3 axis)
{
	rotMatrix = glm::rotate(rotMatrix, angle, axis);
	modelMatrix = transMatrix * rotMatrix;
}

void DecorationEntity::translate(glm::vec3 pos)
{
	transMatrix = glm::translate(transMatrix, pos);
	modelMatrix = transMatrix * rotMatrix;
}

glm::vec3 DecorationEntity::getPosition()
{
	return glm::vec3(modelMatrix * glm::vec4(0, 0, 0, 1));
}

glm::mat4 DecorationEntity::getModelMatrix()
{
	return modelMatrix;
}

DecorationEntity::~DecorationEntity(void)
{
}
