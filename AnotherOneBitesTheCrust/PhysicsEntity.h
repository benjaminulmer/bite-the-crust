#pragma once
#include "Entity.h"
#include <PxPhysicsAPI.h>
#include <glm\glm.hpp>

class PhysicsEntity :
	public Entity
{
public:
	PhysicsEntity(void);
	~PhysicsEntity(void);

	void setActor(physx::PxRigidActor* a);
	physx::PxRigidActor* getActor();
	glm::vec3 getPosition();
private:
	physx::PxRigidActor* actor;
};

