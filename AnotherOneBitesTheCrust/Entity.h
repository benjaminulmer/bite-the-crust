#pragma once
#include "Renderable.h"
#include <glm.hpp>

enum class EntityType
{
	BASE,
	PHYSICS,
	DYNAMIC,
	PIZZABOX,
	VEHICLE,
	WHEEL
};

class Entity
{
public:
	Entity(void);
	~Entity(void);

	EntityType type;
	void testPrint();

	// THESE NEED TO DIE IN A FIRE
	void setDefaultRotation(float radians, glm::vec3 axis);
	void setDefaultTranslation(glm::vec3 trans);
	void setDefaultScale(glm::vec3 scale);
	float getDefaultRotationAngle();
	glm::vec3 getDefaultRotationAxis();
	glm::vec3 getDefaultTranslation();
	glm::vec3 getDefaultScale();
	// END THINGS THAT NEED TO DIE IN A FIRE

	virtual void setPosition(glm::vec3 pos);
	virtual glm::vec3 getPosition();
	virtual glm::mat4 getModelMatrix();

	// Component for rendering
	bool hasRenderable(); 

	void setRenderable(Renderable* r);
	Renderable* getRenderable();

	void setTexture(GLuint tex);
	GLuint getTexture();
	

private:
	Renderable* renderable;
	GLuint texture;

	float defaultRotation;
	glm::vec3 defaultRotationAxis;
	glm::vec3 defaultTranslation;
	glm::vec3 defaultScale;
};

