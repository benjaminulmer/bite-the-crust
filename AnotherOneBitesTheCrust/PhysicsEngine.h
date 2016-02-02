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

	physx::PxDefaultErrorCallback* defaultErrorCallback;
	physx::PxDefaultAllocator* defaultAllocator;
	physx::PxTolerancesScale scale;

	physx::PxFoundation* foundation;
	physx::PxProfileZoneManager* profileZoneManager;
	physx::PxPhysics* physics;
	physx::PxCooking* cooking;

	physx::PxScene* scene;
	physx::PxMaterial* planeMaterial;

	physx::PxRigidDynamic* aSphereActor;
};