#include "Vehicle.h"

using namespace physx;

Vehicle::Vehicle(void)
{
	testTuning();

	currentPath = std::vector<glm::vec3>();

	input.forward = 0;
	input.backward = 0;
	input.leftSteer = 0;
	input.rightSteer = 0;
	input.handBrake = false;
	input.shootPizza = false;
}

physx::PxVehicleDrive4W* Vehicle::getPhysicsVehicle()
{
	return physicsVehicle;
}

void Vehicle::testTuning()
{
	tuning.chassisStaticFriction = 0.5f;
	tuning.chassisDynamicFriction = 0.5f;
	tuning.chassisRestitution = 0.6f;
	tuning.wheelStaticFriction = 0.5f;
	tuning.wheelDynamicFriction = 0.5f;
	tuning.wheelRestitution = 0.5f;
}

void Vehicle::updateTuning()
{
	// Recalculate variables which are based off of other variables, if some of them have been changed.
	tuning.chassisMOI = PxVec3
		((tuning.chassisDims.y * tuning.chassisDims.y + tuning.chassisDims.z * tuning.chassisDims.z) * tuning.chassisMass/12.0f,
		 (tuning.chassisDims.x * tuning.chassisDims.x + tuning.chassisDims.z * tuning.chassisDims.z) * 0.8f * tuning.chassisMass/12.0f,
		 (tuning.chassisDims.x * tuning.chassisDims.x + tuning.chassisDims.y * tuning.chassisDims.y) * tuning.chassisMass/12.0f);
	tuning.chassisCMOffset = PxVec3(0.0f, -tuning.chassisDims.y * 0.5f + 0.65f, 0.25f);
	tuning.wheelMOI = 0.5f * tuning.wheelMass * tuning.wheelRadius * tuning.wheelRadius;
}

void Vehicle::handleInput()
{
	float handBrake = 0;
	float forwardSpeed = physicsVehicle->computeForwardSpeed();
	(input.handBrake) ? handBrake = 1.0f: handBrake = 0.0f;

	// Check if gear should switch from reverse to forward or vise versa
	if (forwardSpeed == 0 && input.backward > 0)
	{
		physicsVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
	}
	else if (forwardSpeed == 0 && input.forward > 0)
	{
		physicsVehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
	}

	// Determine how to apply controller input depending on current gear
	if (physicsVehicle->mDriveDynData.getCurrentGear() == PxVehicleGearsData::eREVERSE)
	{
		physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, input.backward);
		physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, input.forward);
	} 
	else 
	{
		physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, input.forward);
		physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, input.backward);
	}

	// Steer and handbrake
	physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, input.leftSteer);
	physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, input.rightSteer);
	physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_HANDBRAKE, handBrake);

	if (input.shootPizza)
	{
		ShootPizzaSignal(this);
		input.shootPizza = false;
	}
}

Vehicle::~Vehicle(void)
{
}

// test stuff

#include <iostream>

const void Vehicle::test()
{
	//std::cout << "test" << std::endl;
}