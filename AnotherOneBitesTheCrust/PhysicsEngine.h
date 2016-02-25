#pragma once
#include <PxPhysicsAPI.h>
#include "VehicleSceneQueryData.h"
#include "FrictionPairs.h"
#include "VehicleCreator.h"
#include "Vehicle.h"

class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void simulate(unsigned int deltaTimeMs);
	void fetchSimulationResults();
	//void createStaticEntity(StaticEntity* entity, physx::PxTransform transform);
	void createDynamicEntity(DynamicEntity* entity, physx::PxTransform transform);
	void createVehicle(Vehicle* vehicle, physx::PxTransform transform);

	void createTrigger();

private:
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

	physx::PxMaterial* drivingSurfaces[TireType::MAX];
	physx::PxVehicleDrivableSurfaceToTireFrictionPairs* frictionPairs;

	physx::PxMaterial* testChassisMat;
	physx::PxMaterial* testWheelMat;

	physx::PxRigidStatic* groundPlane;
	
	std::vector<physx::PxVehicleWheels*> vehicles;
	//std::vector<physx::PxRigidDynamic*> entities;

	void initSimulationData();
	void initPhysXSDK();
	void initVehicleSDK();

	static const int MAX_VEHICLES = 4;
};