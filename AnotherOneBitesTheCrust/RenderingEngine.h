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

using namespace std;
using glm::vec3;
using glm::mat4;
using glm::perspective;


struct Renderables
{
	vector <vec3> vertices;
	mat4 model;
	GLuint vaoID;
};


class RenderingEngine
{
public:
	RenderingEngine(void);
	~RenderingEngine(void);

	void pushEntities();
	void draw();

	void init();
	void displayFunc(vector<Renderable*> renderables);

	void generateIDs();
	void deleteIDs();
	void setupVAO();
	void loadBuffer();
	void loadModelViewMatrix();
	void loadProjectionMatrix();
	void setupModelViewProjectionTransform();
	void reloadMVPUniform();

	void assignBuffers(Renderable* r);
	void deleteBuffers(Renderable* r);

	
	GLuint basicProgramID;		//shader program 

	GLuint vaoID;			//cube vao
	GLuint vertBufferID;	//VBO for vertices
	GLuint colorBufferID;	//VBO for colors

	GLuint floorID;
	GLuint floorBuffer;
	GLuint floorColorBuffer;

	mat4 MVP;
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 floorM;

};

