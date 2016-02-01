#include "PhysicsEngine.h"

using namespace physx;

PhysicsEngine::PhysicsEngine(void) {
	// Initializaiton values
	bool recordMemoryAllocations = true;
	PxTolerancesScale scale = PxTolerancesScale();
	float staticFriction = 0.9;
	float dynamicFriction = 0.9;
	float restitution = 0.5;
	// END

	defaultErrorCallback = new PxDefaultErrorCallback();
	defaultAllocator = new PxDefaultAllocator();

	// Create foundation and profile zone manager
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, *defaultAllocator, *defaultErrorCallback);
	profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(foundation);

	// Create main physics object 
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

	// Create scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	scene = physics->createScene(sceneDesc);

	// Make material and add plane and sphere to scene
	planeMaterial = physics->createMaterial(staticFriction, dynamicFriction, restitution);
	PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(PxVec3(0,1,0), PxVec3(0,0,0)), *planeMaterial);
	//scene->addActor(*plane);

	//PxRigidDynamic* aSphereActor = PxCreateDynamic(*physics, PxTransform(PxVec3(0,1,0)), PxSphereGeometry(1), *planeMaterial, PxReal(0.5));
	//aSphereActor->setLinearVelocity(PxVec3(1,0,0));
}



void PhysicsEngine::simulate(unsigned int deltaTimeMs) {
	//scene->simulate(deltaTimeMs);
	//scene->fetchResults(true);
}

PhysicsEngine::~PhysicsEngine(void) {
	physics->release();
	cooking->release();
	profileZoneManager->release();
	foundation->release();
}
