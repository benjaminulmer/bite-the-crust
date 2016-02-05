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

	GLint mvpID = glGetUniformLocation(basicProgramID, "MVP");

	for (int i = 0; i < (int)entities.size(); i++) {
		if (!entities[i]->hasRenderable())
			continue;
		M = mat4(1.0f);

		//Translations done here. Order of translations is scale, rotate, translate
		//M = glm::scale(M, vec3(x,y,z))
		//M = glm::rotate(M, 45.0f, vec3(0,1,0));
		M = glm::translate(M, entities[i]->getPosition());
		
		MVP = P * V * M;
		glUniformMatrix4fv( mvpID,
					1,
					GL_FALSE,
					value_ptr(MVP)
					);
		glBindVertexArray(entities[i]->getRenderable()->getVAO());
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
}

void RenderingEngine::loadProjectionMatrix()
{
	P = perspective(45.0f, (float)1024/768 , 0.1f, 100.0f);
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
	loadProjectionMatrix();
}

void RenderingEngine::assignBuffers(Renderable* r) {
	GLuint vertexBuffer;
	GLuint colourBuffer;
	GLuint indexBuffer;
	GLuint vao;


	glGenVertexArrays(1, &vao);			//vao
	glGenBuffers(1, &vertexBuffer);		//vertices vbo
	glGenBuffers(1, &colourBuffer);		//color vbo
	glGenBuffers(1, &indexBuffer);		//faces
	

	vector<vec3> vertices = r->getVertices();
	vector<vec3> colours = r->getColours();
	vector<GLuint> faces = r->getFaces();

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);	//buffering vertex data

	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*colours.size(), colours.data(), GL_STATIC_DRAW);		//buffering color data

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);			//bind faces
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * faces.size(), faces.data(), GL_STATIC_DRAW);

	//r->setVertexVBO(vertexBuffer);
	//r->setColourVBO(colourBuffer);

	//bind to shaders
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
