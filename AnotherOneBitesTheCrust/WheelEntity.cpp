#include "WheelEntity.h"

using namespace physx;

WheelEntity::WheelEntity(Vehicle* vehicle, PxShape* physicsShape)
{
	this->vehicle = vehicle;
	this->physicsShape = physicsShape;
}

glm::vec3 WheelEntity::getPosition()
{
	PxTransform vehicleTransform = vehicle->getDynamicActor()->getGlobalPose();
	PxTransform transform = physicsShape->getLocalPose();
	transform = transform * vehicleTransform;
	return glm::vec3(transform.p.x, transform.p.y, transform.p.z); 
}

glm::mat4 WheelEntity::getModelMatrix()
{
	glm::mat4 vehicleMatrix = vehicle->getModelMatrix();
	PxMat44 oldM(physicsShape->getLocalPose());
	
	glm::mat4 newM;
	for(unsigned int i = 0; i < 4; i++)
	{
		for(unsigned int f = 0; f < 4; f++)
		{
			newM[i][f] = oldM[i][f];
		}
	}
	return vehicleMatrix * newM;
}

WheelEntity::~WheelEntity(void)
{
}
