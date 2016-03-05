#pragma once
#include <PxPhysicsAPI.h>
#include "PhysicsEntityInfo.h"

class PhysicsHelper
{
public:
	PhysicsHelper(physx::PxPhysics* physics, physx::PxCooking* cooking);
	~PhysicsHelper(void);

	physx::PxRigidDynamic* createBox(physx::PxMaterial* material);

	physx::PxRigidStatic* createDrivablePlane(physx::PxMaterial* material);

	physx::PxConvexMesh* createConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts);

	physx::PxTriangleMesh* createTriangleMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, const physx::PxU32* faces, const physx::PxU32 numFaces);

	std::vector<physx::PxVec3> glmVertsToPhysXVerts(std::vector<glm::vec3> verts);

private:
	physx::PxPhysics* physics;
	physx::PxCooking* cooking;
};

