#pragma once
#include "PhysicsEntity.h"

class DynamicEntity :
	public PhysicsEntity
{
public:
	DynamicEntity(void);
	~DynamicEntity(void);

	physx::PxRigidDynamic* getRigidDynamic();
};

