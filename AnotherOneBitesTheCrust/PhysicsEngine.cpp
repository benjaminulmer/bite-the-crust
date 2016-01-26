#include "PhysicsEngine.h"

using namespace physx;

PhysicsEngine::PhysicsEngine(void)
{
	bool recordMemoryAllocations = true;
	PxTolerancesScale scale = PxTolerancesScale();

	// Create foundation and profile zone manager
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocator, defaultErrorCallback);
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
}

void PhysicsEngine::simulate(unsigned int deltaTimeMs) {}

PhysicsEngine::~PhysicsEngine(void)
{
}
