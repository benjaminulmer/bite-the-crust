#pragma once

#include "Renderable.h"
#include <glm/glm.hpp>

class Entity
{
public:
	Entity(void);
	~Entity(void);

	void setDefaultRotation(float radians, glm::vec3 axis);
	void setDefaultTranslation(glm::vec3 trans);
	void setDefaultScale(glm::vec3 scale);

	float getDefaultRotationAngle();
	glm::vec3 getDefaultRotationAxis();
	glm::vec3 getDefaultTranslation();
	glm::vec3 getDefaultScale();

	virtual glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	bool hasRenderable(); 
	void setRenderable(Renderable* r);
	Renderable* getRenderable();
private:
	glm::vec3 position;
	Renderable* renderable;
	float defaultRotation;
	glm::vec3 defaultRotationAxis;
	glm::vec3 defaultTranslation;
	glm::vec3 defaultScale;
};

