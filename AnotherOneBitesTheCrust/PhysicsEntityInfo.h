#pragma once
#include "Filtering.h"

#include <vector>
#include <foundation/PxTransform.h> 
#include <glm.hpp>

enum class Geometry
{
	SPHERE,
	BOX,
	CAPSULE,
	CONVEX_MESH,
	TRIANGLE_MESH,
	MAX_TYPES
};

enum class PhysicsType
{
	DYNAMIC,
	STATIC
};

struct DynamicInfo
{
	DynamicInfo() : density(1.0f), linearDamping(0.0f), angularDamping(0.0f), maxAngularVelocity(100.0f),
	                cmOffset(0.0f, 0.0f, 0.0f){}

	float density;
	float linearDamping;
	float angularDamping;
	float maxAngularVelocity;

	physx::PxVec3 cmOffset;
};

struct ShapeInfo 
{
	ShapeInfo() : transform(0.0f, 0.0f, 0.0f),
		          staticFriction(0.5f), dynamicFriction(0.5f), restitution(0.1f), isDrivable(false),
		          filterFlag0(FilterFlag::OBSTACLE), filterFlag1(FilterFlag::OBSTACLE_AGAINST), 
				  filterFlag2(FilterFlag::DEFAULT), filterFlag3(FilterFlag::DEFAULT) {}

	Geometry geometry;
	physx::PxTransform transform;
	physx::PxShapeFlags shapeFlags; // Not used yet, defaults are complicated

	bool isDrivable;

	float staticFriction;
	float dynamicFriction;
	float restitution;

	FilterFlag filterFlag0;
	FilterFlag filterFlag1;
	FilterFlag filterFlag2;
	FilterFlag filterFlag3;
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

struct TriangleMeshInfo
	: public ShapeInfo
{
	std::vector<glm::vec3> verts;
	std::vector<unsigned int> faces;
};

struct PhysicsEntityInfo
{
	std::vector<ShapeInfo*> shapeInfo;
	PhysicsType type;
	DynamicInfo* dynamicInfo;
};