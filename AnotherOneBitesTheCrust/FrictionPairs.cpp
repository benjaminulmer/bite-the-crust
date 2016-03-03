#include "FrictionPairs.h"

using namespace physx;

PxVehicleDrivableSurfaceToTireFrictionPairs* FrictionPairs::createFrictionPairs(const PxMaterial* defaultMaterial){
	//Tire model friction for each combination of drivable surface type and tire type.
	PxF32 tireFrictionMultipliers[SurfaceType::MAX][TireType::MAX]=
	{
		//NORMAL,	WORN
		{2.0f,		0.1f}//TARMAC
	};

	PxVehicleDrivableSurfaceType surfaceTypes[1];
	surfaceTypes[0].mType = (PxU32)SurfaceType::TARMAC;

	const PxMaterial* surfaceMaterials[1];
	surfaceMaterials[0] = defaultMaterial;

	PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
		PxVehicleDrivableSurfaceToTireFrictionPairs::allocate((PxU32)TireType::MAX, (PxU32)SurfaceType::MAX);

	surfaceTirePairs->setup((PxU32)TireType::MAX, (PxU32)SurfaceType::MAX, surfaceMaterials, surfaceTypes);

	for(PxU32 i = 0; i < (PxU32)SurfaceType::MAX; i++) {
		for(PxU32 j = 0; j < (PxU32)TireType::MAX; j++) {
			surfaceTirePairs->setTypePairFriction(i, j, tireFrictionMultipliers[i][j]);
		}
	}
	return surfaceTirePairs;
}
