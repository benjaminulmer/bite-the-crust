#pragma once
#include <PxPhysicsAPI.h>
#include "VehicleSceneQueryData.h"
#include "FrictionPairs.h"
#include "PhysicsCreator.h"
#include "DrivingInput.h"
#include "Vehicle.h"

class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void simulate(unsigned int deltaTimeMs);
	void fetchSimulationResults();
	double getPosX();
	double getPosY();
	double getPosZ();
	void initVehicle(Vehicle* vehicle);

private:
	physx::PxTolerancesScale scale;
	physx::PxDefaultErrorCallback* defaultErrorCallback;
	physx::PxDefaultAllocator* defaultAllocator;
	physx::PxF32 stepSizeS;
	physx::PxU32 numWorkers;
	
	physx::PxFoundation* foundation;
	physx::PxProfileZoneManager* profileZoneManager;
	physx::PxPhysics* physics;
	physx::PxCooking* cooking;
	physx::PxDefaultCpuDispatcher* cpuDispatcher;
	physx::PxScene* scene;
	
	VehicleSceneQueryData* vehicleSceneQueryData;
	physx::PxBatchQuery* batchQuery;

	physx::PxMaterial* drivingSurfaces[FrictionPairs::MAX_NUM_SURFACE_TYPES];
	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* frictionPairs;

	physx::PxMaterial* testChassisMat;
	physx::PxMaterial* testWheelMat;

	physx::PxRigidStatic* groundPlane;

	physx::PxF32 deltaTimeSAcc;
	
	std::vector<physx::PxVehicleWheels*> vehicles;

	void initSimulationData();
	void initPhysX();
	void initVehicles();

	void testScene(); // Test method


};