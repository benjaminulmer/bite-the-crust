#include "Camera.h"


Camera::Camera(void)
{
}


Camera::~Camera(void)
{
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
