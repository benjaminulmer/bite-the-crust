#pragma once
#include "PhysicsEntity.h"

class DynamicEntity :
	public PhysicsEntity
{
public:
	DynamicEntity(void);
	virtual ~DynamicEntity(void);

	physx::PxRigidDynamic* getRigidDynamic();
};

