#pragma once
#include <PxPhysicsAPI.h>
#include <string>

enum class FilterFlag
{
	GROUND            = 1 << 0,
	WHEEL             = 1 << 1,
	CHASSIS           = 1 << 2,
	OBSTACLE          = 1 << 3, // Will colide with rigid body wheels
	DRIVABLE_OBSTACLE = 1 << 4, // Will not colide with rigid body wheels

	GROUND_AGAINST            =                  CHASSIS | OBSTACLE | DRIVABLE_OBSTACLE,
	WHEEL_AGAINST             =	         WHEEL | CHASSIS | OBSTACLE,
	CHASSIS_AGAINST	          = GROUND | WHEEL | CHASSIS | OBSTACLE | DRIVABLE_OBSTACLE,
	OBSTACLE_AGAINST          = GROUND | WHEEL | CHASSIS | OBSTACLE | DRIVABLE_OBSTACLE,
	DRIVABLE_OBSTACLE_AGAINST =	GROUND         | CHASSIS | OBSTACLE | DRIVABLE_OBSTACLE,

	DEFAULT
};

FilterFlag stringToFlag(std::string string);

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