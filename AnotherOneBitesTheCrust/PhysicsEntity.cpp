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