#include "FrictionPairs.h"

using namespace physx;

PxVehicleDrivableSurfaceToTireFrictionPairs* FrictionPairs::createFrictionPairs(const PxMaterial* defaultMaterial){
	//Tire model friction for each combination of drivable surface type and tire type.
	PxF32 tireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES]=
	{
		//NORMAL,	WORN
		{1.20f,		0.1f}//TARMAC
	};

	PxVehicleDrivableSurfaceType surfaceTypes[1];
	surfaceTypes[0].mType = SURFACE_TYPE_TARMAC;

	const PxMaterial* surfaceMaterials[1];
	surfaceMaterials[0] = defaultMaterial;

	PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
		PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES,MAX_NUM_SURFACE_TYPES);

	surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, surfaceMaterials, surfaceTypes);

	for(PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++) {
		for(PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++) {
			surfaceTirePairs->setTypePairFriction(i, j, tireFrictionMultipliers[i][j]);
		}
	}
	return surfaceTirePairs;
}
