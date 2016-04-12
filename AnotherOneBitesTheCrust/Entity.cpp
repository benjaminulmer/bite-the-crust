#include "Entity.h"
#include <iostream>

void Entity::testPrint() 
{
	std::cout << "I am an entity! " << (int)type << std::endl;
}

Entity::Entity(void)
{
	defaultRotation = 0;
	defaultRotationAxis = glm::vec3(0,1,0);
	defaultTranslation = glm::vec3(0,0,0);
	defaultScale = glm::vec3(1.0f);
	renderable = nullptr;

	type = EntityType::BASE;
}

Entity::~Entity(void)
{
	delete renderable;
}

void Entity::setTexture(GLuint tex)
{
	texture = tex;
}

GLuint Entity::getTexture()
{
	return texture;
}

glm::vec3 Entity::getPosition() {
	return glm::vec3(0, 0, 0);
}

glm::mat4 Entity::getModelMatrix() {
	return glm::mat4(1.0f);
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
