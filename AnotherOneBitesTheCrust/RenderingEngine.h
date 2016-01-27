/*
* The class responsible for rendering functions. Implemented as a singleton for the sake of GLUT 
  needing a function callback to display.
*/

#pragma once
#include <vector>
#include "TestEntity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>



#include <GL\glew.h>

#include "res_path.h"
#include "ShaderTools.h"


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
	void displayFunc();

	void generateIDs();
	void deleteIDs();
	void setupVAO();
	void loadBuffer();
	void loadModelViewMatrix();
	void loadProjectionMatrix();
	void setupModelViewProjectionTransform();
	void reloadMVPUniform();



	
	GLuint basicProgramID;		//shader program for cube
	GLuint floorShaderID;

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
	mat4 floorMVP;
	mat4 floorM;

};

