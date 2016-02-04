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


	//glBindVertexArray(floorID);
	//glDrawArrays(GL_QUADS, 0, 4);

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
		//glDrawArrays(GL_TRIANGLES, 0, entities[i]->getRenderable()->getVertexCount());
		glDrawElements(GL_TRIANGLES, entities[i]->getRenderable()->getFaces().size(), GL_UNSIGNED_INT, (void*)0);
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
	GLuint indexBuffer;
	GLuint vao;
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &colourBuffer);
	glGenBuffers(1, &indexBuffer);
	glGenVertexArrays(1, &vao);
	vector<vec3> vertices = r->getVertices();
	vector<vec3> colours = r->getColours();
	vector<GLuint> faces = r->getFaces();

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

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * faces.size(), faces.data(), GL_STATIC_DRAW);

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
