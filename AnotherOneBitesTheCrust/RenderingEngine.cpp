#include "RenderingEngine.h"
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



void RenderingEngine::displayFunc(vector<Entity*> entities)
{

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	M = mat4(1.0f);
	
	MVP = P * V * M;

	GLint mvpID = glGetUniformLocation(basicProgramID, "MVP");

	glUniformMatrix4fv( mvpID,
						1,
						GL_FALSE,
						value_ptr(MVP)
						);
	glBindVertexArray(vaoID);
	glDrawArrays(GL_TRIANGLES, 0, 12*3);

	/*floorM = scale(M, vec3(2.0f));
	floorM = rotate(floorM, 45.0f, vec3(0,1,0));

	//scale rotate translate

	MVP = P * V * floorM;

	glUniformMatrix4fv( mvpID,
						1,
						GL_FALSE,
						value_ptr(MVP)
						);
	glBindVertexArray(floorID);
	glDrawArrays(GL_QUADS, 0, 4);*/

	for (int i = 0; i < (int)entities.size(); i++) {
		if (!entities[i]->hasRenderable())
			continue;
		M = mat4(1.0f);
		M = glm::translate(M, entities[i]->getPosition());
		M = glm::rotate(M, 45.0f, vec3(0,1,0));
		MVP = P * V * M;
		glUniformMatrix4fv( mvpID,
					1,
					GL_FALSE,
					value_ptr(MVP)
					);
		glBindVertexArray(entities[i]->getRenderable()->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, entities[i]->getRenderable()->getVertexCount());
	}

}

void RenderingEngine::generateIDs()
{
	cout << "Generating IDs" << endl;
	string vsShader = "res\\Shaders\\basic_vs.glsl";
	string fsShader = "res\\Shaders\\basic_fs.glsl";
	string vsSource = loadShaderStringfromFile(vsShader);
	string fsSource = loadShaderStringfromFile(fsShader);
	basicProgramID = CreateShaderProgram(vsSource, fsSource);
	glUseProgram(basicProgramID);

	glGenVertexArrays(1, &vaoID);	//generate VAO
	glGenBuffers(1, &vertBufferID);	//generate buffer for vertices
	glGenBuffers(1, &colorBufferID);	//generate buffer for colors

	glGenVertexArrays(1, &floorID);
	glGenBuffers(1, &floorBuffer);
	glGenBuffers(1, &floorColorBuffer);
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

	static const GLfloat g_vertex_buffer_data[] = {
      -1.0f,-1.0f,-1.0f, // triangle 1 : begin
      -1.0f,-1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f, // triangle 1 : end
      1.0f, 1.0f,-1.0f, // triangle 2 : begin
      -1.0f,-1.0f,-1.0f,
      -1.0f, 1.0f,-1.0f, // triangle 2 : end
     1.0f,-1.0f, 1.0f,
     -1.0f,-1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     -1.0f,-1.0f,-1.0f,
     -1.0f,-1.0f,-1.0f,
     -1.0f, 1.0f, 1.0f,
     -1.0f, 1.0f,-1.0f,
     1.0f,-1.0f, 1.0f,
     -1.0f,-1.0f, 1.0f,
     -1.0f,-1.0f,-1.0f,
     -1.0f, 1.0f, 1.0f,
     -1.0f,-1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f,-1.0f,
     1.0f,-1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     1.0f, 1.0f,-1.0f,
     -1.0f, 1.0f,-1.0f,
     1.0f, 1.0f, 1.0f,
     -1.0f, 1.0f,-1.0f,
     -1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f,
     -1.0f, 1.0f, 1.0f,
     1.0f,-1.0f, 1.0f
 };
	

	glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	vector <vec3> colors;
	colors.push_back(vec3(1,0,0));
	colors.push_back(vec3(0,1,0));
	colors.push_back(vec3(0,0,1));
	colors.push_back(vec3(1,1,1));

	glBindBuffer(GL_ARRAY_BUFFER, floorColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*4, colors.data(), GL_STATIC_DRAW);

	static const GLfloat g_color_buffer_data[] = {
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
 };
	
	
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
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

	V = glm::lookAt(vec3(4,3,3),	//at (2,0,0)
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

	//glUseProgram(basicProgramID);
	glUniformMatrix4fv( mvpID,
						1,
						GL_FALSE,
						value_ptr(MVP)
						);

}

void RenderingEngine::updateView(Camera& c) {
	V = glm::lookAt(c.getPosition(), c.getLookAtPosition(), c.getUpVector());
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

void RenderingEngine::assignBuffers(Renderable* r) {
	GLuint vertexBuffer;
	GLuint colourBuffer;
	GLuint vao;
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &colourBuffer);
	glGenVertexArrays(1, &vao);
	vector<vec3> vertices = r->getVertices();
	vector<vec3> colours = r->getColours();
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*colours.size(), colours.data(), GL_STATIC_DRAW);
	r->setVertexVBO(vertexBuffer);
	r->setColourVBO(colourBuffer);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	glBindVertexArray(0);
	r->setVAO(vao);
}

void RenderingEngine::deleteBuffers(Renderable *r) {
	GLuint vao = r->getVAO();
	GLuint vbuf = r->getVertexVBO();
	GLuint cbuf = r->getColourVBO();
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbuf);
	glDeleteBuffers(1, &cbuf);
}