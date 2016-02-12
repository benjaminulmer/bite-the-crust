#include "DynamicEntity.h"


DynamicEntity::DynamicEntity(void)
{
}


DynamicEntity::~DynamicEntity(void)
{
}

void DynamicEntity::setActor(physx::PxRigidDynamic* a) {
	actor = a;
	dynamicActor = a;
}

physx::PxRigidActor* DynamicEntity::getActor() {
	return dynamicActor;
}

physx::PxRigidDynamic* DynamicEntity::getDynamicActor() {
	return dynamicActor;
}