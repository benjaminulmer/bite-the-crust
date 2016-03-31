#pragma once
#include "Entity.h"

class DecorationEntity :
	public Entity
{
public:
	DecorationEntity(void);
	~DecorationEntity(void);

	glm::vec3 getPosition();
	glm::mat4 getModelMatrix();

private:
	glm::mat4 modelMatrix;
};

