#include "PhysicsEntity.h"


PhysicsEntity::PhysicsEntity(void)
{
}


PhysicsEntity::~PhysicsEntity(void)
{
}

void PhysicsEntity::setActor(physx::PxRigidActor* a) {
	actor = a;
}

physx::PxRigidActor* PhysicsEntity::getActor() {
	return actor;
}

glm::vec3 PhysicsEntity::getPosition() {
	physx::PxVec3 position = actor->getGlobalPose().p;
	return glm::vec3(position.x, position.y, position.z);
}

glm::mat4 PhysicsEntity::getModelMatrix() {
	physx::PxMat44 oldM(actor->getGlobalPose());
	glm::mat4 newM;
	for(unsigned int i = 0; i < 4; i++) {
		for(unsigned int f = 0; f < 4; f++) {
			newM[i][f] = oldM[i][f];
		}
	}
	return newM;
}