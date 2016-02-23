/*
* The class responsible for rendering functions. Implemented as a singleton for the sake of GLUT 
  needing a function callback to display.
*/

#pragma once
#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/transform.hpp>

#include <glew.h>

#include "res_path.h"
#include "ShaderTools.h"
#include "Renderable.h"
#include "Camera.h"

class RenderingEngine
{
public:
	RenderingEngine(void);
	~RenderingEngine(void);	

	void displayFunc(std::vector<Entity*> entities);
	glm::mat4 calculateDefaultModel(glm::mat4 model, Entity * entity);

	void generateIDs();
	void deleteIDs();
	void loadProjectionMatrix();
	void updateView(Camera& c);

	void assignBuffers(Renderable* r);
	void deleteBuffers(Renderable* r);

	void testOBJLoading();

	GLuint basicProgramID;		//shader program 

	GLuint vanVAO;
	GLuint vanVerts;
	GLuint vanColors;
	GLuint vanNormals;
	GLuint vanIndexBuffer;

	std::vector <glm::vec3> colors;
	//std::vector <glm::vec3> vertices;
	std::vector <GLuint> faces;

	glm::mat4 MV;
	glm::mat4 M;
	glm::mat4 V;
	glm::mat4 P;

	//phong testing
	GLuint phongProgramID;
	std::vector <glm::vec3> phongVerts;
	std::vector <glm::vec3> phongNorms;
	std::vector <GLuint> phongFaces;
	std::vector <glm::vec2> phongUVs;
};

