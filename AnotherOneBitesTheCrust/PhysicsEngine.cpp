#include "PhysicsEngine.h"
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

	stepSizeS = 1.0/60.0;
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
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);
}

void PhysicsEngine::initVehicles() {
	PxInitVehicleSDK(*physics); 

	PxVehicleSetBasisVectors(PxVec3(0,1,0), PxVec3(0,0,1)); 
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	//VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, defaultAllocator);
}

void PhysicsEngine::testScene() {
	/// Create sample scene
	PxMaterial* planeMaterial = physics->createMaterial(PxReal(0.9), PxReal(0.9), PxReal(0.5));

	// Create plane and sphere and add them too scene
	groundPlane = PxCreatePlane(*physics, PxPlane(PxVec3(0,0,0), PxVec3(0,1,0)), *planeMaterial);
	scene->addActor(*groundPlane);

	aSphereActor = PxCreateDynamic(*physics, PxTransform(PxVec3(10,1,10)), PxSphereGeometry(1), *planeMaterial, PxReal(0.5));
	aSphereActor->setLinearVelocity(PxVec3(1,0,0));
	scene->addActor(*aSphereActor);
}

void PhysicsEngine::simulate(unsigned int deltaTimeMs) {
	PxF32 deltaTimeS = deltaTimeMs / 1000.0;
	deltaTimeSAcc += deltaTimeS;

	if (deltaTimeSAcc >= stepSizeS) {
		deltaTimeSAcc -= stepSizeS;
		scene->simulate(stepSizeS);
	}
	std::cout << aSphereActor->getGlobalPose().p.x << " : " << aSphereActor->getGlobalPose().p.y << " : " << aSphereActor->getGlobalPose().p.z << std::endl;
}

void PhysicsEngine::fetchSimulationResults() {
	scene->fetchResults(true);
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