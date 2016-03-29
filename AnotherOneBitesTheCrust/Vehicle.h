#pragma once
#include <PxPhysicsAPI.h>
#include "DynamicEntity.h"
#include <sigslot.h>

struct VehicleInput
{
	float steer;
	float forward;
	float backward;
	bool handBrake;
	bool shootPizza;

	// inputs for cheats
	bool jump;
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
	Vehicle(unsigned int stepSizeMS);
	~Vehicle(void);

	static const int MAX_PIZZAS = 3;
	int pizzaCount;

	// AI stuff; might be moved into 'Player' class
	std::vector<glm::vec3> currentPath;
	bool pizzaDelivered, isAI, newDestination, avoiding, pickingUp;
	int avoidAttempts, stuckDuration;
	glm::vec3 getDestination();
	// END AI stuff

	VehicleInput input;
	VehicleTuning tuning;

	GLuint houseTexture;
	glm::vec3 color;

	bool isSlipping;
	bool isInAir;

	void update();
	glm::mat4 getModelMatrix();

	void setPhysicsVehicle(physx::PxVehicleDrive4W* vehicle);
	physx::PxVehicleDrive4W* getPhysicsVehicle();

	sigslot::signal1<Vehicle*> shootPizzaSignal;
	sigslot::signal1<Vehicle*> dryFireSignal;
	sigslot::signal1<Vehicle*> brakeSignal;
	sigslot::signal1<Vehicle*> idleSignal;
	sigslot::signal1<Vehicle*> gasSignal;

private:
	physx::PxF32 tipAngle;

	physx::PxF32 stepSizeS;
	physx::PxVehicleDrive4W* physicsVehicle;
	physx::PxVehicleDrive4WRawInputData vehicleInput;

	physx::PxVehiclePadSmoothingData smoothingData;
	physx::PxF32 steerVsSpeedData[2*8];
	physx::PxFixedSizeLookupTable<8> steerVsSpeedTable;

	void resetIfNeeded();
	void setSmoothingData();
	void setSteerSpeedData();
};

