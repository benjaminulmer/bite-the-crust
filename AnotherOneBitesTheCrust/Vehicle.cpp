#include "Vehicle.h"
#include <iostream>

using namespace physx;

Vehicle::Vehicle(void)
{
	currentPath = std::vector<glm::vec3>();

	input.forward = 0;
	input.backward = 0;
	input.steer = 0;
	input.handBrake = false;
	input.shootPizza = false;

	tipAngle = 0;

	vehicleInput = PxVehicleDrive4WRawInputData();

	setSmoothingData();
	setSteerSpeedData();
}

void Vehicle::setSmoothingData()
{
	smoothingData.mRiseRates[0] = 6.0f;
	smoothingData.mRiseRates[1] = 6.0f;
	smoothingData.mRiseRates[2] = 12.0f;
	smoothingData.mRiseRates[3] = 2.5f;
	smoothingData.mRiseRates[4] = 2.5f;

	smoothingData.mFallRates[0] = 10.0f;
	smoothingData.mFallRates[1] = 10.0f;
	smoothingData.mFallRates[2] = 12.0f;
	smoothingData.mFallRates[3] = 5.0f;
	smoothingData.mFallRates[4] = 5.0f;
}

void Vehicle::setSteerSpeedData()
{
	steerVsSpeedData[0] = 0.0f;          steerVsSpeedData[1] = 0.75f;
	steerVsSpeedData[2] = 5.0f;          steerVsSpeedData[3] = 0.75f;
	steerVsSpeedData[4] = 30.0f;         steerVsSpeedData[5] = 0.125f;
	steerVsSpeedData[6] = 120.0f;        steerVsSpeedData[7] = 0.1f;
	steerVsSpeedData[8] = PX_MAX_F32;    steerVsSpeedData[9] = PX_MAX_F32;
	steerVsSpeedData[10] = PX_MAX_F32;    steerVsSpeedData[11] = PX_MAX_F32;
	steerVsSpeedData[12] = PX_MAX_F32;    steerVsSpeedData[13] = PX_MAX_F32;
	steerVsSpeedData[14] = PX_MAX_F32;    steerVsSpeedData[15] = PX_MAX_F32;
	steerVsSpeedTable = PxFixedSizeLookupTable<8>(steerVsSpeedData, 4); 
}

void Vehicle::setPhysicsVehicle(physx::PxVehicleDrive4W* vehicle)
{
	physicsVehicle = vehicle;
}

physx::PxVehicleDrive4W* Vehicle::getPhysicsVehicle()
{
	return physicsVehicle;
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
		vehicleInput.setAnalogAccel(input.backward);
		vehicleInput.setAnalogBrake(input.forward);
	} 
	else 
	{
		vehicleInput.setAnalogAccel(input.forward);
		vehicleInput.setAnalogBrake(input.backward);
	}

	// Steer and handbrake
	vehicleInput.setAnalogSteer(input.steer);
	vehicleInput.setAnalogHandbrake(handBrake);

	PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(smoothingData, steerVsSpeedTable, vehicleInput, 16.0f/1000.0f, false, *physicsVehicle);

	std::cout << input.steer << std::endl;

	if (input.shootPizza)
	{
		shootPizzaSignal(this);
		input.shootPizza = false;
	}
}

glm::mat4 Vehicle::getModelMatrix()
{
	tipAngle = 0.98 * tipAngle + 0.02 * input.steer * physicsVehicle->computeForwardSpeed() * 0.01f;

	PxTransform transform(PxQuat(tipAngle, PxVec3(0, 0, 1)));
	transform = actor->getGlobalPose() * transform;

	physx::PxMat44 oldM(transform);
	glm::mat4 newM;
	for(unsigned int i = 0; i < 4; i++)
	{
		for(unsigned int f = 0; f < 4; f++)
		{
			newM[i][f] = oldM[i][f];
		}
	}
	return newM;
}

Vehicle::~Vehicle(void)
{
}

