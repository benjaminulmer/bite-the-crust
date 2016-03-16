#include "PhysicsHelper.h"
#include "VehicleSceneQueryData.h"
#include "Filtering.h"

using namespace physx;

PhysicsHelper::PhysicsHelper(PxPhysics* physics, PxCooking* cooking)
	: physics(physics), cooking(cooking) {}

PxConvexMesh* PhysicsHelper::createConvexMesh(const PxVec3* verts, const PxU32 numVerts)
{
	// Create descriptor for convex mesh
	PxConvexMeshDesc meshDesc;
	meshDesc.points.count		= numVerts;
	meshDesc.points.stride		= sizeof(PxVec3);
	meshDesc.points.data		= verts;
	meshDesc.flags				= PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

	PxConvexMesh* convexMesh = NULL;
	PxDefaultMemoryOutputStream buf;
	if(cooking->cookConvexMesh(meshDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		convexMesh = physics->createConvexMesh(id);
	}

	return convexMesh;
}

#include <iostream>

PxTriangleMesh* PhysicsHelper::createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* faces, const PxU32 numFaces)
{
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count           = numVerts;
	meshDesc.points.stride          = sizeof(PxVec3);
	meshDesc.points.data            = verts;

	meshDesc.triangles.count        = numFaces;
	meshDesc.triangles.stride       = 3*sizeof(PxU32);
	meshDesc.triangles.data         = faces;

	PxTriangleMesh* triangleMesh = NULL;
	PxDefaultMemoryOutputStream buf;
	if (cooking->cookTriangleMesh(meshDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		triangleMesh = physics->createTriangleMesh(id);
	}
	else {
		std::cout << "bad triangle mesh" << std::endl;
	}

	return triangleMesh;
}

std::vector<PxVec3> PhysicsHelper::glmVertsToPhysXVerts(std::vector<glm::vec3> verts)
{
	std::vector<PxVec3> toReturn = std::vector<PxVec3>(); 
	for (auto vert : verts)
	{
		toReturn.push_back(PxVec3(vert.x, vert.y, vert.z));
	}

	return toReturn;
}

std::vector<PxU32> PhysicsHelper::u16ToU32Faces(std::vector<unsigned short> faces)
{
	std::vector<PxU32> toReturn = std::vector<PxU32>(); 
	for (auto face : faces)
	{
		toReturn.push_back((PxU32)face);
	}

	return toReturn;
}