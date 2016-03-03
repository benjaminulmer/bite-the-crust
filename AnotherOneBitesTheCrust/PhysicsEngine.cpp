#include "PhysicsEngine.h"
#include "Filtering.h"
#include <iostream>

using namespace physx;

PhysicsEngine::PhysicsEngine(void)
{
	initSimulationData();
	initPhysXSDK();

	helper = new PhysicsHelper(physics, cooking);
	vehCreator = new VehicleCreator(physics, cooking, helper);

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

	simulationCallback = new SimulationCallback();
	sceneDesc.simulationEventCallback = simulationCallback;

	scene = physics->createScene(sceneDesc);
}

void PhysicsEngine::initVehicleSDK()
{
	PxInitVehicleSDK(*physics); 

	PxVehicleSetBasisVectors(PxVec3(0,1,0), PxVec3(0,0,1)); 
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	// Allocate buffers and stuff? for raycasts
	vehicleSceneQueryData = VehicleSceneQueryData::allocate(MAX_VEHICLES, PX_MAX_NB_WHEELS, MAX_VEHICLES, *defaultAllocator);
	batchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *vehicleSceneQueryData, scene);

	drivingSurfaces[0] = physics->createMaterial(0.8f, 0.8f, 0.6f);
	frictionPairs = FrictionPairs::createFrictionPairs(drivingSurfaces[0]);

	// Create a plane and add it to the scene
	groundPlane = helper->createDrivablePlane(drivingSurfaces[0]);
	scene->addActor(*groundPlane);
}

// Creates an physics entity from an entity info structure and a starting transform
void PhysicsEngine::createEntity(PhysicsEntity* entity, PhysicsEntityInfo* info, PxTransform transform)
{
	transform.p.y += info->yPosOffset;
	// Set static/dynamic info for actor depending on its type
	PxRigidActor* actor;
	if (info->type == PhysicsType::DYNAMIC) 
	{
		DynamicInfo* dInfo = info->dynamicInfo;
		PxRigidDynamic* dynamicActor = physics->createRigidDynamic(transform);
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

	// All shapes in actor
	for (auto sInfo : info->shapeInfo)
	{
		// Create material and geometry for shape and add it to actor
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
			geometry = new PxCapsuleGeometry(capInfo->radius, capInfo->halfHeight);
		}
		else if (sInfo->geometry == Geometry::CONVEX_MESH)
		{
			ConvexMeshInfo* cmInfo = (ConvexMeshInfo*)sInfo;
			std::vector<PxVec3> verts = helper->glmVertsToPhysXVerts(cmInfo->verts);

			PxConvexMesh* mesh = helper->createConvexMesh(verts.data(), verts.size());
			geometry = new PxConvexMeshGeometry(mesh);
		}
		// Not working until index drawing is set up
		else if (sInfo->geometry == Geometry::TRIANGLE_MESH)
		{
			TriangleMeshInfo* tmInfo = (TriangleMeshInfo*)sInfo;
			std::vector<PxVec3> verts = helper->glmVertsToPhysXVerts(tmInfo->verts);

			PxTriangleMesh* mesh = helper->createTriangleMesh(verts.data(), verts.size(), tmInfo->faces.data(), tmInfo->faces.size());
			geometry = new PxTriangleMeshGeometry(mesh);

			std::cout << "verts: " << tmInfo->verts.size() << std::endl;
			std::cout << "faces: " << tmInfo->faces.size() << std::endl;
		}
		PxShape* shape = actor->createShape(*geometry, *material); // TODO support shape flags
		shape->setLocalPose(sInfo->transform);

		// Set up querry filter data for shape
		PxFilterData qryFilterData;
		(sInfo->isDrivable) ? qryFilterData.word3 = (PxU32)Surface::DRIVABLE : qryFilterData.word3 = (PxU32)Surface::UNDRIVABLE;
		shape->setQueryFilterData(qryFilterData);

		// Set up simulation filter data for shape
		PxFilterData simFilterData;
		simFilterData.word0 = (PxU32)sInfo->filterFlag0;
		simFilterData.word1 = (PxU32)sInfo->filterFlag1;
		simFilterData.word2 = (PxU32)sInfo->filterFlag2;
		simFilterData.word3 = (PxU32)sInfo->filterFlag3;
		shape->setSimulationFilterData(simFilterData);

		if (info->type == PhysicsType::DYNAMIC) 
		{
			DynamicInfo* dInfo = info->dynamicInfo;
			PxRigidBodyExt::updateMassAndInertia(*(PxRigidBody*)actor, dInfo->density, &dInfo->cmOffset);

			PxRigidBody* body = (PxRigidBody*)actor;
		}
	}

	// Add actor to scene, set actor for entity, and set user data for actor. Creates one to one between entities and phyX
	scene->addActor(*actor);
	entity->setActor(actor);
	actor->userData = entity;
}

void PhysicsEngine::createTrigger()
{
	PxActor* object = helper->createTriggerVolume();
	scene->addActor(*object);
}

void PhysicsEngine::createVehicle(Vehicle* vehicle, PxTransform transform)
{
	VehicleTuning* tuning = &vehicle->tuning;
	tuningFromUserTuning(vehicle);
	

	PxVehicleDrive4W* physVehicle = vehCreator->createVehicle4W(vehicle);
	//PxTransform startTransform(PxVec3(0, (tuning->chassisDims.y*0.5f + tuning->wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	PxRigidDynamic* actor = physVehicle->getRigidDynamicActor();

	actor->setGlobalPose(transform);
	scene->addActor(*actor);

	physVehicle->setToRestState();
	physVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	physVehicle->mDriveDynData.setUseAutoGears(true);
	
	vehicle->setActor(actor);
	vehicle->setPhysicsVehicle(physVehicle);
	actor->userData = vehicle;

	vehicles.push_back(physVehicle);
}

void PhysicsEngine::tuningFromUserTuning(Vehicle* vehicle)
{
	VehicleTuning* tuning = &vehicle->tuning;

	//Set up the default chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	tuning->chassisDims = PxVec3(2.5f,2.0f,5.0f);
	tuning->chassisMOI = PxVec3
		((tuning->chassisDims.y * tuning->chassisDims.y + tuning->chassisDims.z * tuning->chassisDims.z) * tuning->chassisMass/12.0f * tuning->chassisMOIscalar.x,
		 (tuning->chassisDims.x * tuning->chassisDims.x + tuning->chassisDims.z * tuning->chassisDims.z) * tuning->chassisMass/12.0f * tuning->chassisMOIscalar.y,
		 (tuning->chassisDims.x * tuning->chassisDims.x + tuning->chassisDims.y * tuning->chassisDims.y) * tuning->chassisMass/12.0f * tuning->chassisMOIscalar.z);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	tuning->wheelRadius = 0.5f;
	tuning->wheelWidth = 0.4f;
	tuning->wheelMOI = 0.5f * tuning->wheelMass * tuning->wheelRadius * tuning->wheelRadius * tuning->wheelMOIscalar;
	tuning->numWheels = 4;

	tuning->maxSteer = PxPi * tuning->maxSteerDegrees/180.0f;

	tuning->chassisMaterial = physics->createMaterial(tuning->chassisStaticFriction, tuning->chassisDynamicFriction, tuning->chassisRestitution);
	tuning->wheelMaterial = physics->createMaterial(tuning->wheelStaticFriction, tuning->wheelDynamicFriction, tuning->wheelRestitution);
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