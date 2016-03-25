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
#include "graphNode.h"
#include <random>

struct NodeTemplate
{
	glm::vec3 position;
	std::vector<int> neighbours;
};

struct PackedVertex{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};


namespace ContentLoading
{
	Renderable* createRenderable(std::string modelFile);
	PhysicsEntityInfo* createPhysicsInfo(const char* filename, Renderable* model);

	bool loadVehicleData(char* filename, Vehicle* vehicle);
	bool verifyEntityList(const rapidjson::Document &d);
	bool loadEntityList(char* filename, std::map<std::string, Renderable*> &modelMap, std::map<std::string, PhysicsEntityInfo*> &physicsMap, std::map<std::string, GLuint> &textureMap);
	bool loadRenderables(char* filename, std::map<std::string, Renderable*> &map);
	bool validateMap(rapidjson::Document &d);
	bool loadMap(char* filename, Map &map);
	bool loadGraph(char * filename, graphNode &node);
	bool getSimilarVertexIndex_fast( 
		PackedVertex & packed, 
		std::map<PackedVertex,unsigned short> & VertexToOutIndex,
		unsigned short & result);
	void indexVBO(
		std::vector<glm::vec3> & in_vertices,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,
		std::vector<unsigned short> & out_indices,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals);
	bool loadOBJ(
		const char * path, 
		std::vector<glm::vec3> & out_vertices, 
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals,
		std::vector<GLuint> &out_faces,
		std::vector<glm::vec3> & raw_verts);
	GLuint loadDDS(const char * imagepath);

	extern std::map<std::string, Renderable*> loadedModels;
	extern std::map<std::string, GLuint> loadedTextures;
};

