#include "FilterShader.h"

using namespace physx;

PxFilterFlags FilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
						   PxFilterObjectAttributes attributes1, PxFilterData filterData1,
						   PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	if( (0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)) )
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	return PxFilterFlags();
}