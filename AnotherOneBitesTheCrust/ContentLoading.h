#pragma once
#include "Vehicle.h"
#include <iostream>
#include <rapidjson\filereadstream.h>
#include <rapidjson\document.h>

namespace ContentLoading
{
	bool loadVehicleData(char* filename, Vehicle* vehicle);

	bool loadOBJNonIndexed(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	//std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
);
};

