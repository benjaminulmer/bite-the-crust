#include "PhysicsEngine.h"
#include "Filtering.h"
#include <iostream>

using namespace physx;

PhysicsEngine::PhysicsEngine(void)
{
	defaultErrorCallback = nullptr;
	defaultAllocator = nullptr;
	foundation = nullptr;
	physics = nullptr;
	cooking = nullptr;
	cpuDispatcher = nullptr;
	scene = nullptr;
	vehicleSceneQueryData = nullptr;
	batchQuery = nullptr;
	frictionPairs = nullptr;
	testChassisMat = nullptr;
	testWheelMat = nullptr;
	groundPlane = nullptr;
	initSimulationData();
	initPhysXSDK();
	initVehicleSDK();
}

void PhysicsEngine::initSimulationData()
{
	scale = PxTolerancesScale();
	defaultErrorCallback = new PxDefaultErrorCallback();
	defaultAllocator = new PxDefaultAllocator();

	stepSizeS = 1.0f/60.0f;
	numWorkers = 8;
}

void PhysicsEngine::initPhysXSDK()
{
	// Create foundation and profile zone manager
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *defaultAllocator, *defaultErrorCallback);
	profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(foundation);

	// Create main physics object 
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, true, profileZoneManager );
	
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

void PhysicsEngine::createDynamicEntity(DynamicEntity* entity, glm::vec3 position, glm::vec3 velocity)
{
	PxMaterial* defaultMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f);
	glm::vec3 d = entity->getRenderable()->getDimensions();
	PxRigidDynamic* object = PhysicsCreator::createBox(defaultMaterial, physics, PxVec3(d.x * 0.5f, d.y * 0.5f, d.z * 0.5f));
	PxTransform startTransform(PxVec3(position.x, position.y, position.z), PxQuat(PxIdentity));
	object->setGlobalPose(startTransform);
	//entities.push_back(object);
	scene->addActor(*object);
	entity->setActor(object);
	object->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
}

void PhysicsEngine::createVehicle(Vehicle* vehicle)
{
	VehicleTuning* tuning = vehicle->getTuningStruct();

	PxMaterial* chassisMaterial;
	PxMaterial* wheelMaterial;
	chassisMaterial = physics->createMaterial(tuning->chassisStaticFriction, tuning->chassisDynamicFriction, tuning->chassisRestitution);
	wheelMaterial = physics->createMaterial(tuning->wheelStaticFriction, tuning->wheelDynamicFriction, tuning->wheelRestitution);
	tuning->chassisMaterial = chassisMaterial;
	tuning->wheelMaterial = wheelMaterial;
	PxVehicleDrive4W* testVehicle = PhysicsCreator::createVehicle4W(vehicle, physics, cooking);
	PxTransform startTransform(PxVec3(0, (tuning->chassisDims.y*0.5f + tuning->wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	PxRigidDynamic* actor = testVehicle->getRigidDynamicActor();
	actor->setGlobalPose(startTransform);
	scene->addActor(*actor);
	vehicle->setActor(actor);
	testVehicle->setToRestState();
	testVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	testVehicle->mDriveDynData.setUseAutoGears(true);
	vehicle->physicsVehicle = testVehicle;
	vehicles.push_back(testVehicle);
}

bool PhysicsEngine::simulate(unsigned int deltaTimeMs)
{
	PxF32 deltaTimeS = deltaTimeMs/1000.0f;
	deltaTimeSAcc += deltaTimeS;

	if (deltaTimeSAcc >= stepSizeS)
	{
		deltaTimeSAcc -= stepSizeS;

		// Wheel raycasts
		PxVehicleWheels** vehiclesPointer = vehicles.data();
		PxRaycastQueryResult* raycastResults = vehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = vehicleSceneQueryData->getRaycastQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(batchQuery, vehicles.size(), vehiclesPointer, raycastResultsSize, raycastResults);

		//Vehicle update.
		const PxVec3 grav = scene->getGravity();
		PxVehicleUpdates(stepSizeS, grav, *frictionPairs, vehicles.size(), vehiclesPointer, nullptr);

		scene->simulate(stepSizeS);
		return true;
	}
	return false;
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