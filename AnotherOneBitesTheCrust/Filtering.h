#pragma once
#include <PxPhysicsAPI.h>

enum
{
	COLLISION_FLAG_GROUND			=	1 << 0,
	COLLISION_FLAG_WHEEL			=	1 << 1,
	COLLISION_FLAG_CHASSIS			=	1 << 2,
	COLLISION_FLAG_OBSTACLE			=	1 << 3, // Will colide with rigid body wheels
	COLLISION_FLAG_DRIVABLE_OBSTACLE=	1 << 4, // Will not colide with rigid body wheels

	COLLISION_FLAG_GROUND_AGAINST	=															COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_WHEEL_AGAINST	=									COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
	COLLISION_FLAG_CHASSIS_AGAINST	=			COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_OBSTACLE_AGAINST	=			COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST=	COLLISION_FLAG_GROUND 						 |	COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
};

physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
								  physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
								  physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

class FilterCallback
	: public physx::PxSimulationFilterCallback
{
	physx::PxFilterFlags pairFound(physx::PxU32 pairID, physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
								   const physx::PxActor *a0, const physx::PxShape *s0, physx::PxFilterObjectAttributes attributes1,
								   physx::PxFilterData filterData1, const physx::PxActor *a1, const physx::PxShape *s1, physx::PxPairFlags &pairFlags);

	void pairLost(physx::PxU32 pairID, physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		          physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, bool objectDeleted);

	bool statusChange(physx::PxU32 &pairID, physx::PxPairFlags &pairFlags, physx::PxFilterFlags &filterFlags);
};