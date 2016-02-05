#pragma once
#include <PxPhysicsAPI.h>

class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void simulate(unsigned int deltaTimeMs);
	void fetchSimulationResults();
	
private:
	physx::PxTolerancesScale scale;
	physx::PxDefaultErrorCallback* defaultErrorCallback;
	physx::PxDefaultAllocator* defaultAllocator;
	physx::PxF32 stepSizeS;
	physx::PxU32 numWorkers;
	
	physx::PxFoundation* foundation;
	physx::PxProfileZoneManager* profileZoneManager;
	physx::PxPhysics* physics;
	physx::PxCooking* cooking;
	physx::PxDefaultCpuDispatcher* cpuDispatcher;


	physx::PxScene* scene;

	physx::PxRigidStatic* groundPlane;
	physx::PxRigidDynamic* aSphereActor; // Test object






	physx::PxF32 deltaTimeSAcc;
	

	void initSimulationData();
	void initPhysX();
	void initVehicles();

	void testScene(); // Test method
};