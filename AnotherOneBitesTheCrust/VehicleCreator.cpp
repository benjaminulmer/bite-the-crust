#include "VehicleCreator.h"
#include "VehicleSceneQueryData.h"
#include "Filtering.h"
#include "FrictionPairs.h"
#include "PhysicsHelper.h"

using namespace physx;

PxConvexMesh* VehicleCreator::createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking)
{
	const PxF32 x = dims.x*0.5f;
	const PxF32 y = dims.y*0.5f;
	const PxF32 z = dims.z*0.5f;
	PxVec3 verts[8] =
	{
		PxVec3(x,y,-z), 
		PxVec3(x,y,z),
		PxVec3(x,-y,z),
		PxVec3(x,-y,-z),
		PxVec3(-x,y,-z), 
		PxVec3(-x,y,z),
		PxVec3(-x,-y,z),
		PxVec3(-x,-y,-z)
	};
	return PhysicsHelper::createConvexMesh(verts,8,physics,cooking);
}

PxConvexMesh* VehicleCreator::createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking)
{
	PxVec3 points[2*16];
	for(PxU32 i = 0; i < 16; i++)
	{
		const PxF32 cosTheta = PxCos(i*PxPi*2.0f/16.0f);
		const PxF32 sinTheta = PxSin(i*PxPi*2.0f/16.0f);
		const PxF32 y = radius*cosTheta;
		const PxF32 z = radius*sinTheta;
		points[2*i+0] = PxVec3(-width/2.0f, y, z);
		points[2*i+1] = PxVec3(+width/2.0f, y, z);
	}

	return PhysicsHelper::createConvexMesh(points,32,physics,cooking);
}

PxRigidDynamic* VehicleCreator::createVehicleActor(const PxVehicleChassisData& chassisData, PxMaterial** wheelMaterials, 
												   PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,PxMaterial** chassisMaterials, 
												   PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, PxPhysics& physics)
{
	//We need a rigid body actor for the vehicle.
	//Don't forget to add the actor to the scene after setting up the associated vehicle.
	PxRigidDynamic* vehActor = physics.createRigidDynamic(PxTransform(PxIdentity));

	//Wheel and chassis simulation filter data.
	PxFilterData wheelSimFilterData;
	wheelSimFilterData.word0 = (PxU32)FilterFlag::WHEEL;
	wheelSimFilterData.word1 = (PxU32)FilterFlag::WHEEL_AGAINST;
	PxFilterData chassisSimFilterData;
	chassisSimFilterData.word0 = (PxU32)FilterFlag::CHASSIS;
	chassisSimFilterData.word1 = (PxU32)FilterFlag::CHASSIS_AGAINST;

	//Wheel and chassis query filter data.
	//Optional: cars don't drive on other cars.
	PxFilterData wheelQryFilterData;
	wheelQryFilterData.word3 = (PxU32)Surface::UNDRIVABLE;
	PxFilterData chassisQryFilterData;
	chassisQryFilterData.word3 = (PxU32)Surface::UNDRIVABLE;

	//Add all the wheel shapes to the actor.
	for(PxU32 i = 0; i < numWheels; i++)
	{
		PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
		PxShape* wheelShape=vehActor->createShape(geom, *wheelMaterials[i]);
		wheelShape->setQueryFilterData(wheelQryFilterData);
		wheelShape->setSimulationFilterData(wheelSimFilterData);
		wheelShape->setLocalPose(PxTransform(PxIdentity));
	}

	//Add the chassis shapes to the actor.
	for(PxU32 i = 0; i < numChassisMeshes; i++)
	{
		PxShape* chassisShape=vehActor->createShape(PxConvexMeshGeometry(chassisConvexMeshes[i]),*chassisMaterials[i]);
		chassisShape->setQueryFilterData(chassisQryFilterData);
		chassisShape->setSimulationFilterData(chassisSimFilterData);
		chassisShape->setLocalPose(PxTransform(PxIdentity));
	}
	vehActor->setMass(chassisData.mMass);
	vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
	vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset,PxQuat(PxIdentity)));

	return vehActor;
}

void VehicleCreator::computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, 
													  const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 numWheels, PxVec3* wheelCentreOffsets)
{
	//chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
	//The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
	//Compute a position for the front wheel and the rear wheel along the z-axis.
	//Compute the separation between each wheel along the z-axis.
	const PxF32 numLeftWheels = numWheels/2.0f;
	const PxF32 deltaZ = (wheelFrontZ - wheelRearZ)/(numLeftWheels-1.0f);
	//Set the outside of the left and right wheels to be flush with the chassis.
	//Set the top of the wheel to be just touching the underside of the chassis.
	//Begin by setting the rear-left/rear-right/front-left,front-right wheels.
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT] = PxVec3((-chassisDims.x + wheelWidth)*0.5f, -(chassisDims.y/2 + wheelRadius), wheelRearZ + 0*deltaZ*0.5f);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = PxVec3((+chassisDims.x - wheelWidth)*0.5f, -(chassisDims.y/2 + wheelRadius), wheelRearZ + 0*deltaZ*0.5f);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = PxVec3((-chassisDims.x + wheelWidth)*0.5f, -(chassisDims.y/2 + wheelRadius), wheelRearZ + (numLeftWheels-1)*deltaZ);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = PxVec3((+chassisDims.x - wheelWidth)*0.5f, -(chassisDims.y/2 + wheelRadius), wheelRearZ + (numLeftWheels-1)*deltaZ);
	//Set the remaining wheels.
	for(PxU32 i = 2, wheelCount = 4; i < numWheels-2; i+=2, wheelCount+=2)
	{
		wheelCentreOffsets[wheelCount + 0] = PxVec3((-chassisDims.x + wheelWidth)*0.5f, -(chassisDims.y/2 + wheelRadius), wheelRearZ + i*deltaZ*0.5f);
		wheelCentreOffsets[wheelCount + 1] = PxVec3((+chassisDims.x - wheelWidth)*0.5f, -(chassisDims.y/2 + wheelRadius), wheelRearZ + i*deltaZ*0.5f);
	}
}

void VehicleCreator::setupWheelsSimulationData(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth, 
											   const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets, const PxVec3& chassisCMOffset,
											   const PxF32 chassisMass,PxVehicleWheelsSimData* wheelsSimData)
{
	//Set up the wheels.
	PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
	{
		//Set up the wheel data structures with mass, moi, radius, width.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			wheels[i].mMass = wheelMass;
			wheels[i].mMOI = wheelMOI;
			wheels[i].mRadius = wheelRadius;
			wheels[i].mWidth = wheelWidth;
			//wheels[i].mDampingRate = 0.25f;
		}

		//Enable the handbrake for the rear wheels only.
		wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque=4000.0f;
		wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque=4000.0f;
		//Enable steering for the front wheels only.
		wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer=PxPi*0.3333f;
		wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer=PxPi*0.3333f;
	}

	//Set up the tires.
	PxVehicleTireData tires[PX_MAX_NB_WHEELS];
	{
		//Set up the tires.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			tires[i].mType = (PxU32)TireType::NORMAL;
		}
	}

	//Set up the suspensions
	PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
	{
		//Compute the mass supported by each suspension spring.
		PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
		PxVehicleComputeSprungMasses(numWheels, wheelCenterActorOffsets, chassisCMOffset, chassisMass, 1, suspSprungMasses);

		//Set the suspension data.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			suspensions[i].mMaxCompression = 0.3f;
			suspensions[i].mMaxDroop = 0.1f;
			suspensions[i].mSpringStrength = 35000.0f;	
			suspensions[i].mSpringDamperRate = 4500.0f;
			suspensions[i].mSprungMass = suspSprungMasses[i];
		}

		//Set the camber angles.
		const PxF32 camberAngleAtRest=0.0;
		const PxF32 camberAngleAtMaxDroop=0.01f;
		const PxF32 camberAngleAtMaxCompression=-0.01f;
		for(PxU32 i = 0; i < numWheels; i+=2)
		{
			suspensions[i + 0].mCamberAtRest =  camberAngleAtRest;
			suspensions[i + 1].mCamberAtRest =  -camberAngleAtRest;
			suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
			suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
			suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
			suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
		}
	}

	//Set up the wheel geometry.
	PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
	PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
	PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
	PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
	{
		//Set the geometry data.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			//Vertical suspension travel.
			suspTravelDirections[i] = PxVec3(0,-1,0);

			//Wheel center offset is offset from rigid body center of mass.
			wheelCentreCMOffsets[i] = 
				wheelCenterActorOffsets[i] - chassisCMOffset;

			//Suspension force application point 0.3 metres below 
			//rigid body center of mass.
			suspForceAppCMOffsets[i] =
				PxVec3(wheelCentreCMOffsets[i].x,-0.3f,wheelCentreCMOffsets[i].z);

			//Tire force application point 0.3 metres below 
			//rigid body center of mass.
			tireForceAppCMOffsets[i] =
				PxVec3(wheelCentreCMOffsets[i].x,-0.3f,wheelCentreCMOffsets[i].z);
		}
	}

	//Set up the filter data of the raycast that will be issued by each suspension.
	PxFilterData qryFilterData;
	qryFilterData.word3 = (PxU32)Surface::UNDRIVABLE;

	//Set the wheel, tire and suspension data.
	//Set the geometry data.
	//Set the query filter data
	for(PxU32 i = 0; i < numWheels; i++)
	{
		wheelsSimData->setWheelData(i, wheels[i]);
		wheelsSimData->setTireData(i, tires[i]);
		wheelsSimData->setSuspensionData(i, suspensions[i]);
		wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
		wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
		wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
		wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
		wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
		wheelsSimData->setWheelShapeMapping(i, i); 
	}
}

PxVehicleDrive4W* VehicleCreator::createVehicle4W(Vehicle* vehicle, PxPhysics* physics, PxCooking* cooking) 
{
	VehicleTuning* tuning = &vehicle->tuning;

	const PxVec3 chassisDims = tuning->chassisDims;
	const PxF32 wheelWidth = tuning->wheelWidth;
	const PxF32 wheelRadius = tuning->wheelRadius;
	const PxU32 numWheels = tuning->numWheels;

	//Construct a physx actor with shapes for the chassis and wheels.
	//Set the rigid body mass, moment of inertia, and center of mass offset.
	PxRigidDynamic* veh4WActor = NULL;
	{
		//Construct a convex mesh for a cylindrical wheel.
		PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius, *physics, *cooking);
		//Assume all wheels are identical for simplicity.
		PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
		PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

		//Set the meshes and materials for the driven wheels.
		for(PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = tuning->wheelMaterial;
		}
		//Set the meshes and materials for the non-driven wheels
		for(PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < numWheels; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = tuning->wheelMaterial;
		}

		//Chassis just has a single convex shape for simplicity.
		PxConvexMesh* chassisConvexMesh = createChassisMesh(chassisDims, *physics, *cooking);
		PxConvexMesh* chassisConvexMeshes[1] = {chassisConvexMesh};
		PxMaterial* chassisMaterials[1] = {tuning->chassisMaterial};

		//Rigid body data.
		PxVehicleChassisData rigidBodyData;
		rigidBodyData.mMOI = tuning->chassisMOI;
		rigidBodyData.mMass = tuning->chassisMass;
		rigidBodyData.mCMOffset = tuning->chassisCMOffset;

		veh4WActor = createVehicleActor
			(rigidBodyData,
			wheelMaterials, wheelConvexMeshes, numWheels,
			chassisMaterials, chassisConvexMeshes, 1,
			*physics);
	}

	//Set up the sim data for the wheels.
	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(numWheels);
	{
		//Compute the wheel center offsets from the origin.
		PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
		const PxF32 frontZ = chassisDims.z*0.3f;
		const PxF32 rearZ = -chassisDims.z*0.3f;
		computeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels, wheelCenterActorOffsets);

		//Set up the simulation data for all wheels.
		setupWheelsSimulationData
			(tuning->wheelMass, tuning->wheelMOI, wheelRadius, wheelWidth, 
			 numWheels, wheelCenterActorOffsets,
			 tuning->chassisCMOffset, tuning->chassisMass,
			 wheelsSimData);
	}

	//Set up the sim data for the vehicle drive model.
	PxVehicleDriveSimData4W driveSimData;
	{
		//Diff
		PxVehicleDifferential4WData diff;
		diff.mType=PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
		//diff.mFrontBias = 1.3f;
		//diff.mRearBias = 1.3f;
		//diff.mCentreBias = 1.3f;
		driveSimData.setDiffData(diff);

		//Engine
		PxVehicleEngineData engine;
		engine.mPeakTorque=500.0f;
		engine.mMaxOmega=600.0f;//approx 6000 rpm
		//engine.mMOI = 0.25f;
		driveSimData.setEngineData(engine);

		//Gears
		PxVehicleGearsData gears;
		gears.mSwitchTime=0.2f;
		driveSimData.setGearsData(gears);

		//Clutch
		PxVehicleClutchData clutch;
		clutch.mAccuracyMode = PxVehicleClutchAccuracyMode::eBEST_POSSIBLE;
		clutch.mStrength=10.0f;
		driveSimData.setClutchData(clutch);

		//Ackermann steer accuracy
		PxVehicleAckermannGeometryData ackermann;
		ackermann.mAccuracy=1.0f;
		ackermann.mAxleSeparation=
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z-
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
		ackermann.mFrontWidth=
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x-
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
		ackermann.mRearWidth=
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
			wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
		driveSimData.setAckermannGeometryData(ackermann);
	}

	//Create a vehicle from the wheels and drive sim data.
	PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(numWheels);
	vehDrive4W->setup(physics, veh4WActor, *wheelsSimData, driveSimData, numWheels - 4);

	//Free the sim data because we don't need that any more.
	wheelsSimData->free();

	return vehDrive4W;
}
