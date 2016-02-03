#include "PhysicsEngine.h"
#include <iostream>

using namespace physx;

PhysicsEngine::PhysicsEngine(void) {
	initSimulationData();

	// Create foundation and profile zone manager
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *defaultAllocator, *defaultErrorCallback);
	profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(foundation);

	// Create main physics object 
	bool recordMemoryAllocations = true;
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, scale, recordMemoryAllocations, profileZoneManager );
	
	// Create cooking
	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams =
		PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES |
		PxMeshPreprocessingFlag::eREMOVE_UNREFERENCED_VERTICES |
		PxMeshPreprocessingFlag::eREMOVE_DUPLICATED_TRIANGLES);
	cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, params); 

	// Initialize physx extensions
	PxInitExtensions(*physics);

	// END ACTUAL INIT
	// END ACTUAL INIT
	// END ACTUAL INIT

	// Create scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	scene = physics->createScene(sceneDesc);

	// Make material and add plane and sphere to scene
	PxVec3 norm = PxVec3(1,1,0);
	norm.normalize();
	planeMaterial = physics->createMaterial(PxReal(0.9), PxReal(0.9), PxReal(0.5));
	PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(PxVec3(0,0,0), norm), *planeMaterial);
	scene->addActor(*plane);

	aSphereActor = PxCreateDynamic(*physics, PxTransform(PxVec3(0,1,0)), PxSphereGeometry(1), *planeMaterial, PxReal(0.5));
	aSphereActor->setLinearVelocity(PxVec3(1,0,0));
	scene->addActor(*aSphereActor);
}

void PhysicsEngine::initSimulationData() {
	scale = PxTolerancesScale();
	defaultErrorCallback = new PxDefaultErrorCallback();
	defaultAllocator = new PxDefaultAllocator();
}

void PhysicsEngine::simulate(unsigned int deltaTimeMs) {
	scene->simulate(PxReal(deltaTimeMs/1000.0));
	scene->fetchResults(true);

	std::cout << aSphereActor->getGlobalPose().p.x << " : " << aSphereActor->getGlobalPose().p.y << " : " << aSphereActor->getGlobalPose().p.z << std::endl;
}

PhysicsEngine::~PhysicsEngine(void) {
	physics->release();
	cooking->release();
	profileZoneManager->release();
	foundation->release();
}
