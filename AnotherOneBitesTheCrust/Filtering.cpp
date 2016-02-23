#include "Filtering.h"
#include <iostream>

using namespace physx;

PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
						   PxFilterObjectAttributes attributes1, PxFilterData filterData1,
						   PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	if(((filterData0.word0 & filterData1.word1) == 0) && ((filterData1.word0 & filterData0.word1) == 0))
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	//return PxFilterFlags();
	return PxFilterFlag::eCALLBACK;
}

PxFilterFlags FilterCallback::pairFound(PxU32 pairID, PxFilterObjectAttributes attributes0, PxFilterData filterData0,
									    const PxActor *a0, const PxShape *s0, PxFilterObjectAttributes attributes1,
									    PxFilterData filterData1, const PxActor *a1, const PxShape *s1, PxPairFlags &pairFlags)
{
	std::cout << "callback" << std::endl;
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