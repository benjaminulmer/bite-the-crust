#include "Renderable.h"


Renderable::Renderable(void)
{
	vao = 0;
	vertexVBO = colourVBO = 0;
	indexBuffer = 0;
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

void Renderable::setVerts(std::vector<glm::vec3> vertices)
{
	verts = vertices;
}

void Renderable::setUVs(std::vector<glm::vec2> UVs)
{
	uvs = UVs;
}

void Renderable::setNorms(std::vector <glm::vec3> normals)
{
	norms = normals;
}

void Renderable::setFaces(std::vector<GLuint> face)
{
	faces = face;
}


std::vector<glm::vec3> Renderable::getVertices() 
{
	return verts;
}

//std::vector<glm::vec3> Renderable::getColours() {
//	std::vector<glm::vec3> vertices;
//	for (int i = 0; i < (int)points.size(); i++) {
//		vertices.push_back(points[i].colour);
//	}
//	return vertices;
//}

std::vector<glm::vec2> Renderable::getUVs()
{
	return uvs;
}

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

int Renderable::getVertexCount() {
	return verts.size();
}

glm::vec3 Renderable::getDimensions()
{
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
	minX = maxX = verts.at(0).x;
	minY = maxY = verts.at(0).y;
	minZ = maxZ = verts.at(0).z;
	for (unsigned int i = 1; i < verts.size(); i++) {
		glm::vec3 v = verts.at(i);
		minX = glm::min(minX, v.x);
		maxX = glm::max(maxX, v.x);
		minY = glm::min(minY, v.y);
		maxY = glm::max(maxY, v.y);
		minZ = glm::min(minZ, v.z);
		maxZ = glm::max(maxZ, v.z);
	}
	return glm::vec3(glm::abs(maxX-minX), glm::abs(maxY-minY), glm::abs(maxZ-minZ));
}


glm::vec3 Renderable::getCenter()
{
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
	minX = maxX = verts.at(0).x;
	minY = maxY = verts.at(0).y;
	minZ = maxZ = verts.at(0).z;
	for (unsigned int i = 1; i < verts.size(); i++) {
		glm::vec3 v = verts.at(i);
		minX = glm::min(minX, v.x);
		maxX = glm::max(maxX, v.x);
		minY = glm::min(minY, v.y);
		maxY = glm::max(maxY, v.y);
		minZ = glm::min(minZ, v.z);
		maxZ = glm::max(maxZ, v.z);
	}

	glm::vec3 max(maxX, maxY, maxZ);
	glm::vec3 min(minX, minY, minZ);

	glm::vec3 center = max + min;
	center.x /= 2;
	center.y /= 2;
	center.z /= 2;

	center = -center;

	return center;
}


glm::vec3 Renderable::getColor()
{
	return color;
}

glm::vec3 Renderable::getAmbient()
{
	return ambient_color;
}

void Renderable::setColor(glm::vec3 c)
{
	color = c;
	ambient_color = c/glm::vec3(10.0f);
}
