#include "Filtering.h"
#include "Vehicle.h"
#include <iostream>

using namespace physx;

FilterFlag stringToFlag(std::string string) {
	if (string == "GROUND")
		return FilterFlag::GROUND;
	else if (string == "WHEEL")
		return FilterFlag::WHEEL;
	else if (string == "CHASSIS")
		return FilterFlag::CHASSIS;
	else if (string == "OBSTACLE")
		return FilterFlag::OBSTACLE;
	else if (string == "DRIVABLE_OBSTACLE")
		return FilterFlag::DRIVABLE_OBSTACLE;
	else if (string == "GROUND_AGAINST")
		return FilterFlag::GROUND_AGAINST;
	else if (string == "WHEEL_AGAINST")
		return FilterFlag::WHEEL_AGAINST;
	else if (string == "CHASSIS_AGAINST")
		return FilterFlag::CHASSIS_AGAINST;
	else if (string == "OBSTACLE_AGAINST")
		return FilterFlag::OBSTACLE_AGAINST;
	else if (string == "DRIVABLE_OBSTACLE_AGAINST")
		return FilterFlag::DRIVABLE_OBSTACLE_AGAINST;
	return FilterFlag::DEFAULT;
}

PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
						   PxFilterObjectAttributes attributes1, PxFilterData filterData1,
						   PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	if (filterData0.word0 == (PxU32)FilterFlag::CHASSIS || filterData1.word0 == (PxU32)FilterFlag::CHASSIS)
	{
		if ((PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}
		else 
		{
			pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;
			return PxFilterFlags();
		}
	}

	if(((filterData0.word0 & filterData1.word1) == 0) && ((filterData1.word0 & filterData0.word1) == 0))
	{
		return PxFilterFlag::eSUPPRESS;
	}
	
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	return PxFilterFlags();
}

PxFilterFlags FilterCallback::pairFound(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0,
									    const PxActor *a0, const PxShape *s0, PxFilterObjectAttributes attributes1,
									    PxFilterData filterData1, const PxActor *a1, const PxShape *s1, PxPairFlags &pairFlags)
{
	return PxFilterFlags();
}

void FilterCallback::pairLost(physx::PxU32 pairID, physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		                      physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, bool objectDeleted)
{
	return;
}

bool FilterCallback::statusChange(physx::PxU32 &pairID, physx::PxPairFlags &pairFlags, physx::PxFilterFlags &filterFlags)
{
	return false;
}