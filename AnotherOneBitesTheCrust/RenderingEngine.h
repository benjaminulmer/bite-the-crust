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
using glm::mat4;
using glm::perspective;

class RenderingEngine
{
public:
	RenderingEngine(void);
	~RenderingEngine(void);

	void pushEntities();
	void draw();

	void init();
	void displayFunc(vector<Entity*> renderables);
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
	GLuint basicProgramID;		//shader program 

	GLuint vanVAO;
	GLuint vanVerts;
	GLuint vanColors;
	GLuint vanIndexBuffer;

	vector <vec3> colors;
	vector <vec3> vertices;
	vector <GLuint> faces;

	mat4 MVP;
	mat4 M;
	mat4 V;
	mat4 P;
};

