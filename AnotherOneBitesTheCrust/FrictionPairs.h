#pragma once
#include <PxPhysicsAPI.h>

enum class SurfaceType
	{
		TARMAC,
		MAX
	};

enum class TireType
	{
		NORMAL,
		WORN,
		MAX
	};

class FrictionPairs
{
public:
	static physx::PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs(const physx::PxMaterial* defaultMaterial);
};

