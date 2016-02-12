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
	void createDynamicEntity(PhysicsEntity* entity, glm::vec3 position, glm::vec3 velocity = glm::vec3(0,0,0));
	void createVehicle(Vehicle* vehicle);

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
	std::vector<physx::PxRigidDynamic*> entities;

	void initSimulationData();
	void initPhysXSDK();
	void initVehicleSDK();
};