#pragma once

#include "Renderable.h"
#include <glm/glm.hpp>

class Entity
{
public:
	Entity(void);
	~Entity(void);

	virtual glm::vec3 getPosition();
	virtual glm::mat4 getModelMatrix();
	void setPosition(glm::vec3 pos);
	bool hasRenderable(); 
	void setRenderable(Renderable* r);
	Renderable* getRenderable();
private:
	glm::vec3 position;
	Renderable* renderable;
};

