#include "DynamicEntity.h"


DynamicEntity::DynamicEntity(void)
{
}


DynamicEntity::~DynamicEntity(void)
{
}

void DynamicEntity::setActor(physx::PxRigidDynamic* a) {
	actor = a;
}

physx::PxRigidActor* DynamicEntity::getActor() {
	return actor;
}

physx::PxRigidDynamic* DynamicEntity::getDynamicActor() {
	return (physx::PxRigidDynamic*)(actor);
}