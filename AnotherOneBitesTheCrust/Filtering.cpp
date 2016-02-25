#include "Filtering.h"
#include "Vehicle.h"
#include <iostream>

using namespace physx;

PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
						   PxFilterObjectAttributes attributes1, PxFilterData filterData1,
						   PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eCALLBACK;
    }

	if(((filterData0.word0 & filterData1.word1) == 0) && ((filterData1.word0 & filterData0.word1) == 0))
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	return PxFilterFlags();
}

PxFilterFlags FilterCallback::pairFound(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0,
									    const PxActor *a0, const PxShape *s0, PxFilterObjectAttributes attributes1,
									    PxFilterData filterData1, const PxActor *a1, const PxShape *s1, PxPairFlags &pairFlags)
{
	std::cout << "callback" << std::endl;
	Vehicle* vehicle = (Vehicle*)(a0->userData);
	vehicle->test();

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
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