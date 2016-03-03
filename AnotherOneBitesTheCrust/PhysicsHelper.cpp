#include "PhysicsHelper.h"
#include "VehicleSceneQueryData.h"
#include "Filtering.h"

using namespace physx;

PhysicsHelper::PhysicsHelper(PxPhysics* physics, PxCooking* cooking)
	: physics(physics), cooking(cooking) {}

PxActor* PhysicsHelper::createTriggerVolume()
{
	PxSphereGeometry geometry(10.0f); 
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.5f);

	PxRigidStatic* actor = PxCreateStatic(*physics, transform, geometry, *material);
	PxShape* shape;
	actor->getShapes(&shape, 1);
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);

	return actor;
}

PxRigidStatic* PhysicsHelper::createDrivablePlane(PxMaterial* material)
{
	//Add a plane to the scene.
	PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0,1,0,0), *material);

	//Get the plane shape so we can set query and simulation filter data.
	PxShape* shape;
	groundPlane->getShapes(&shape, 1);

	//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
	PxFilterData qryFilterData;
	qryFilterData.word3 = (PxU32)Surface::DRIVABLE;
	shape->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	PxFilterData simFilterData;
	simFilterData.word0 = (PxU32)FilterFlag::GROUND;
	simFilterData.word1 = (PxU32)FilterFlag::GROUND_AGAINST;
	shape->setSimulationFilterData(simFilterData);

	return groundPlane;
}

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

