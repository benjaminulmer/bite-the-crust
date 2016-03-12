#include "DynamicEntity.h"

DynamicEntity::DynamicEntity(void)
{
}

DynamicEntity::~DynamicEntity(void)
{
}

physx::PxRigidDynamic* DynamicEntity::getRigidDynamic() {
	return (physx::PxRigidDynamic*)actor;
}
