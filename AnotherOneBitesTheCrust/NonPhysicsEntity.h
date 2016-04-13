#pragma once
#include "Entity.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>

class NonPhysicsEntity :
	public Entity
{
public:
	NonPhysicsEntity(void);
	virtual ~NonPhysicsEntity(void);

	void rotate(float angle, glm::vec3 axis);
	void translate(glm::vec3 pos);
	void setTranslation(glm::vec3 pos);

	glm::vec3 getPosition();
	glm::mat4 getModelMatrix();

private:
	glm::mat4 rotMatrix;
	glm::mat4 transMatrix;
	glm::mat4 modelMatrix;
};

