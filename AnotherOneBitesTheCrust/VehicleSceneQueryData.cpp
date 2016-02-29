#include "VehicleSceneQueryData.h"
#include <new>

using namespace physx;

PxQueryHitType::Enum WheelRaycastPreFilter(PxFilterData filterData0, PxFilterData filterData1, const void* constantBlock, PxU32 constantBlockSize, PxHitFlags& queryFlags)
{
	//filterData0 is the vehicle suspension raycast.
	//filterData1 is the shape potentially hit by the raycast.
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);
	PX_UNUSED(filterData0);
	PX_UNUSED(queryFlags);
	return ((0 == (filterData1.word3 & (PxU32)Surface::DRIVABLE)) ? PxQueryHitType::eNONE : PxQueryHitType::eBLOCK);
}

VehicleSceneQueryData::VehicleSceneQueryData()
:  numRaycastsPerBatch(0),
   sqResults(NULL),
   sqHitBuffer(NULL),
   preFilterShader(WheelRaycastPreFilter)
{
}

VehicleSceneQueryData* VehicleSceneQueryData::allocate(const PxU32 maxNumVehicles, const PxU32 maxNumWheelsPerVehicle, const PxU32 numVehiclesInBatch, PxAllocatorCallback& allocator)
{
	PX_COMPILE_TIME_ASSERT(0 == (sizeof(PxRaycastQueryResult) & 15));
	PX_COMPILE_TIME_ASSERT(0 == (sizeof(PxRaycastHit) & 15));

	const PxU32 sqDataSize = ((sizeof(VehicleSceneQueryData) + 15) & ~15);

	const PxU32 maxNumWheels = maxNumVehicles*maxNumWheelsPerVehicle;

	const PxU32 size = sqDataSize + sizeof(PxRaycastQueryResult)*maxNumWheels + sizeof(PxRaycastHit)*maxNumWheels;
	PxU8* buffer = (PxU8*)allocator.allocate(size, NULL, NULL, 0);
	
	VehicleSceneQueryData* sqData = new(buffer) VehicleSceneQueryData();
	buffer += sqDataSize;

	sqData->numRaycastsPerBatch = numVehiclesInBatch*maxNumWheelsPerVehicle;
	
	sqData->sqResults = (PxRaycastQueryResult*)buffer;
	buffer += sizeof(PxRaycastQueryResult)*maxNumWheels;

	sqData->sqHitBuffer = (PxRaycastHit*)buffer;
	buffer += sizeof(PxRaycastHit)*maxNumWheels;

	for(PxU32 i = 0; i < maxNumVehicles; i++)
	{
		for(PxU32 j = 0; j < maxNumWheelsPerVehicle; j++)
		{
			PX_ASSERT((size_t)(sqData->sqResults + i*maxNumWheelsPerVehicle + j) < (size_t)buffer);
			PX_ASSERT((size_t)(sqData->sqHitBuffer + i*maxNumWheelsPerVehicle + j) < (size_t)buffer);
			new(sqData->sqResults + i*maxNumWheelsPerVehicle + j) PxRaycastQueryResult();
			new(sqData->sqHitBuffer + i*maxNumWheelsPerVehicle + j) PxRaycastHit();
		}
	}

	return sqData;
}

void VehicleSceneQueryData::free(PxAllocatorCallback& allocator)
{
	allocator.deallocate(this);
}

PxBatchQuery* VehicleSceneQueryData::setUpBatchedSceneQuery(const PxU32 batchId, const VehicleSceneQueryData& vehicleSceneQueryData, PxScene* scene)
{
	const PxU32 maxNumRaycastsInBatch = vehicleSceneQueryData.numRaycastsPerBatch;
	PxBatchQueryDesc sqDesc(maxNumRaycastsInBatch, 0, 0);
	sqDesc.queryMemory.userRaycastResultBuffer = vehicleSceneQueryData.sqResults + batchId*maxNumRaycastsInBatch;
	sqDesc.queryMemory.userRaycastTouchBuffer = vehicleSceneQueryData.sqHitBuffer + batchId*maxNumRaycastsInBatch;
	sqDesc.queryMemory.raycastTouchBufferSize = maxNumRaycastsInBatch;
	sqDesc.preFilterShader = vehicleSceneQueryData.preFilterShader;
	return scene->createBatchQuery(sqDesc);
}

PxRaycastQueryResult* VehicleSceneQueryData::getRaycastQueryResultBuffer(const PxU32 batchId) 
{
	return (sqResults + batchId*numRaycastsPerBatch);
}

PxU32 VehicleSceneQueryData::getRaycastQueryResultBufferSize() const 
{
	return numRaycastsPerBatch;
}

VehicleSceneQueryData::~VehicleSceneQueryData()
{
}