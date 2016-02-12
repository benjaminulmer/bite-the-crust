#include "Renderable.h"


Renderable::Renderable(void)
{
}


Renderable::~Renderable(void)
{
}

void Renderable::addPoint(glm::vec3 vertex, glm::vec3 colour) {
	GLPoint point(vertex, colour);
	points.push_back(point);
}

void Renderable::createFace(GLuint face)
{
	faces.push_back(face);
}

void Renderable::setPoints(std::vector<glm::vec3> vertices)
{
	for(unsigned int i = 0; i<vertices.size(); i++)
	{
		GLPoint point(vertices.at(i), glm::vec3(1,1,1));
		points.push_back(point);
	}
}

void Renderable::setFaces(std::vector<GLuint> face)
{
	faces = face;
}


std::vector<glm::vec3> Renderable::getVertices() {
	std::vector<glm::vec3> vertices;
	for (int i = 0; i < (int)points.size(); i++) {
		vertices.push_back(points[i].vertex);
	}
	return vertices;
}

std::vector<glm::vec3> Renderable::getColours() {
	std::vector<glm::vec3> vertices;
	for (int i = 0; i < (int)points.size(); i++) {
		vertices.push_back(points[i].colour);
	}
	return vertices;
}

std::vector<GLuint> Renderable::getFaces(){
	return faces;
}

std::vector<GLPoint> Renderable::getPoints() {
	return points;
}

void Renderable::setVAO(GLuint v) {
	vao = v;
}

GLuint Renderable::getVAO() {
	return vao;
}

void Renderable::setVertexVBO(GLuint v) {
	vertexVBO = v;
}

GLuint Renderable::getVertexVBO() {
	return vertexVBO;
}

void Renderable::setColourVBO(GLuint v) {
	colourVBO = v;
}

GLuint Renderable::getColourVBO() {
	return colourVBO;
}

int Renderable::getVertexCount() {
	return points.size();
}

glm::vec3 Renderable::getDimensions() {
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
	minX = maxX = points.at(0).vertex.x;
	minY = maxY = points.at(0).vertex.y;
	minZ = maxZ = points.at(0).vertex.z;
	for (unsigned int i = 1; i < points.size(); i++) {
		glm::vec3 v = points.at(i).vertex;
		minX = glm::min(minX, v.x);
		maxX = glm::max(maxX, v.x);
		minY = glm::min(minY, v.y);
		maxY = glm::max(maxY, v.y);
		minZ = glm::min(minZ, v.z);
		maxZ = glm::max(maxZ, v.z);
	}
	return glm::vec3(glm::abs(maxX-minX), glm::abs(maxY-minY), glm::abs(maxZ-minZ));
}