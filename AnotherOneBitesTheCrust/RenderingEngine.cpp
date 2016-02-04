#include "RenderingEngine.h"
#include "TestEntity.h"
#include "Game.h"
#include <math.h>
#include <iostream>

RenderingEngine::RenderingEngine() {}

RenderingEngine::~RenderingEngine(void) {}
// Push all the entity data to the renderer.
void RenderingEngine::pushEntities() {
}

void RenderingEngine::draw() {
	// Draw all our entities

	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}



void RenderingEngine::displayFunc()
{

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	M = mat4(1.0f);
	
	MVP = P * V * M;

	GLint mvpID = glGetUniformLocation(basicProgramID, "MVP");

	glUseProgram(basicProgramID);
	glUniformMatrix4fv( mvpID,
						1,
						GL_FALSE,
						value_ptr(MVP)
						);
	//glUseProgram(basicProgramID);
	glBindVertexArray(vaoID);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	//glDrawArrays(GL_TRIANGLES, 0, 12*3);

	glDrawElements(GL_TRIANGLES,
					6,
					GL_UNSIGNED_INT,
					(void*)0
					);

	floorM = scale(M, vec3(1.0f));
	floorM = rotate(floorM, 45.0f, vec3(0,1,0));
	//floorM = glm::translate(floorM, vec3(2, 0,0));
	////scale rotate translate

	MVP = P * V * floorM;

	glUniformMatrix4fv( mvpID,
						1,
						GL_FALSE,
						value_ptr(MVP)
						);

	glBindVertexArray(floorID);
	//glDrawArrays(GL_QUADS, 0, 12*3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);



}

void RenderingEngine::generateIDs()
{
	cout << "Generating IDs" << endl;
	string vsShader = "res\\Shaders\\basic_vs.glsl";
	string fsShader = "res\\Shaders\\basic_fs.glsl";
	string vsSource = loadShaderStringfromFile(vsShader);
	string fsSource = loadShaderStringfromFile(fsShader);
	basicProgramID = CreateShaderProgram(vsSource, fsSource);

	glGenVertexArrays(1, &vaoID);	//generate VAO
	glGenBuffers(1, &vertBufferID);	//generate buffer for vertices
	glGenBuffers(1, &colorBufferID);	//generate buffer for colors
	glGenBuffers(1, &eboID);

	glGenVertexArrays(1, &floorID);
	glGenBuffers(1, &floorBuffer);
	glGenBuffers(1, &floorColorBuffer);
	glGenBuffers(1, &floorEBO);
}

void RenderingEngine::deleteIDs()
{
	glDeleteProgram(basicProgramID);
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vertBufferID);
	glDeleteBuffers(1, &colorBufferID);

	glDeleteVertexArrays(1, &floorID);
	glDeleteBuffers(1, &floorBuffer);
	glDeleteBuffers(1, &floorColorBuffer);
}

void RenderingEngine::setupVAO()
{
	cout << "Setting up VAO and VBO" << endl;
	
	glBindVertexArray(vaoID);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);	//binding vertex vbo
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);	//binding color vbo
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);

	glBindVertexArray(0);

	glBindVertexArray(floorID);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, floorBuffer);	//binding vertex vbo
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, floorColorBuffer);	//binding color vbo
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
	
	glBindVertexArray(0);
}

void RenderingEngine::loadBuffer()
{
	cout << "Loading the buffers" << endl;

	vector <vec3> verts;
	verts.push_back(vec3(2,-2,2));
	verts.push_back(vec3(2,-2,-2));
	verts.push_back(vec3(-2,-2,-2));
	verts.push_back(vec3(-2,-2,2));

	glBindBuffer(GL_ARRAY_BUFFER, floorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*verts.size(), verts.data(), GL_STATIC_DRAW);

	vector <GLuint> feces;
	feces.push_back(0);
	feces.push_back(1);
	feces.push_back(2);
	feces.push_back(2);
	feces.push_back(3);
	feces.push_back(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6, feces.data(), GL_STATIC_DRAW);

	//static const GLfloat g_vertex_buffer_data[] = {
 //     -1.0f,-1.0f,-1.0f, // triangle 1 : begin
 //     -1.0f,-1.0f, 1.0f,
 //     -1.0f, 1.0f, 1.0f, // triangle 1 : end
 //     1.0f, 1.0f,-1.0f, // triangle 2 : begin
 //     -1.0f,-1.0f,-1.0f,
 //     -1.0f, 1.0f,-1.0f, // triangle 2 : end
 //    1.0f,-1.0f, 1.0f,
 //    -1.0f,-1.0f,-1.0f,
 //    1.0f,-1.0f,-1.0f,
 //    1.0f, 1.0f,-1.0f,
 //    1.0f,-1.0f,-1.0f,
 //    -1.0f,-1.0f,-1.0f,
 //    -1.0f,-1.0f,-1.0f,
 //    -1.0f, 1.0f, 1.0f,
 //    -1.0f, 1.0f,-1.0f,
 //    1.0f,-1.0f, 1.0f,
 //    -1.0f,-1.0f, 1.0f,
 //    -1.0f,-1.0f,-1.0f,
 //    -1.0f, 1.0f, 1.0f,
 //    -1.0f,-1.0f, 1.0f,
 //    1.0f,-1.0f, 1.0f,
 //    1.0f, 1.0f, 1.0f,
 //    1.0f,-1.0f,-1.0f,
 //    1.0f, 1.0f,-1.0f,
 //    1.0f,-1.0f,-1.0f,
 //    1.0f, 1.0f, 1.0f,
 //    1.0f,-1.0f, 1.0f,
 //    1.0f, 1.0f, 1.0f,
 //    1.0f, 1.0f,-1.0f,
 //    -1.0f, 1.0f,-1.0f,
 //    1.0f, 1.0f, 1.0f,
 //    -1.0f, 1.0f,-1.0f,
 //    -1.0f, 1.0f, 1.0f,
 //    1.0f, 1.0f, 1.0f,
 //    -1.0f, 1.0f, 1.0f,
 //    1.0f,-1.0f, 1.0f
 //};

	GLfloat vertices[] = {
		1,1,0,
		1,-1,0,
		-1,-1,0,
		-1,1,0
	};
		
	

	glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint faces[] = {
		2, 3, 0,
		2, 0, 1
	};
		
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

	vector <vec3> colors;
	colors.push_back(vec3(1,0,0));
	colors.push_back(vec3(0,1,0));
	colors.push_back(vec3(0,0,1));
	colors.push_back(vec3(1,1,1));

	glBindBuffer(GL_ARRAY_BUFFER, floorColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*4, colors.data(), GL_STATIC_DRAW);

	/*static const GLfloat g_color_buffer_data[] = {
     0.583f,  0.771f,  0.014f,
     0.609f,  0.115f,  0.436f,
     0.327f,  0.483f,  0.844f,
     0.822f,  0.569f,  0.201f,
     0.435f,  0.602f,  0.223f,
     0.310f,  0.747f,  0.185f,
     0.597f,  0.770f,  0.761f,
     0.559f,  0.436f,  0.730f,
     0.359f,  0.583f,  0.152f,
     0.483f,  0.596f,  0.789f,
     0.559f,  0.861f,  0.639f,
     0.195f,  0.548f,  0.859f,
     0.014f,  0.184f,  0.576f,
     0.771f,  0.328f,  0.970f,
     0.406f,  0.615f,  0.116f,
     0.676f,  0.977f,  0.133f,
     0.971f,  0.572f,  0.833f,
     0.140f,  0.616f,  0.489f,
     0.997f,  0.513f,  0.064f,
     0.945f,  0.719f,  0.592f,
     0.543f,  0.021f,  0.978f,
     0.279f,  0.317f,  0.505f,
     0.167f,  0.620f,  0.077f,
     0.347f,  0.857f,  0.137f,
     0.055f,  0.953f,  0.042f,
     0.714f,  0.505f,  0.345f,
     0.783f,  0.290f,  0.734f,
     0.722f,  0.645f,  0.174f,
     0.302f,  0.455f,  0.848f,
     0.225f,  0.587f,  0.040f,
     0.517f,  0.713f,  0.338f,
     0.053f,  0.959f,  0.120f,
     0.393f,  0.621f,  0.362f,
     0.673f,  0.211f,  0.457f,
     0.820f,  0.883f,  0.371f,
     0.982f,  0.099f,  0.879f
 };*/
	
	GLfloat squareColors[] = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
		1, 1, 1
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareColors), squareColors, GL_STATIC_DRAW);


}

void RenderingEngine::loadModelViewMatrix()
{
	//M = scale(M, vec3(0.25f));

	//vec3 myRotationAxis(0.0f, 0.0f, 1.0f);
	//M = rotate(M, 0.0f, myRotationAxis);
	//
	M = mat4(1.0f);
	floorM = mat4(1.0f);
	//floorM = scale(floorM, vec3(5.0f));
	//M = translate(M, vec3(0.0f,0.0f,0.0f));

	//V = mat4(1.0f);

	V = glm::lookAt(vec3(0,3,2),	//at (2,0,0)
					vec3(0,0,0),	//looking at (0,0,0)
					vec3(0,1,0)		//right side up
					);

	
}

void RenderingEngine::loadProjectionMatrix()
{
	P = perspective(45.0f, (float)1024/768 , 0.1f, 100.0f);
}

void RenderingEngine::setupModelViewProjectionTransform()
{
	MVP = P * V * M;
}

void RenderingEngine::reloadMVPUniform()
{
	GLint mvpID = glGetUniformLocation(basicProgramID, "MVP");

	glUseProgram(basicProgramID);
	glUniformMatrix4fv( mvpID,
						1,
						GL_FALSE,
						value_ptr(MVP)
						);

}

void RenderingEngine::init()
{
	cout << "Initialize" << endl;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	generateIDs();
	setupVAO();
	loadBuffer();

	loadModelViewMatrix();
	loadProjectionMatrix();
	setupModelViewProjectionTransform();
	//reloadMVPUniform();
}

static const GLfloat tetVertices[] = {
  0.0f, 0.0f, -0.75f, 1.0f,
  0.0f, 0.75f, 0.0f, 1.0f,
  -0.75f, -0.75f, 0.0f, 1.0f,
  0.75f, -0.75f, 0.0f, 1.0f,
};
static const GLushort tetFaceIndices[] = {
  0,1,2,
  0,1,3,
  1,2,3,
  0,2,3
};
static const GLfloat tetColours[] = {
  1.0f,1.0f,1.0f,1.0f,
  1.0f,0.0f,0.0f,1.0f,
  0.0f,1.0f,0.0f,1.0f,
  0.0f,0.0f,1.0f,1.0f
};
static const GLfloat tetNormals[] = {
  0.0f,0.0f,-1.0f, 
  0.0f,1.0f,0.0f,
  -0.707107f, -0.707107f,0.0f,
  0.707107f, -0.707107f,0.0f
};

#define VERTEX_DATA 0
#define VERTEX_COLOR 1
#define VERTEX_NORMAL 2
#define VERTEX_INDICES 3

