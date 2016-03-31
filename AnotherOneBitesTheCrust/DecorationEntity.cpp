#include "DecorationEntity.h"

DecorationEntity::DecorationEntity(void)
{
	type = EntityType::DECORATION;
}

void DecorationEntity::rotate(float angle, glm::vec3 axis)
{
	modelMatrix = glm::rotate(modelMatrix, angle, axis);
}

void DecorationEntity::translate(glm::vec3 pos)
{
	modelMatrix = glm::translate(modelMatrix, pos);
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
