#pragma once
#include "physicsentity.h"
class DynamicEntity :
	public PhysicsEntity
{
public:
	DynamicEntity(void);
	~DynamicEntity(void);

	void setActor(physx::PxRigidDynamic* a);
	physx::PxRigidActor* getActor();
	physx::PxRigidDynamic* getDynamicActor();
private:
	physx::PxRigidDynamic* dynamicActor;
};

