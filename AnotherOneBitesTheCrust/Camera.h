#pragma once
#include "Entity.h"
#include "glm/glm.hpp"


class Camera : public Entity
{
public:
	Camera(void);
	~Camera(void);

	void setPosition(glm::vec3 v);
	glm::vec3 getPosition();
	void setUpVector(glm::vec3 v);
	glm::vec3 getUpVector();
	void setLookAtPosition(glm::vec3 v);
	glm::vec3 getLookAtPosition();

private:
	glm::vec3 position;
	glm::vec3 upVector;
	glm::vec3 lookAtPosition; // The position the camera is looking at
};

