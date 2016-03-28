#pragma once
#include <PxPhysicsAPI.h>
#include <sigslot.h>
#include "Vehicle.h"
#include "PizzaBox.h"

class SimulationCallback
	: public physx::PxSimulationEventCallback
{
public:
	void onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs);

	void onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count);

	void onSleep(physx::PxActor **actors, physx::PxU32 count);

	void onWake(physx::PxActor **actors, physx::PxU32 count);

	void onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count);

	sigslot::signal1<PizzaBox*> pizzaBoxSleep;
	sigslot::signal1<Vehicle*> inPickUpLocation;
	sigslot::signal1<Vehicle*> collision;
};