#include "Vehicle.h"
#include <cmath>
#include <iostream>

using namespace physx;

Vehicle::Vehicle(unsigned int stepSizeMS)
{
	currentPath = std::vector<glm::vec3>();

	input.forward = 0;
	input.backward = 0;
	input.steer = 0;
	input.handBrake = false;
	input.shootPizza = false;

	stepSizeS = stepSizeMS/100.0f;
	tipAngle = 0;
	vehicleInput = PxVehicleDrive4WRawInputData();

	setSmoothingData();
	setSteerSpeedData();

	pizzaCount = 3;
}

glm::vec3 Vehicle::getDestination()
{
	return currentPath.back();
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

void Vehicle::update()
{
	PxVec3 up(0, 1, 0);
	PxVec3 vehUp = actor->getGlobalPose().rotate(up);

	PxF32 cos = vehUp.dot(up);
	PxVec3 vel = ((PxRigidDynamic*)actor)->getLinearVelocity();
	if (cos <= 0.5f && vel.x == 0 && vel.y == 0 && vel.z == 0)
	{
		PxVec3 forw(0, 0, 1);
		PxVec3 vehForw = actor->getGlobalPose().rotate(forw);
		PxF32 angleRad = acos(vehForw.dot(forw));

		PxTransform cur = actor->getGlobalPose();
		actor->setGlobalPose(PxTransform(PxVec3(cur.p.x, cur.p.y + 0.5f, cur.p.z), PxQuat(-angleRad, up)));
	}

	float handBrake = 0;
	float forwardSpeed = physicsVehicle->computeForwardSpeed();
	(input.handBrake) ? handBrake = 1.0f: handBrake = 0.0f;

	// Check if gear should switch from reverse to forward or vise versa
	if (forwardSpeed < 5 && input.backward > 0)
	{
		physicsVehicle->mDriveDynData.setTargetGear(PxVehicleGearsData::eREVERSE);
	}
	else if (forwardSpeed > -5 && input.forward > 0)
	{
		physicsVehicle->mDriveDynData.setTargetGear(PxVehicleGearsData::eFIRST);
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

	PxVehicleDrive4WSmoothAnalogRawInputsAndSetAnalogInputs(smoothingData, steerVsSpeedTable, vehicleInput, stepSizeS, false, *physicsVehicle);

	if (input.shootPizza)
	{ 
		if (pizzaCount > 0) {
			shootPizzaSignal(this);
			pizzaCount--;
		}
		input.shootPizza = false;
	}
}

glm::mat4 Vehicle::getModelMatrix()
{
	PxF32 alpha = 0.02f;
	tipAngle = (1 - alpha) * tipAngle + (alpha * input.steer * physicsVehicle->computeForwardSpeed() * 0.01f);
	if (tipAngle > PxPi * (45.0f/180.0f))
	{
		tipAngle = PxPi * (45.0f/180.0f);
	}

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

