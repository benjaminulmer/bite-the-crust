#pragma once
#include <PxPhysicsAPI.h>

class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void simulate(unsigned int deltaTimeMs);
	
private:
	//simulationData simData;
	//vehicleData vehicleData;

	// Initialization parameters
	physx::PxTolerancesScale scale;
	

	// END initialization parameters

	physx::PxDefaultErrorCallback* defaultErrorCallback;
	physx::PxDefaultAllocator* defaultAllocator;
	

	physx::PxFoundation* foundation;
	physx::PxProfileZoneManager* profileZoneManager;
	physx::PxPhysics* physics;
	physx::PxCooking* cooking;

	physx::PxScene* scene;
	physx::PxMaterial* planeMaterial;

	physx::PxRigidDynamic* aSphereActor;

	void initSimulationData();
};