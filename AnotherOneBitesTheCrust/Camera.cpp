#include "Camera.h"


Camera::Camera(void)
{
}


Camera::~Camera(void)
{
}

void Camera::setPosition(glm::vec3 v) {
	position = v;
}

glm::vec3 Camera::getPosition() {
	return position;
}

void Camera::setUpVector(glm::vec3 v) {
	upVector = v;
}

glm::vec3 Camera::getUpVector() {
	return upVector;
}

void Camera::setLookAtPosition(glm::vec3 v) {
	lookAtPosition = v;
}

glm::vec3 Camera::getLookAtPosition() {
	return lookAtPosition;
}
