#include "DecorationEntity.h"


DecorationEntity::DecorationEntity(void)
{

	type = EntityType::DECORATION;
}


glm::vec3 DecorationEntity::getPosition()
{
	return glm::vec3();
}

glm::mat4 DecorationEntity::getModelMatrix()
{
	return glm::mat4();
}

DecorationEntity::~DecorationEntity(void)
{
}
