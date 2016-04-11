#include "SimulationCallback.h"
#include "PhysicsEntity.h"

using namespace physx;

void SimulationCallback::onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs)
{
	PhysicsEntity* entity1 = (PhysicsEntity*)pairHeader.actors[0]->userData;
	PhysicsEntity* entity2 = (PhysicsEntity*)pairHeader.actors[1]->userData;

	if (entity1->type == EntityType::VEHICLE && entity2->type == EntityType::VEHICLE)
	{
		Vehicle* veh1 = (Vehicle*)entity1;
		Vehicle* veh2 = (Vehicle*)entity2;

		PxVec3 force(veh1->getActor()->getGlobalPose().p - veh2->getActor()->getGlobalPose().p);
		toApplyForce.push_back(std::pair<PxRigidDynamic*, PxVec3>(veh1->getRigidDynamic(), 5 * force * veh2->getRigidDynamic()->getLinearVelocity().magnitude()));
		toApplyForce.push_back(std::pair<PxRigidDynamic*, PxVec3>(veh2->getRigidDynamic(), -5 * force * veh1->getRigidDynamic()->getLinearVelocity().magnitude()));

		if (veh1->getRigidDynamic()->getLinearVelocity().magnitude() > 10)
		{
			collision(veh1);
		}
	}
	else if (entity1->type == EntityType::VEHICLE) 
	{
		Vehicle* veh = (Vehicle*)entity1;
		if (veh->getRigidDynamic()->getLinearVelocity().magnitude() > 10)
		{
			collision(veh);
		}
	}
	else if (entity2->type == EntityType::VEHICLE) 
	{
		Vehicle* veh = (Vehicle*)entity2;
		if (veh->getRigidDynamic()->getLinearVelocity().magnitude() > 10)
		{
			collision(veh);
		}
	}
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
		toEndSleepNotifies.push_back(actors[i]);
	}
}

void SimulationCallback::onWake(physx::PxActor **actors, physx::PxU32 count)
{

}

void SimulationCallback::onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count)
{

}

void SimulationCallback::finishedFetch()
{
	for (PxActor* actor : toEndSleepNotifies) 
	{
		actor->setActorFlag(physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, false);
	}
	toEndSleepNotifies.clear();

	for (std::pair<PxRigidDynamic*, PxVec3> pair : toApplyForce) 
	{
		pair.first->addForce(pair.second, PxForceMode::eACCELERATION);
	}
	toApplyForce.clear();
}