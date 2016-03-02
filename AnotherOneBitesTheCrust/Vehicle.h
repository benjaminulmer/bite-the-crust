#pragma once
#include <PxPhysicsAPI.h>
#include "DynamicEntity.h"
#include <sigslot.h>

struct VehicleInput
{
	float leftSteer;
	float rightSteer;
	float forward;
	float backward;
	bool handBrake;
	bool shootPizza;
};

struct VehicleTuning
{
	// Should be taken from models
	physx::PxVec3 chassisDims;
	physx::PxF32 wheelWidth;
	physx::PxF32 wheelRadius;

	// Chassis physical properties
	physx::PxF32 chassisMass;
	physx::PxVec3 chassisMOI;
	physx::PxVec3 chassisCMOffset;
	physx::PxMaterial* chassisMaterial;

	// Wheel physical properties
	physx::PxF32 wheelMass;
	physx::PxF32 wheelMOI;
	physx::PxF32 wheelDamping;
	physx::PxU32 numWheels;
	physx::PxMaterial* wheelMaterial;

	// Other
	physx::PxF32 maxBrakeTorque;
	physx::PxF32 maxHandBrakeTorque;
	physx::PxF32 maxSteer;

	physx::PxF32 engineTorque;
	physx::PxF32 engineRPM;
	physx::PxF32 engineMOI;

	physx::PxF32 gearSwitchTime;
	physx::PxF32 gearFinalRatio;

	physx::PxF32 clutchStrength;

	// User only properties (not used directly in vehicle creation)
	physx::PxReal chassisStaticFriction;
	physx::PxReal chassisDynamicFriction;
	physx::PxReal chassisRestitution;
	physx::PxReal wheelStaticFriction;
	physx::PxReal wheelDynamicFriction;
	physx::PxReal wheelRestitution;

	physx::PxVec3 chassisMOIscalar;
	physx::PxF32 wheelMOIscalar;
	physx::PxU32 maxSteerDegrees;
};

class Vehicle :
	public DynamicEntity
{
public:
	Vehicle(void);
	~Vehicle(void);

	physx::PxVehicleDrive4W* physicsVehicle;
	VehicleInput input;
	VehicleTuning tuning;

	// AI stuff; might be moved into 'Player' class
	std::vector<glm::vec3> currentPath;

	void updateTuning();
	void handleInput();
	physx::PxVehicleDrive4W* getPhysicsVehicle();
	glm::mat4 getModelMatrix();

	sigslot::signal1<Vehicle*> ShootPizzaSignal;	

private:
	void testTuning();
};

