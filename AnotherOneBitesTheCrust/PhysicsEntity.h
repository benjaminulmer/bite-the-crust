#pragma once
#include "Entity.h"
#include <PxPhysicsAPI.h>
#include <glm.hpp>

class PhysicsEntity :
	public Entity
{
public:
	PhysicsEntity(void);
	~PhysicsEntity(void);

	void setPosition(glm::vec3);
	glm::vec3 getPosition();
	virtual glm::mat4 getModelMatrix();

	virtual void setActor(physx::PxRigidActor* a);
	virtual physx::PxRigidActor* getActor();

protected:
	physx::PxRigidActor* actor;
};

