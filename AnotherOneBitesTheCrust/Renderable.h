#pragma once

#include <glm.hpp>
#include <vector>
#include <glew.h>

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
	void createFace(GLuint face);

	void setVerts(std::vector <glm::vec3> vertices);
	void setUVs(std::vector <glm::vec2> UVs);
	void setNorms(std::vector <glm::vec3> normals);
	void setFaces(std::vector <GLuint> faces);
	void setRawVerts(std::vector <glm::vec3> raw);
	//void setColors(std::vector <glm::vec3> colors);


	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getUVs();
	std::vector<glm::vec3> getNormals();
	//std::vector<glm::vec3> getColours();
	std::vector<GLuint> getFaces();
	std::vector<glm::vec3> getRawVerts();
	std::vector<GLPoint> getPoints();

	void setVAO(GLuint v);
	GLuint getVAO();
	void setVertexVBO(GLuint v);
	GLuint getVertexVBO();
	void setColourVBO(GLuint v);
	GLuint getColourVBO();
	int getVertexCount();
	glm::vec3 getDimensions();
	glm::vec3 getCenter();


	void setColor(glm::vec3 c);
	glm::vec3 getColor();
	glm::vec3 getAmbient();

private:
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> norms;
	std::vector<GLuint> faces;
	std::vector<glm::vec3> raw_verts;
	GLuint vertexVBO, colourVBO;
	GLuint vao;
	GLuint indexBuffer;

	glm::vec3 color;
	glm::vec3 ambient_color;
	// Model
	// Texture
};

