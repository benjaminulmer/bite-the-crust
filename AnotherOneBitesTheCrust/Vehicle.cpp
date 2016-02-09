#include "Vehicle.h"

using namespace physx;

Vehicle::Vehicle(void)
{
	//Set up the default chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	chassisMass = 1500.0f;
	chassisDims = PxVec3(2.5f,2.0f,5.0f);
	chassisMOI = PxVec3
		((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass/12.0f,
		 (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass/12.0f,
		 (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass/12.0f);
	chassisCMOffset = PxVec3(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	wheelMass = 20.0f;
	wheelRadius = 0.5f;
	wheelWidth = 0.4f;
	wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
	numWheels = 6;

	chassisStaticFriction = 0.5f;
	chassisDynamicFriction = 0.5f;
	chassisRestitution = 0.6f;
	wheelStaticFriction = 0.5f;
	wheelDynamicFriction = 0.5f;
	wheelRestitution = 0.5f;
}


Vehicle::~Vehicle(void)
{
}

void Vehicle::updateTuning() {
	// Recalculate variables which are based off of other variables, if some of them have been changed.
	chassisMOI = PxVec3
	((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass/12.0f,
	 (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass/12.0f,
	 (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass/12.0f);
	chassisCMOffset = PxVec3(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);
	wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
}

void Vehicle::handleInput(DrivingInput* input) {
	physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, input->accel);
	physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, input->brake);
	physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, input->leftSteer);
	physicsVehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, input->rightSteer);
}