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

	virtual void setActor(physx::PxRigidActor* a);
	virtual physx::PxRigidActor* getActor();
	glm::vec3 getPosition();
	void setPosition(glm::vec3);
	glm::mat4 getModelMatrix();

protected:
	physx::PxRigidActor* actor;
};

