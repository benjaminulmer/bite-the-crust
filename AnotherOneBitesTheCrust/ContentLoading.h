#pragma once
#include "Vehicle.h"
#include <iostream>
#include <filereadstream.h>
#include <document.h>
#include <map>
#include <vector>
#include "glm.hpp"
#include <string>
#include "Map.h"
#include "PhysicsEngine.h"

namespace ContentLoading
{
	bool loadVehicleData(char* filename, Vehicle* vehicle);
	bool loadRenderables(char* filename, std::map<std::string, Renderable*> &map);
	bool loadMap(char* filename, Map &map);

	bool loadOBJNonIndexed(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	//std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
);
};

