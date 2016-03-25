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

	physx::PxF32 staticFriction;
	physx::PxF32 dynamicFriction;
	physx::PxF32 restitution;

	FilterFlag filterFlag0;
	FilterFlag filterFlag1;
	FilterFlag filterFlag2;
	FilterFlag filterFlag3;
};

struct BoxInfo
	: public ShapeInfo
{
	physx::PxF32 halfX;
	physx::PxF32 halfY;
	physx::PxF32 halfZ;
};

struct SphereInfo
	: public ShapeInfo
{
	physx::PxF32 radius;
};

struct CapsuleInfo
	: public ShapeInfo
{
	physx::PxF32 radius;
	physx::PxF32 halfHeight;
};

struct ConvexMeshInfo
	: public ShapeInfo
{
	std::vector<physx::PxVec3> verts;

	void vertsFromGLMVerts(std::vector<glm::vec3> glmVerts) 
	{
		for (auto glmVert : glmVerts)
		{
			verts.push_back(physx::PxVec3(glmVert.x, glmVert.y, glmVert.z));
		}
	}
};

struct TriangleMeshInfo
	: public ConvexMeshInfo
{
	std::vector<physx::PxU32> faces;
};

struct PhysicsEntityInfo
{
	PhysicsType type;
	DynamicInfo* dynamicInfo;

	std::vector<ShapeInfo*> shapeInfo;
	physx::PxF32 yPosOffset;
};