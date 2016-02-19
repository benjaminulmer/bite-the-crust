#pragma once
#include "PhysicsEntity.h"
#include <PxPhysicsAPI.h>
#include "DrivingInput.h"
#include "DynamicEntity.h"
#include <sigslot.h>

class Vehicle :
	public DynamicEntity, public sigslot::has_slots<>
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

	// AI stuff; might be moved into 'Player' class
	std::vector<glm::vec3> currentPath;

	physx::PxVehicleDrive4W* getPhysicsVehicle();
	void updateTuning();

	void handleInput();
	DrivingInput* getInputStruct();

	sigslot::signal1<Vehicle*> ShootPizzaSignal;

private:
	DrivingInput input;
};

