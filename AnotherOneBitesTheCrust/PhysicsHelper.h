#pragma once
#include <PxPhysicsAPI.h>
#include "PhysicsEntityInfo.h"

class PhysicsHelper
{
public:
	static physx::PxRigidDynamic* createBox(physx::PxMaterial* material, physx::PxPhysics* physics, physx::PxVec3 dimensions);

	static physx::PxActor* createTriggerVolume(physx::PxPhysics* physics);

	static physx::PxRigidStatic* createDrivablePlane(physx::PxMaterial* material, physx::PxPhysics* physics);

	static physx::PxConvexMesh* createConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking);

	static std::vector<physx::PxVec3> glmVertsToPhysXVerts(std::vector<glm::vec3> verts);
};

