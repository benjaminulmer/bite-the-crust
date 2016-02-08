#pragma once
#include "physicsentity.h"
#include <PxPhysicsAPI.h>
#include "DrivingInput.h"

class Vehicle :
	public PhysicsEntity
{
public:
	Vehicle(void);
	~Vehicle(void);

	physx::PxF32 chassisMass;
	physx::PxVec3 chassisDims;
	physx::PxVec3 chassisMOI;
	physx::PxVec3 chassisCMOffset;
	physx::PxMaterial* chassisMaterial;
	physx::PxF32 wheelMass;
	physx::PxF32 wheelWidth;
	physx::PxF32 wheelRadius;
	physx::PxF32 wheelMOI;
	physx::PxMaterial* wheelMaterial;
	physx::PxU32 numWheels;
	physx::PxReal chassisStaticFriction;
	physx::PxReal chassisDynamicFriction;
	physx::PxReal chassisRestitution;
	physx::PxReal wheelStaticFriction;
	physx::PxReal wheelDynamicFriction;
	physx::PxReal wheelRestitution;
	physx::PxVehicleDrive4W* physicsVehicle;

	void handleInput(DrivingInput* input);
	void updateTuning();
private: 
};

