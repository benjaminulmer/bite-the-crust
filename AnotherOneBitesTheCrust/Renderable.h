#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <GL\glew.h>

struct GLPoint
{
	GLPoint(glm::vec3 v, glm::vec3 c) {
		vertex = v;
		colour = c;
	}
	glm::vec3 vertex;
	glm::vec3 colour;
};

class Renderable
{
public:
	Renderable(void);
	~Renderable(void);

	void addPoint(glm::vec3 vertex, glm::vec3 colour);
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec3> getColours();
	std::vector<GLPoint> getPoints();
	void setVAO(GLuint v);
	GLuint getVAO();
	void setVertexVBO(GLuint v);
	GLuint getVertexVBO();
	void setColourVBO(GLuint v);
	GLuint getColourVBO();
	int getVertexCount();
private:
	std::vector<GLPoint> points;
	GLuint vertexVBO, colourVBO;
	GLuint vao;
	// Model
	// Texture
};

