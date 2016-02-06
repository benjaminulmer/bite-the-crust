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

	PxVehicleSetBasisVectors(PxVec3(0,1,0), PxVec3(0,0,1)); 
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	vehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, *defaultAllocator);
	batchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *vehicleSceneQueryData, scene);

	drivingSurfaces[0] = physics->createMaterial(0.5f, 0.5f, 0.6f);
	frictionPairs = FrictionPairs::createFrictionPairs(drivingSurfaces[0]);

	groundPlane = PhysicsCreator::createDrivablePlane(drivingSurfaces[0], physics);
	scene->addActor(*groundPlane);

	testChassisMat = physics->createMaterial(0.5f, 0.5f, 0.6f);
	testWheelMat = physics->createMaterial(0.5f, 0.5f, 0.6f);
	VehicleDesc vehicleDesc = initVehicleDesc();
	testVehicle = PhysicsCreator::createVehicle4W(vehicleDesc, physics, cooking);
	PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
	testVehicle->getRigidDynamicActor()->setGlobalPose(startTransform);
	scene->addActor(*testVehicle->getRigidDynamicActor());

	testVehicle->setToRestState();
	testVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	testVehicle->mDriveDynData.setUseAutoGears(true);
}

void PhysicsEngine::testScene() {
}

void PhysicsEngine::simulate(unsigned int deltaTimeMs, DrivingInput* playerInput) {
	PxF32 deltaTimeS = deltaTimeMs/1000.0f;
	deltaTimeSAcc += deltaTimeS;

	if (deltaTimeSAcc >= stepSizeS) {
		deltaTimeSAcc -= stepSizeS;

		testVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, playerInput->accel);
		testVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, playerInput->brake);
		testVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, playerInput->leftSteer);
		testVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, playerInput->rightSteer);

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
	std::cout << testVehicle->getRigidDynamicActor()->getGlobalPose().p.x << " : " << testVehicle->getRigidDynamicActor()->getGlobalPose().p.y << " : " << testVehicle->getRigidDynamicActor()->getGlobalPose().p.z << std::endl;
}

void PhysicsEngine::fetchSimulationResults() {
	scene->fetchResults(true);
}

VehicleDesc PhysicsEngine::initVehicleDesc()
{
	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisDims(2.5f,2.0f,5.0f);
	const PxVec3 chassisMOI
		((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass/12.0f,
		 (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass/12.0f,
		 (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass/12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelRadius = 0.5f;
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
	const PxU32 nbWheels = 6;

	VehicleDesc vehicleDesc;
	vehicleDesc.chassisMass = chassisMass;
	vehicleDesc.chassisDims = chassisDims;
	vehicleDesc.chassisMOI = chassisMOI;
	vehicleDesc.chassisCMOffset = chassisCMOffset;
	vehicleDesc.chassisMaterial = testChassisMat;
	vehicleDesc.wheelMass = wheelMass;
	vehicleDesc.wheelRadius = wheelRadius;
	vehicleDesc.wheelWidth = wheelWidth;
	vehicleDesc.wheelMOI = wheelMOI;
	vehicleDesc.numWheels = nbWheels;
	vehicleDesc.wheelMaterial = testWheelMat;
	return vehicleDesc;
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