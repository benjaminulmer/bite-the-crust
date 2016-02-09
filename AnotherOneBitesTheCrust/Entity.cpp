#include "Entity.h"


Entity::Entity(void)
{
}


Entity::~Entity(void)
{
}

glm::vec3 Entity::getPosition() {
	return position;
}

glm::mat4 Entity::getModelMatrix() {
	return glm::mat4(1.0f);
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