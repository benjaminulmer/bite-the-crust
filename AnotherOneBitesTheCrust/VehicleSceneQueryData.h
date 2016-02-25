#pragma once
#include <PxPhysicsAPI.h>

enum class Surface
{
	DRIVABLE   = 1 << 0,
	UNDRIVABLE = 1 << 1
};

physx::PxQueryHitType::Enum WheelRaycastPreFilter(physx::PxFilterData filterData0, physx::PxFilterData filterData1, 
												  const void* constantBlock, physx::PxU32 constantBlockSize, physx::PxHitFlags& queryFlags);

//Data structure for quick setup of scene queries for suspension raycasts.
class VehicleSceneQueryData
{
public:
	VehicleSceneQueryData(void);
	~VehicleSceneQueryData(void);

	//Allocate scene query data for up to maxNumVehicles and up to maxNumWheelsPerVehicle with numVehiclesInBatch per batch query.
	static VehicleSceneQueryData* allocate(const physx::PxU32 maxNumVehicles, const physx::PxU32 maxNumWheelsPerVehicle, const physx::PxU32 numVehiclesInBatch, physx::PxAllocatorCallback& allocator);

	//Free allocated buffers.
	void free(physx::PxAllocatorCallback& allocator);

	//Create a PxBatchQuery instance that will be used for a single specified batch.
	static physx::PxBatchQuery* setUpBatchedSceneQuery(const physx::PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, physx::PxScene* scene);

	//Return an array of scene query results for a single specified batch.
	physx::PxRaycastQueryResult* getRaycastQueryResultBuffer(const physx::PxU32 batchId); 

	//Get the number of scene query results that have been allocated for a single batch.
	physx::PxU32 getRaycastQueryResultBufferSize() const; 

private:
	//Number of raycasts per batch
	physx::PxU32 numRaycastsPerBatch;

	//One result for each wheel.
	physx::PxRaycastQueryResult* sqResults;

	//One hit for each wheel.
	physx::PxRaycastHit* sqHitBuffer;

	//Filter shader used to filter drivable and non-drivable surfaces
	physx::PxBatchQueryPreFilterShader preFilterShader;
};

