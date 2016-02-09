#include "Entity.h"


Entity::Entity(void)
{
}


Entity::~Entity(void)
{
}

void Entity::setDefaultRotation(float radians, glm::vec3 axis)
{
	defaultRotation = radians;
	defaultRotationAxis = axis;
}

void Entity::setDefaultTranslation(glm::vec3 trans)
{
	defaultTranslation = trans;
}

void Entity::setDefaultScale(glm::vec3 scale)
{
	defaultScale = scale;
}

float Entity::getDefaultRotationAngle()
{
	return defaultRotation;
}

glm::vec3 Entity::getDefaultRotationAxis()
{
	return defaultRotationAxis;
}

glm::vec3 Entity::getDefaultTranslation()
{
	return defaultTranslation;
}

glm::vec3 Entity::getDefaultScale()
{
	return defaultScale;
}
	


glm::vec3 Entity::getPosition() {
	return position;
}

void Entity::setPosition(glm::vec3 pos) {
	position = pos;
}

bool Entity::hasRenderable() {
	return (renderable != NULL);
}

void Entity::setRenderable(Renderable* r) {
	renderable = r;
}

Renderable* Entity::getRenderable() {
	return renderable;
}