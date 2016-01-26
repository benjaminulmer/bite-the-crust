/*
  A super basic entity class that just defines a struct for vectors with 3 floating points (I'd rather we bring in some library with vectors).
  An entity is just a set of vertices and a position. Really we'll have a hierarchy, and for entities we want to render we'll preload
  from the vertices as .obj or whatever into a VBO handler so that we only have to set it up on the GPU once.
*/

#pragma once
#include <vector>

using namespace std;

struct Vector3f {
	float x, y, z;

	Vector3f() {

		}
	Vector3f(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}
};

class TestEntity
{
public:
	TestEntity();
	~TestEntity();

	Vector3f position;
	vector<Vector3f> vertices;
};