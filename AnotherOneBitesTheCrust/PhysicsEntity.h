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

	glm::vec3 getPosition();

	virtual void setActor(physx::PxRigidActor* a);
	virtual physx::PxRigidActor* getActor();

	void setPosition(glm::vec3);
	virtual glm::mat4 getModelMatrix();

	virtual void onSleep();

protected:
	physx::PxRigidActor* actor;
};

