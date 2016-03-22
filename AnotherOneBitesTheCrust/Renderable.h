#pragma once
#include <glm.hpp>
#include <vector>
#include <glew.h>

class Renderable
{
public:
	Renderable(void);
	~Renderable(void);

	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> norms;
	std::vector<unsigned short> drawFaces;
	std::vector<GLuint> faces;
	std::vector<glm::vec3> raw_verts;
	GLuint vbo;
	GLuint vao;
	GLuint indexBuffer;

	glm::vec3 getDimensions();
};

