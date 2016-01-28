#pragma once
#include <PxPhysicsAPI.h>

class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void simulate(unsigned int deltaTimeMs);
	
private:
	physx::PxDefaultErrorCallback defaultErrorCallback;
	physx::PxDefaultAllocator defaultAllocator;
	physx::PxFoundation* foundation;
	physx::PxProfileZoneManager* profileZoneManager;
	physx::PxPhysics* physics;
	physx::PxCooking* cooking;
};