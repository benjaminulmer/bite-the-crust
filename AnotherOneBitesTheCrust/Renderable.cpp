#include "Renderable.h"


Renderable::Renderable(void)
{
}


Renderable::~Renderable(void)
{
}

void Renderable::addPoint(glm::vec3 vertex, glm::vec3 colour) {
	GLPoint point(vertex, colour);
	//points.push_back(point);
}

void Renderable::createFace(GLuint face)
{
	faces.push_back(face);
}

void Renderable::setVerts(std::vector<glm::vec4> vertices)
{
	verts = vertices;
}

void Renderable::setNorms(std::vector <glm::vec3> normals)
{
	norms = normals;
}

void Renderable::setFaces(std::vector<GLuint> face)
{
	faces = face;
}


std::vector<glm::vec4> Renderable::getVertices() {
	return verts;
}

//std::vector<glm::vec3> Renderable::getColours() {
//	std::vector<glm::vec3> vertices;
//	for (int i = 0; i < (int)points.size(); i++) {
//		vertices.push_back(points[i].colour);
//	}
//	return vertices;
//}

std::vector<glm::vec3> Renderable::getNormals()
{
	return norms;
}

std::vector<GLuint> Renderable::getFaces(){
	return faces;
}

//std::vector<GLPoint> Renderable::getPoints() {
//	return points;
//}

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

//int Renderable::getVertexCount() {
//	return points.size();
//}