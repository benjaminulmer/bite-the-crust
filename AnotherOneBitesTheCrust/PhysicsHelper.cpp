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

	PxConvexMesh* convexMesh = nullptr;
	PxDefaultMemoryOutputStream buf;
	if(cooking->cookConvexMesh(meshDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		convexMesh = physics->createConvexMesh(id);
	}

	return convexMesh;
}

PxTriangleMesh* PhysicsHelper::createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* faces, const PxU32 numFaces)
{
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count           = numVerts;
	meshDesc.points.stride          = sizeof(PxVec3);
	meshDesc.points.data            = verts;

	meshDesc.triangles.count        = numFaces;
	meshDesc.triangles.stride       = 3*sizeof(PxU32);
	meshDesc.triangles.data         = faces;

	PxTriangleMesh* triangleMesh = nullptr;
	PxDefaultMemoryOutputStream buf;
	cooking->validateTriangleMesh(meshDesc);
	if (cooking->cookTriangleMesh(meshDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		triangleMesh = physics->createTriangleMesh(id);
	}

	return triangleMesh;
}