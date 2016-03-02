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
#include "ContentLoading.h"

#include <ft2build.h>
#include FT_FREETYPE_H

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

	/////////////////////////////////////////////TEXTURE TESTING//////////////////////////////////////////////////////
	void testDraw();
	void testOBJLoading();
	GLuint textureProgramID;
	
	bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
	);

	std::vector <glm::vec3> textureVerts;
	std::vector <glm::vec3> textureNorms;
	std::vector <glm::vec2> textureUVs;

	GLuint Texture;
	GLuint TextureID;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//int init_resourses();
	//void render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy);
	//void displayText();

	void initText2D(const char * texturePath);
	void printText2D(const char * text, int x, int y, int size);
	void cleanupText2D();

	GLuint textProgramID;			//shader
	GLuint textTextureID;			//texture
	GLuint Text2DVertexBufferID;	//vertex buffer
	GLuint Text2DUVBufferID;		//UV buffer

	GLuint basicProgramID;		//shader program 

	GLuint vanVAO;
	GLuint vanVerts;
	GLuint vanUVs;
	GLuint vanNormals;

	std::vector <glm::vec3> colors;
	//std::vector <glm::vec3> vertices;
	std::vector <GLuint> faces;

	glm::mat4 MV;
	glm::mat4 M;
	glm::mat4 V;
	glm::mat4 P;
	glm::mat4 O;

	void renderText();

	//phong testing
	GLuint phongProgramID;
	std::vector <glm::vec3> phongVerts;
	std::vector <glm::vec3> phongNorms;
	std::vector <glm::vec2> phongUVs;

	//stuff for text
	GLuint textVBO;
	GLuint textVAO;
	GLuint texture;
	GLuint sampler;
	GLint attribute_coord;
	GLint uniform_tex;
	GLint uniform_color;

	FT_Library ft;
	FT_Face face;

	struct point {
	GLfloat x;
	GLfloat y;
	GLfloat s;
	GLfloat t;
	};

};

