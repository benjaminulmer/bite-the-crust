#include "PhysicsCreator.h"
#include "VehicleSceneQueryData.h"
#include "Filtering.h"

using namespace physx;

PxRigidDynamic* PhysicsCreator::createBox(PxMaterial* material, PxPhysics* physics, PxVec3 dimensions)
{
	PxBoxGeometry geometry(dimensions);
	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat::createIdentity());
	PxReal density = 1.0f;
	PxRigidDynamic* actor = PxCreateDynamic(*physics, transform, geometry, *material, density);

	//Get the box shape so we can set query and simulation filter data.
	PxShape* shape;
	actor->getShapes(&shape, 1);

	//Set the query filter data of the box so that the vehicle raycasts cannot hit the ground.
	PxFilterData qryFilterData;
	setupNonDrivableSurface(qryFilterData);
	shape->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the box so that it collides with the chassis of a vehicle but not the wheels.
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_DRIVABLE_OBSTACLE;
	simFilterData.word1 = COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST;
	shape->setSimulationFilterData(simFilterData);

	return actor;
}

PxActor* PhysicsCreator::createTriggerVolume(PxPhysics* physics)
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

PxRigidStatic* PhysicsCreator::createDrivablePlane(PxMaterial* material, PxPhysics* physics)
{
	//Add a plane to the scene.
	PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0,1,0,0), *material);

	//Get the plane shape so we can set query and simulation filter data.
	PxShape* shape;
	groundPlane->getShapes(&shape, 1);

	//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
	PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	shape->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_GROUND;
	simFilterData.word1 = COLLISION_FLAG_GROUND_AGAINST;
	shape->setSimulationFilterData(simFilterData);

	return groundPlane;
}

PxConvexMesh* PhysicsCreator::createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking)
{
	// Create descriptor for convex mesh
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count			= numVerts;
	convexDesc.points.stride		= sizeof(PxVec3);
	convexDesc.points.data			= verts;
	convexDesc.flags				= PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

	PxConvexMesh* convexMesh = NULL;
	PxDefaultMemoryOutputStream buf;
	if(cooking.cookConvexMesh(convexDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		convexMesh = physics.createConvexMesh(id);
	}

	return convexMesh;
}

