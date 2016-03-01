#pragma once
#include <PxPhysicsAPI.h>

class SimulationCallback
	: public physx::PxSimulationEventCallback
{
	void onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs);

	void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count);

	void onSleep(physx::PxActor **actors, physx::PxU32 count);

	void onWake(physx::PxActor **actors, physx::PxU32 count);

	void onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count);
};

