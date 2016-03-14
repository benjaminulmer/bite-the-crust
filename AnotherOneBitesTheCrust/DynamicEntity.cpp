#include "DynamicEntity.h"

DynamicEntity::DynamicEntity(void)
{
	type = EntityType::DYNAMIC;
}

DynamicEntity::~DynamicEntity(void)
{
}

physx::PxRigidDynamic* DynamicEntity::getRigidDynamic() {
	return (physx::PxRigidDynamic*)actor;
}
