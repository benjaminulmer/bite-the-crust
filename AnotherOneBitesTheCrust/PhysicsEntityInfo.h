#pragma once
#include <vector>
#include <foundation/PxTransform.h> 
#include <PxShape.h>
#include <glm.hpp>

enum class Geometry
{
	SPHERE,
	BOX,
	CAPSULE,
	CONVEX_MESH,
	MAX_TYPES
};

enum class PhysicsType
{
	DYNAMIC,
	STATIC
};

struct DynamicInfo
{
	float density;
	float linearDamping;
	float angularDamping;
	float maxAngularVelocity;
};

struct ShapeInfo 
{
	Geometry geometry;
	physx::PxTransform transform;
	
	std::vector<physx::PxShapeFlag> shapeFlags;

	float staticFriction;
	float dynamicFriction;
	float restitution;

	unsigned int filterFlag1;
	unsigned int filterFlag2;
	unsigned int filterFlag3;
	unsigned int filterFlag4;
};

struct BoxInfo
	: public ShapeInfo
{
	float halfX;
	float halfY;
	float halfZ;
};

struct SphereInfo
	: public ShapeInfo
{
	float radius;
};

struct CapsuleInfo
	: public ShapeInfo
{
	float raidus;
	float halfHeight;
};

struct ConvexMeshInfo
	: public ShapeInfo
{
	std::vector<glm::vec3> verts;
};

struct PhysicsEntityInfo
{
	std::vector<ShapeInfo*> shapeInfo;
	PhysicsType type;
	DynamicInfo* dynamicInfo;
};