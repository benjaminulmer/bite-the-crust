/*
* The class responsible for rendering functions. Implemented as a singleton for the sake of GLUT 
  needing a function callback to display.
*/

#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>



#include <GL\glew.h>

#include "res_path.h"
#include "ShaderTools.h"
#include "Renderable.h"
#include "Camera.h"

using namespace std;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::perspective;

class RenderingEngine
{
public:
	RenderingEngine(void);
	~RenderingEngine(void);

	void pushEntities();
	void draw();
	//void RenderText(string message, SDL_Color color, int x, int y, int size);

	void init();
	void displayFunc(vector<Entity*> entities);
	mat4 calculateDefaultModel(mat4 model, Entity * entity);

	void generateIDs();
	void deleteIDs();
	void loadProjectionMatrix();
	void updateView(Camera& c);

	void assignBuffers(Renderable* r);
	void deleteBuffers(Renderable* r);

	void testOBJLoading();

	

bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec3> & out_normals,
	std::vector<GLuint> & out_faces
);

bool loadOBJPhong(
	const char * path, 
	std::vector<glm::vec4> & out_vertices, 
	std::vector<glm::vec3> & out_normals,
	std::vector<GLuint> & out_faces
);

bool loadOBJNonIndexed(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	//std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
);

	GLuint basicProgramID;		//shader program 

	GLuint vanVAO;
	GLuint vanVerts;
	GLuint vanColors;
	GLuint vanNormals;
	GLuint vanIndexBuffer;

	vector <vec3> colors;
	//vector <vec3> vertices;
	vector <GLuint> faces;

	mat4 MV;
	mat4 M;
	mat4 V;
	mat4 P;

	//phong testing
	GLuint phongProgramID;
	vector <vec3> phongVerts;
	vector <vec3> phongNorms;
	vector <GLuint> phongFaces;
	vector <glm::vec2> phongUVs;
};

