#include "SimulationCallback.h"
#include "PhysicsEntity.h"

using namespace physx;

void SimulationCallback::onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs)
{

}

void SimulationCallback::onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count)
{
	for (PxU32 i = 0; i < count; i++) 
	{
		inPickUpLocation((Vehicle*)pairs[i].otherActor->userData);
	}
}

void SimulationCallback::onSleep(physx::PxActor **actors, physx::PxU32 count)
{
	for (PxU32 i = 0; i < count; i ++)
	{
		pizzaBoxSleep((PizzaBox*)actors[i]->userData);
		//actors[i]->setActorFlag(physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, false);
	}
}

void SimulationCallback::onWake(physx::PxActor **actors, physx::PxU32 count)
{

}

void SimulationCallback::onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count)
{

}