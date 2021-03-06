#pragma once
#include "Renderable.h"
#include <glm.hpp>

enum class EntityType
{
	BASE,
	CAMERA,
	NON_PHYSICS,
	PHYSICS,
	DYNAMIC,
	STATIC,
	PIZZABOX,
	VEHICLE,
	WHEEL
};

class Entity
{
public:
	Entity(void);
	virtual ~Entity(void);

	EntityType type;
	void testPrint();

	// THESE NEED TO DIE IN A FIRE
	// END THINGS THAT NEED TO DIE IN A FIRE

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

