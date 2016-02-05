#pragma once
#include <PxPhysicsAPI.h>

class FrictionPairs
{
public:
	FrictionPairs(void);
	~FrictionPairs(void);

	enum
	{
		SURFACE_TYPE_TARMAC,
		MAX_NUM_SURFACE_TYPES
	};

	enum
	{
		TIRE_TYPE_NORMAL=0,
		TIRE_TYPE_WORN,
		MAX_NUM_TIRE_TYPES
	};

private:
	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs(const physx::PxMaterial* defaultMaterial);

};

