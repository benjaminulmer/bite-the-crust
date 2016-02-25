#pragma once

enum geometry
{
	SPHERE,
	BOX,
	CAPSULE,
	CONVEX_MESH,
	MAX_TYPES
};

struct PhysicsEntityInfo
{
	

	geometry type;
};

