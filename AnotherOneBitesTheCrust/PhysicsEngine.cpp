#include "PhysicsEngine.h"
#include "FilterShader.h"
#include <iostream>

using namespace physx;

PhysicsEngine::PhysicsEngine(void) {
	initSimulationData();
	initPhysX();
	initVehicles();

	testScene();
}

void PhysicsEngine::initSimulationData() {
	scale = PxTolerancesScale();
	defaultErrorCallback = new PxDefaultErrorCallback();
	defaultAllocator = new PxDefaultAllocator();

	stepSizeS = 1.0f/60.0f;
	numWorkers = 1;
}

void PhysicsEngine::initPhysX() {
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
	scene = physics->createScene(sceneDesc);
}

void PhysicsEngine::initVehicles() {
	PxInitVehicleSDK(*physics); 

	PxVehicleSetBasisVectors(PxVec3(0,1,0), PxVec3(0,0,-1)); 
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	vehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, *defaultAllocator);
	batchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *vehicleSceneQueryData, scene);

	drivingSurfaces[0] = physics->createMaterial(0.8f, 0.8f, 0.6f);
	frictionPairs = FrictionPairs::createFrictionPairs(drivingSurfaces[0]);

	groundPlane = PhysicsCreator::createDrivablePlane(drivingSurfaces[0], physics);
	scene->addActor(*groundPlane);
}

void PhysicsEngine::initVehicle(Vehicle* vehicle) {
	PxMaterial* chassisMaterial;
	PxMaterial* wheelMaterial;
	chassisMaterial = physics->createMaterial(vehicle->chassisStaticFriction, vehicle->chassisDynamicFriction, vehicle->chassisRestitution);
	wheelMaterial = physics->createMaterial(vehicle->wheelStaticFriction, vehicle->wheelDynamicFriction, vehicle->wheelRestitution);
	vehicle->chassisMaterial = chassisMaterial;
	vehicle->wheelMaterial = wheelMaterial;
	testVehicle = PhysicsCreator::createVehicle4W(vehicle, physics, cooking);
	PxTransform startTransform(PxVec3(0, (vehicle->chassisDims.y*0.5f + vehicle->wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	PxRigidDynamic* actor = testVehicle->getRigidDynamicActor();
	actor->setGlobalPose(startTransform);
	scene->addActor(*actor);
	vehicle->setActor(actor);
	testVehicle->setToRestState();
	testVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	testVehicle->mDriveDynData.setUseAutoGears(true);
	vehicle->physicsVehicle = testVehicle;
}

void PhysicsEngine::testScene() {
}

void PhysicsEngine::simulate(unsigned int deltaTimeMs, DrivingInput* playerInput) {
	PxF32 deltaTimeS = deltaTimeMs/1000.0f;
	deltaTimeSAcc += deltaTimeS;

	if (deltaTimeSAcc >= stepSizeS) {
		deltaTimeSAcc -= stepSizeS;

		//std::cout << playerInput->accel << std::endl;

		PxVehicleWheels* vehicles[1] = {testVehicle};
		PxRaycastQueryResult* raycastResults = vehicleSceneQueryData->getRaycastQueryResultBuffer(0);
		const PxU32 raycastResultsSize = vehicleSceneQueryData->getRaycastQueryResultBufferSize();
		PxVehicleSuspensionRaycasts(batchQuery, 1, vehicles, raycastResultsSize, raycastResults);

		//Vehicle update.
		const PxVec3 grav = scene->getGravity();
		PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
		PxVehicleWheelQueryResult vehicleQueryResults[1] = {{wheelQueryResults, testVehicle->mWheelsSimData.getNbWheels()}};
		PxVehicleUpdates(stepSizeS, grav, *frictionPairs, 1, vehicles, vehicleQueryResults);

		scene->simulate(stepSizeS);
	}
	//std::cout << testVehicle->getRigidDynamicActor()->getGlobalPose().p.x << " : " << testVehicle->getRigidDynamicActor()->getGlobalPose().p.y << " : " << testVehicle->getRigidDynamicActor()->getGlobalPose().p.z << std::endl;
}

void PhysicsEngine::fetchSimulationResults() {
	scene->fetchResults(true);
}

double PhysicsEngine::getPosX(){
	return testVehicle->getRigidDynamicActor()->getGlobalPose().p.x;
}

double PhysicsEngine::getPosY(){
	return testVehicle->getRigidDynamicActor()->getGlobalPose().p.y;
}

double PhysicsEngine::getPosZ(){
	return testVehicle->getRigidDynamicActor()->getGlobalPose().p.z;
}

PhysicsEngine::~PhysicsEngine(void) {	
	PxCloseVehicleSDK();
	scene->release();
	cooking->release();
	cpuDispatcher->release();
	physics->release();
	profileZoneManager->release();
	foundation->release();
}