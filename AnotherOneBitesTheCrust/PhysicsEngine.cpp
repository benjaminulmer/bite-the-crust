#include "PhysicsEngine.h"
#include "Filtering.h"
#include "PhysicsCreator.h"
#include "VehicleCreator.h"

#include <iostream>

using namespace physx;

PhysicsEngine::PhysicsEngine(void)
{
	initSimulationData();
	initPhysXSDK();
	initVehicleSDK();
}

void PhysicsEngine::initSimulationData()
{
	scale = PxTolerancesScale();
	defaultErrorCallback = new PxDefaultErrorCallback();
	defaultAllocator = new PxDefaultAllocator();

	numWorkers = 8;
}

void PhysicsEngine::initPhysXSDK()
{
	// Create foundation and profile zone manager
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *defaultAllocator, *defaultErrorCallback);
	profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(foundation);

	// Create main physics object 
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, true, profileZoneManager);
	
	// Create cooking object
	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams =
		PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES |
		PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES |
		PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, params); 

	// Create scene
	cpuDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = cpuDispatcher;
	sceneDesc.filterShader = FilterShader;
	FilterCallback* filterCallback = new FilterCallback();
	sceneDesc.filterCallback = filterCallback;
	scene = physics->createScene(sceneDesc);
}

void PhysicsEngine::initVehicleSDK()
{
	PxInitVehicleSDK(*physics); 

	PxVehicleSetBasisVectors(PxVec3(0,1,0), PxVec3(0,0,1)); 
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	vehicleSceneQueryData = VehicleSceneQueryData::allocate(MAX_VEHICLES, PX_MAX_NB_WHEELS, MAX_VEHICLES, *defaultAllocator);
	batchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *vehicleSceneQueryData, scene);

	drivingSurfaces[0] = physics->createMaterial(0.8f, 0.8f, 0.6f);
	frictionPairs = FrictionPairs::createFrictionPairs(drivingSurfaces[0]);

	groundPlane = PhysicsCreator::createDrivablePlane(drivingSurfaces[0], physics);
	scene->addActor(*groundPlane);
}

void PhysicsEngine::createEntity(PhysicsEntity* entity, PhysicsEntityInfo* info, PxTransform transform)
{
	PxRigidActor* actor;
	if (info->type == PhysicsType::DYNAMIC) 
	{
		DynamicInfo* dInfo = info->dynamicInfo;
		PxRigidDynamic* dynamicActor = physics->createRigidDynamic(transform);
		dynamicActor->setMass(dInfo->mass);
		dynamicActor->setCMassLocalPose(dInfo->comOffset);
		dynamicActor->setLinearDamping(dInfo->linearDamping);
		dynamicActor->setAngularDamping(dInfo->angularDamping);
		dynamicActor->setMaxAngularVelocity(dInfo->maxAngularVelocity);

		actor = dynamicActor;
	}
	else if (info->type == PhysicsType::STATIC)
	{
		PxRigidStatic* staticActor = physics->createRigidStatic(transform);
		actor = staticActor;
	}

	for (auto sInfo : info->shapeInfo)
	{
		PxMaterial* material = physics->createMaterial(sInfo->dynamicFriction, sInfo->staticFriction, sInfo->restitution);
		PxGeometry* geometry;
		if (sInfo->geometry == Geometry::SPHERE)
		{
			SphereInfo* sphInfo = (SphereInfo*)sInfo;
			geometry = new PxSphereGeometry(sphInfo->radius);
		}
		else if (sInfo->geometry == Geometry::BOX)
		{
			BoxInfo* boxInfo = (BoxInfo*)sInfo;		
			geometry = new PxBoxGeometry(boxInfo->halfX, boxInfo->halfY, boxInfo->halfZ);
		}
		else if (sInfo->geometry == Geometry::CAPSULE)
		{
			CapsuleInfo* capInfo = (CapsuleInfo*)sInfo;
			geometry = new PxCapsuleGeometry(capInfo->raidus, capInfo->halfHeight);
		}
		else if (sInfo->geometry == Geometry::CONVEX_MESH)
		{
			ConvexMeshInfo* cmInfo = (ConvexMeshInfo*)sInfo;
			// other stuff here, meshes are more complicated.
			//PhysicsCreator::createConvexMesh(verts, numVerts, physics, cooking);
		}
		else 
		{
			// default? should probably at least specify a geometry type and dimensions
		}
		PxShape* shape = actor->createShape(*geometry, *material);
		shape->setLocalPose(sInfo->transform);

		PxFilterData qryFilterData;
		if (sInfo->isDrivable)
		{
			qryFilterData.word3 = (PxU32)Surface::DRIVABLE;
		}
		else
		{
			qryFilterData.word3 = (PxU32)Surface::UNDRIVABLE;
		}
		shape->setQueryFilterData(qryFilterData);

		PxFilterData simFilterData;
		simFilterData.word0 = (PxU32)sInfo->filterFlag0;
		simFilterData.word1 = (PxU32)sInfo->filterFlag1;
		shape->setSimulationFilterData(simFilterData);

		//PxRigidBodyExt::updateMassAndInertia(actor) // TODO this thing otherwise mass wont work 
	}

	scene->addActor(*actor);
	entity->setActor(actor);
}

void PhysicsEngine::createTrigger()
{
	PxActor* object = PhysicsCreator::createTriggerVolume(physics);
	scene->addActor(*object);
}

void PhysicsEngine::createVehicle(Vehicle* vehicle, PxTransform transform)
{
	VehicleTuning* tuning = &vehicle->tuning;

	PxMaterial* chassisMaterial = physics->createMaterial(tuning->chassisStaticFriction, tuning->chassisDynamicFriction, tuning->chassisRestitution);
	PxMaterial* wheelMaterial = physics->createMaterial(tuning->wheelStaticFriction, tuning->wheelDynamicFriction, tuning->wheelRestitution);
	tuning->chassisMaterial = chassisMaterial;
	tuning->wheelMaterial = wheelMaterial;

	PxVehicleDrive4W* physVehicle = VehicleCreator::createVehicle4W(vehicle, physics, cooking);
	//PxTransform startTransform(PxVec3(0, (tuning->chassisDims.y*0.5f + tuning->wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	PxRigidDynamic* actor = physVehicle->getRigidDynamicActor();

	actor->setGlobalPose(transform);
	scene->addActor(*actor);

	physVehicle->setToRestState();
	physVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	physVehicle->mDriveDynData.setUseAutoGears(true);
	
	vehicle->setActor(actor);
	vehicle->physicsVehicle = physVehicle;
	actor->userData = vehicle;

	vehicles.push_back(physVehicle);
}

void PhysicsEngine::simulate(unsigned int deltaTimeMs)
{
	PxF32 deltaTimeS = deltaTimeMs/1000.0f;

	// Wheel raycasts
	PxVehicleWheels** vehiclesPointer = vehicles.data();
	PxRaycastQueryResult* raycastResults = vehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = vehicleSceneQueryData->getRaycastQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(batchQuery, vehicles.size(), vehiclesPointer, raycastResultsSize, raycastResults);

	//Vehicle update.
	const PxVec3 grav = scene->getGravity();
	PxVehicleUpdates(deltaTimeS, grav, *frictionPairs, vehicles.size(), vehiclesPointer, nullptr);

	scene->simulate(deltaTimeS);
}

void PhysicsEngine::fetchSimulationResults()
{
	scene->fetchResults(true);
}

PhysicsEngine::~PhysicsEngine(void)
{	
	PxCloseVehicleSDK();
	scene->release();
	cooking->release();
	cpuDispatcher->release();
	physics->release();
	profileZoneManager->release();
	foundation->release();
}