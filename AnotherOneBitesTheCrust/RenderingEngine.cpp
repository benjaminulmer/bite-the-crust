#include "RenderingEngine.h"
#include "Game.h"
#include <math.h>
#include <iostream>

using namespace std;
using namespace glm;

RenderingEngine::RenderingEngine()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	generateIDs();
	loadProjectionMatrix();
}

RenderingEngine::~RenderingEngine(void) {}

//vehicle dimensions
//x: 2.5, y:2, z:5
void RenderingEngine::displayFunc(vector<Entity*> entities)
{
	glClearDepth(1.0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glUseProgram(phongProgramID);
	GLuint mvpID = glGetUniformLocation(phongProgramID, "MVP");
	GLuint mvID = glGetUniformLocation(phongProgramID, "MV");
	GLuint vID = glGetUniformLocation(phongProgramID, "V");
	GLuint mID = glGetUniformLocation(phongProgramID, "M");


	for (int i = 0; i < (int)entities.size(); i++) {
		if (!entities[i]->hasRenderable())
			continue;
		M = mat4(1.0f);

		//Translations done here. Order of translations is scale, rotate, translate

		//M = glm::scale(M, vec3(x,y,z))
		//M = glm::rotate(M, 45.0f, vec3(0,1,0));
		M = entities[i]->getModelMatrix();
		
		//M = glm::rotate(M, -1.5708f, vec3(0,1,0));

		M = calculateDefaultModel(M, entities[i]);

		mat4 MVP = P * V * M;
		mat4 MV = V * M;
		glUniformMatrix4fv( mvpID,
					1,
					GL_FALSE,
					value_ptr(MVP)
					);
		glUniformMatrix4fv( mvID,
					1,
					GL_FALSE,
					value_ptr(MV)
					);

	//GLint pID = glGetUniformLocation(phongProgramID, "proj_matrix");
		glUniformMatrix4fv( vID,
					1,
					GL_FALSE,
					value_ptr(V)
					);
		glUniformMatrix4fv( mID,
					1,
					GL_FALSE,
					value_ptr(M)
					);

		

		glUniform3f(glGetUniformLocation(phongProgramID, "MaterialColor"), entities[i]->getRenderable()->getColor().x, 
			                                                               entities[i]->getRenderable()->getColor().y, 
																		   entities[i]->getRenderable()->getColor().z);
		glBindVertexArray(entities[i]->getRenderable()->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, entities[i]->getRenderable()->getVertices().size());
		//glDrawElements(GL_TRIANGLES, entities[i]->getRenderable()->getFaces().size(), GL_UNSIGNED_INT, (void*)0);
	}
}

mat4 RenderingEngine::calculateDefaultModel(mat4 model, Entity * entity)
{
	
	//Translations done here. Order of translations is scale, rotate, translate
	
	model = glm::scale(model,entity->getDefaultScale());
	model = glm::rotate(model, entity->getDefaultRotationAngle(), entity->getDefaultRotationAxis());
	model = glm::translate(model, entity->getDefaultTranslation());
	return model;
}

void RenderingEngine::generateIDs()
{
	cout << "Generating IDs" << endl;
	string vsShader = "res\\Shaders\\StandardShading.vertexshader";
	string fsShader = "res\\Shaders\\StandardShading.fragmentshader";
	string vsSource = loadShaderStringfromFile(vsShader);
	string fsSource = loadShaderStringfromFile(fsShader);
	phongProgramID = CreateShaderProgram(vsSource, fsSource);
	glUseProgram(phongProgramID);
	glUniform3f(glGetUniformLocation(phongProgramID, "LightPosition_worldspace"), 35, 100, 35);
}

void RenderingEngine::loadProjectionMatrix()
{
	P = perspective(1.0472f, (float)1024/768 , 1.0f, 1000.0f);	//radians kek
}



void RenderingEngine::updateView(Camera& c)
{
	V = glm::lookAt(c.getPosition(), c.getLookAtPosition(), c.getUpVector());
}


void RenderingEngine::assignBuffers(Renderable* r) 
{
	glUseProgram(phongProgramID);

	GLuint vertexBuffer;
	GLuint normalBuffer;
	//GLuint indexBuffer;
	GLuint vao;


	glGenVertexArrays(1, &vao);			//vao
	glGenBuffers(1, &vertexBuffer);		//vertices vbo
	glGenBuffers(1, &normalBuffer);		//color vbo
	

	vector<vec3> vertices = r->getVertices();
	vector<vec3> normals = r->getNormals();

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);	//buffering vertex data

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);		//buffering color data

	r->setVertexVBO(vertexBuffer);
	r->setColourVBO(normalBuffer);

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
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	r->setVAO(vao);
	glBindVertexArray(0);
	
}


void RenderingEngine::deleteBuffers(Renderable *r)
{
	GLuint vao = r->getVAO();
	GLuint vbuf = r->getVertexVBO();
	GLuint cbuf = r->getColourVBO();
	
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbuf);
	glDeleteBuffers(1, &cbuf);
}


void RenderingEngine::testOBJLoading()
{
	string vsShader = "res\\Shaders\\StandardShading.vertexshader";
	string fsShader = "res\\Shaders\\StandardShading.fragmentshader";
	string vsSource = loadShaderStringfromFile(vsShader);
	string fsSource = loadShaderStringfromFile(fsShader);
	phongProgramID = CreateShaderProgram(vsSource, fsSource);
	glUseProgram(phongProgramID);

	//std::vector<glm::vec3> vertices;
	//std::vector<glm::vec2> uvs;
	//std::vector<glm::vec3> normals; // Won't be used at the moment.
	//std::vector<GLuint> faces;
	bool res = ContentLoading::loadOBJNonIndexed("res\\Models\\FlatFloor.obj", phongVerts, phongNorms);


	cout << "Number of verts " << phongVerts.size() << endl;
	//cout << "NUmber of faces " << phongFaces.size()/3 << endl;
	cout << "Number of normals " << phongNorms.size() << endl;

	glGenVertexArrays(1, &vanVAO);
	glGenBuffers(1, &vanVerts);
	glGenBuffers(1, &vanNormals);
	glGenBuffers(1, &vanIndexBuffer);

	glBindVertexArray(vanVAO);

	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
	glBufferData(GL_ARRAY_BUFFER, phongVerts.size() * sizeof(glm::vec4), phongVerts.data(), GL_STATIC_DRAW);


	//for(int i = 0; i < phongVerts.size(); i++)
	//{
	//	colors.push_back(vec3(1,1,1));
	//}


	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
	glBufferData(GL_ARRAY_BUFFER, phongNorms.size() * sizeof(glm::vec3), phongNorms.data(), GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vanIndexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, phongFaces.size() * sizeof(GLuint), phongFaces.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	
		glBindVertexArray(0);
}