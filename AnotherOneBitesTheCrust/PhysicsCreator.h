#pragma once
#include <PxPhysicsAPI.h>
#include "Vehicle.h"

class PhysicsCreator
{
public:
	static physx::PxRigidDynamic* createBox(physx::PxMaterial* material, physx::PxPhysics* physics, physx::PxVec3 dimensions);

	static physx::PxRigidStatic* createDrivablePlane(physx::PxMaterial* material, physx::PxPhysics* physics);

	static physx::PxConvexMesh* createConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking);

	static physx::PxConvexMesh* createChassisMesh(const physx::PxVec3 dims, physx::PxPhysics& physics, physx::PxCooking& cooking);

	static physx::PxConvexMesh* createWheelMesh(const physx::PxF32 width, const physx::PxF32 radius, physx::PxPhysics& physics, physx::PxCooking& cooking);

	static physx::PxRigidDynamic* createVehicleActor(const physx::PxVehicleChassisData& chassisData, physx::PxMaterial** wheelMaterials,
		                                             physx::PxConvexMesh** wheelConvexMeshes, const physx::PxU32 numWheels, physx::PxMaterial** chassisMaterials, 
													 physx::PxConvexMesh** chassisConvexMeshes, const physx::PxU32 numChassisMeshes, physx::PxPhysics& physics);

	static physx::PxVehicleDrive4W* createVehicle4W(const Vehicle* vehDesc, physx::PxPhysics* physics, physx::PxCooking* cooking);

private:
	static void computeWheelCenterActorOffsets4W(const physx::PxF32 wheelFrontZ, const physx::PxF32 wheelRearZ, const physx::PxVec3& chassisDims, 
		                                         const physx::PxF32 wheelWidth, const physx::PxF32 wheelRadius, const physx::PxU32 numWheels, physx::PxVec3* wheelCentreOffsets);

	static void setupWheelsSimulationData(const physx::PxF32 wheelMass, const physx::PxF32 wheelMOI, const physx::PxF32 wheelRadius, const physx::PxF32 wheelWidth, 
		                                  const physx::PxU32 numWheels, const physx::PxVec3* wheelCenterActorOffsets, const physx::PxVec3& chassisCMOffset,
										  const physx::PxF32 chassisMass, physx::PxVehicleWheelsSimData* wheelsSimData);

	
};