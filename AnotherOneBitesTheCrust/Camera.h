#pragma once
#include "Entity.h"
#include "glm.hpp"
#include <sigslot.h>

class Camera
	: public Entity, public sigslot::has_slots<>
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

	void setReverseCam(bool val);
	bool isReverseCam();

private:
	glm::vec3 position;
	glm::vec3 upVector;
	glm::vec3 lookAtPosition;
	bool reverseCam;
};

