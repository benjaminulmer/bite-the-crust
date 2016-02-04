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


/*struct Entity
{
	string name;
	vector <vec3> vertices;
	vector <vec3> faces;
	mat4 model;
	GLuint vaoID;
};*/


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
	void setupVAO();
	void loadBuffer();
	void loadModelViewMatrix();
	void loadProjectionMatrix();
	void setupModelViewProjectionTransform();
	void reloadMVPUniform();
	void updateView(Camera& c);

	void addEntity();
	void updateEntity();
	void deleteEntity();

	void assignBuffers(Renderable* r);
	void deleteBuffers(Renderable* r);

	
	GLuint basicProgramID;		//shader program 

	GLuint vaoID;			//cube vao
	GLuint vertBufferID;	//VBO for vertices
	GLuint colorBufferID;	//VBO for colors
	GLuint eboID;

	GLuint floorID;
	GLuint floorBuffer;
	GLuint floorColorBuffer;
	GLuint floorEBO;

	mat4 MVP;
	mat4 M;
	mat4 V;
	mat4 P;
	mat4 floorM;

	GLuint myVAO;			//vao
	GLuint myVBO;			//vbo
	GLuint myIndexBuffer;	//ebo

	GLuint myShaderProgram;

	vector <Entity> entities;

};

