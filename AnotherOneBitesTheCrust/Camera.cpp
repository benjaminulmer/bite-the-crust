#include "Camera.h"

Camera::Camera(Vehicle* vehicle)
{
	this->vehicle = vehicle;
	reverseCam = false;

	for (unsigned int i = 0; i < BUFFER_SIZE; i++)
	{
		posBufferForward[i] = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
		posBufferReverse[i] = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
	}
	posBufferIndex = 0;
	position = (posBufferForward[BUFFER_SIZE]);
	lookAtPosition = vehicle->getPosition();
	upVector = glm::vec3(0,1,0);

	type = EntityType::CAMERA;
}

void Camera::update()
{
	// Update buffer from vehicle position
	posBufferForward[posBufferIndex] = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
	posBufferReverse[posBufferIndex] = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,8,15,0));

	// Don't let camera fall below ground
	if (posBufferForward[posBufferIndex].y < 1) 
	{
			posBufferForward[posBufferIndex].y = 1;
	}
	if (posBufferReverse[posBufferIndex].y < 1) 
	{
			posBufferReverse[posBufferIndex].y = 1;
	}
	posBufferIndex = (posBufferIndex + 1) % BUFFER_SIZE;

	// Set location based on which mode (forward vs reverse)
	position = (reverseCam) ? posBufferReverse[posBufferIndex] : posBufferForward[posBufferIndex];
	lookAtPosition = vehicle->getPosition();
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

glm::vec3 Camera::getUpVector() {
	return upVector;
}

glm::vec3 Camera::getLookAtPosition() {
	return lookAtPosition;
}

void Camera::setReverseCam(bool val) {
	reverseCam = val;
}

bool Camera::isReverseCam() {
	return reverseCam;
}