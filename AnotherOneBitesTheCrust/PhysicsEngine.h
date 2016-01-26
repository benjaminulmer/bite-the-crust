#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;

class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void simulate(unsigned int deltaTimeMs);

	/*
private:
	PxDefaultErrorCallback defaultErrorCallback;
	PxDefaultAllocator defaultAllocator;
	PxFoundation* foundation;
	PxProfileZoneManager* profileZoneManager;
	PxPhysics* physics;
	PxCooking* cooking;
	*/
};

