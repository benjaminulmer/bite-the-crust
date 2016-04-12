#include "Camera.h"

using namespace physx;

Camera::Camera(Vehicle* vehicle, PxScene* scene)
{
	this->vehicle = vehicle;
	this->scene = scene;
	reverseCam = false;

	for (unsigned int i = 0; i < BUFFER_SIZE; i++)
	{
		posBufferForward[i] = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,6,-15,0));
		posBufferReverse[i] = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,6,-15,0));
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
	posBufferForward[posBufferIndex] = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,6,-15,0));
	posBufferReverse[posBufferIndex] = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,6,15,0));

	glm::vec3 properPos = vehicle->getPosition() + glm::vec3(vehicle->getModelMatrix() * glm::vec4(0,6,-15,0));

	PxVec3 car = vehicle->getActor()->getGlobalPose().p;
	PxVec3 cam = PxVec3(properPos.x, properPos.y, properPos.z);
	PxVec3 carToCam = cam - car;
	PxVec3 camToCar = car - cam;

	/*PxF32 dist = carToCam.magnitude() - 3.0f;
	if (dist <= 0)
		dist = 0.1f;*/

	carToCam.normalize();
	camToCar.normalize();
	car += carToCam * 3.0f;
	PxRaycastBuffer buffer;

	scene->raycast(cam, camToCar, 0.1f, buffer);
	if (buffer.hasBlock) 
	{
		PxRaycastHit hit = buffer.block;
		PxVec3 newLoc = camToCar * (hit.distance - 0.1f);

		//posBufferForward[posBufferIndex] = vehicle->getPosition() + glm::vec3(newLoc.x, newLoc.y, newLoc.z);
		//posBufferReverse[posBufferIndex] = vehicle->getPosition() + glm::vec3(0,12,15,0));
	}
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

	glm::vec3 toDest = glm::normalize(vehicle->deliveryLocation - position);
	glm::vec3 viewDir = glm::normalize(lookAtPosition - position);
	glm::vec3 left = glm::normalize(glm::cross(upVector, viewDir));

	float angle = glm::acos(glm::dot(toDest, viewDir));
	float leftCos = glm::dot(toDest, left);

	if (angle > 0.8f)
	{
		arrowState = (leftCos < 0) ? ArrowState::RIGHT : ArrowState::LEFT;
	}
	else
	{
		arrowState = ArrowState::ON_SCREEN;
	}
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