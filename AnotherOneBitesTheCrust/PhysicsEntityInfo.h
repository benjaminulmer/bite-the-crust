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

struct GeometryInfo 
{
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
	: public GeometryInfo
{
	float halfX;
	float halfY;
	float halfZ;
};

struct SphereInfo
	: public GeometryInfo
{
	float radius;
};

struct CapsuleInfo
	: public GeometryInfo
{
	float raidus;
	float halfHeight;
};

struct ConvexMeshInfo
	: public GeometryInfo
{
	std::vector<glm::vec3> verts;
};

struct PhysicsEntityInfo
{
	std::vector<Geometry> geometry;
	std::vector<GeometryInfo*> geometryInfo;
	PhysicsType type;
	DynamicInfo* dynamicInfo;
};