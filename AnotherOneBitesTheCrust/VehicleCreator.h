#pragma once
#include <PxPhysicsAPI.h>
#include "PhysicsHelper.h"
#include "Vehicle.h"

class VehicleCreator
{
public:
	VehicleCreator(physx::PxPhysics* physics, physx::PxCooking* cooking, PhysicsHelper* helper);
	~VehicleCreator(void);

	physx::PxConvexMesh* createChassisMesh(const physx::PxVec3 dims);

	physx::PxConvexMesh* createWheelMesh(const physx::PxF32 width, const physx::PxF32 radius);

	physx::PxRigidDynamic* createVehicleActor(const physx::PxVehicleChassisData& chassisData, physx::PxMaterial** wheelMaterials,
		                                      physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, physx::PxMaterial** chassisMaterials, 
											  physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, physx::PxPhysics& physics);

	physx::PxVehicleDrive4W* createVehicle4W(Vehicle* vehDesc);

private:
	void computeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, 
		                                  const physx::PxF32 wheelWidth, const physx::PxF32 wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets);

	void setupWheelsSimulationData(const physx::PxF32 wheelMass, const physx::PxF32 wheelMOI, const physx::PxF32 wheelRadius, const physx::PxF32 wheelWidth, 
		                           const physx::PxU32 numWheels, const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset,
								   const physx::PxF32 chassisMass, physx::PxVehicleWheelsSimData* wheelsSimData);

	physx::PxPhysics* physics;
	physx::PxCooking* cooking;
	PhysicsHelper* helper;
};