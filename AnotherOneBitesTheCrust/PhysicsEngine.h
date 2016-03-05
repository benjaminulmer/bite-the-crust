#pragma once
#include <PxPhysicsAPI.h>
#include "PhysicsHelper.h"
#include "VehicleCreator.h"
#include "VehicleSceneQueryData.h"
#include "FrictionPairs.h"
#include "Vehicle.h"
#include "PhysicsEntityInfo.h"
#include "SimulationCallback.h"

class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void simulate(unsigned int deltaTimeMs);
	void fetchSimulationResults();
	
	void createEntity(PhysicsEntity* entity, PhysicsEntityInfo* info, physx::PxTransform transform);
	void createVehicle(Vehicle* vehicle, physx::PxTransform transform);
	//TODO make this not suck
	void createPizzaPickup();

	SimulationCallback* simulationCallback;
private:
	static const int MAX_VEHICLES = 4;
	PhysicsHelper* helper;
	VehicleCreator* vehCreator;

	physx::PxTolerancesScale scale;
	physx::PxDefaultErrorCallback* defaultErrorCallback;
	physx::PxDefaultAllocator* defaultAllocator;
	physx::PxU32 numWorkers;
	
	physx::PxFoundation* foundation;
	physx::PxProfileZoneManager* profileZoneManager;
	physx::PxPhysics* physics;
	physx::PxCooking* cooking;
	physx::PxDefaultCpuDispatcher* cpuDispatcher;
	physx::PxScene* scene;
	
	VehicleSceneQueryData* vehicleSceneQueryData;
	physx::PxBatchQuery* batchQuery;

	physx::PxMaterial* drivingSurfaces[SurfaceType::MAX];
	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* frictionPairs;

	physx::PxRigidStatic* groundPlane;
	
	std::vector<physx::PxVehicleWheels*> vehicles;

	void initSimulationData();
	void initPhysXSDK();
	void initVehicleSDK();

	void tuningFromUserTuning(Vehicle* vehicle);
};