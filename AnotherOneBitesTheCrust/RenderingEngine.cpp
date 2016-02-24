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
	glDisable(GL_BLEND);
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
		M = entities[i]->getModelMatrix();
		M = calculateDefaultModel(M, entities[i]);

		mat4 MVP = P * V * M;
		mat4 MV = V * M;
		glUniformMatrix4fv(mvpID, 1, GL_FALSE, value_ptr(MVP));
		glUniformMatrix4fv(mvID, 1, GL_FALSE, value_ptr(MV));
		glUniformMatrix4fv(vID, 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(mID, 1, GL_FALSE, value_ptr(M));

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

	string textVsShader = "res\\Shaders\\text_vs.glsl";
	string textFsShader = "res\\Shaders\\text_fs.glsl";
	string textVsSource = loadShaderStringfromFile(textVsShader);
	string textFsSource = loadShaderStringfromFile(textFsShader);
	textProgramID = CreateShaderProgram(textVsSource, textFsSource);
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

//
//void RenderingEngine::testOBJLoading()
//{
//	string vsShader = "res\\Shaders\\StandardShading.vertexshader";
//	string fsShader = "res\\Shaders\\StandardShading.fragmentshader";
//	string vsSource = loadShaderStringfromFile(vsShader);
//	string fsSource = loadShaderStringfromFile(fsShader);
//	phongProgramID = CreateShaderProgram(vsSource, fsSource);
//	glUseProgram(phongProgramID);
//
//	//std::vector<glm::vec3> vertices;
//	//std::vector<glm::vec2> uvs;
//	//std::vector<glm::vec3> normals; // Won't be used at the moment.
//	//std::vector<GLuint> faces;
//	bool res = ContentLoading::loadOBJNonIndexed("res\\Models\\FlatFloor.obj", phongVerts, phongNorms);
//
//
//	cout << "Number of verts " << phongVerts.size() << endl;
//	//cout << "NUmber of faces " << phongFaces.size()/3 << endl;
//	cout << "Number of normals " << phongNorms.size() << endl;
//
//	glGenVertexArrays(1, &vanVAO);
//	glGenBuffers(1, &vanVerts);
//	glGenBuffers(1, &vanNormals);
//	glGenBuffers(1, &vanIndexBuffer);
//
//	glBindVertexArray(vanVAO);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
//	glBufferData(GL_ARRAY_BUFFER, phongVerts.size() * sizeof(glm::vec4), phongVerts.data(), GL_STATIC_DRAW);
//
//
//	//for(int i = 0; i < phongVerts.size(); i++)
//	//{
//	//	colors.push_back(vec3(1,1,1));
//	//}
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
//	glBufferData(GL_ARRAY_BUFFER, phongNorms.size() * sizeof(glm::vec3), phongNorms.data(), GL_STATIC_DRAW);
//
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vanIndexBuffer);
//	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, phongFaces.size() * sizeof(GLuint), phongFaces.data(), GL_STATIC_DRAW);
//
//	glEnableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
//	glVertexAttribPointer(
//		0,
//		3,
//		GL_FLOAT,
//		GL_FALSE,
//		0,
//		(void*)0);
//
//	glEnableVertexAttribArray(1);
//	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
//	glVertexAttribPointer(
//		1,
//		3,
//		GL_FLOAT,
//		GL_FALSE,
//		0,
//		(void*)0);
//	
//		glBindVertexArray(0);
//}



int RenderingEngine::init_resourses()
{

	cout << "Initializing text resourses " << endl;

	if(FT_Init_FreeType(&ft))
	{
		cout << "Could not init freetype library " << endl;
		return 0;
	}

	if(FT_New_Face(ft, "res\\Fonts\\CodePredators-Regular.ttf", 0, &face))
	{
		cout << "Could not open font" << endl;
		return 0;
	}

	
	GLuint attribute_coord = glGetUniformLocation(textProgramID, "coord");
	GLuint uniform_tex = glGetUniformLocation(textProgramID, "tex");
	GLuint uniform_color = glGetUniformLocation(textProgramID, "color");

	glGenBuffers(1, &textVBO);
	glGenVertexArrays(1, &textVAO);
    glGenTextures(1, &texture);
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	cout << "Finished initializing text resourses" << endl;
	return 1;
}

void RenderingEngine::render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    const FT_GlyphSlot glyph = face->glyph;

    for(auto c : str) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            continue;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     glyph->bitmap.width, glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

        const float vx = x + glyph->bitmap_left * sx;
        const float vy = y + glyph->bitmap_top * sy;
        const float w = glyph->bitmap.width * sx;
        const float h = glyph->bitmap.rows * sy;

        struct {
            float x, y, s, t;
        } data[6] = {
            {vx    , vy    , 0, 0},
            {vx    , vy - h, 0, 1},
            {vx + w, vy    , 1, 0},
            {vx + w, vy    , 1, 0},
            {vx    , vy - h, 0, 1},
            {vx + w, vy - h, 1, 1}
        };

        glBufferData(GL_ARRAY_BUFFER, 24*sizeof(float), data, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (glyph->advance.x >> 6) * sx;
        y += (glyph->advance.y >> 6) * sy;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void RenderingEngine::displayText() {

	//glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	float sx = 2.0 / 1024;
	float sy = 2.0 / 768;
	glUseProgram(textProgramID);
    glBindAttribLocation(textProgramID, 0, "in_Position");
    GLuint texUniform = glGetUniformLocation(textProgramID, "tex");
    GLuint colorUniform = glGetUniformLocation(textProgramID, "color");
	
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindSampler(0, sampler);
    glBindVertexArray(textVAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glUseProgram(textProgramID);
    glUniform4f(colorUniform, 0, 0, 0, 1);
    glUniform1i(texUniform, 0);

    FT_Set_Pixel_Sizes(face, 0, 30);
    render_text("Another One Bites The Crust", face, -1, 0.93, sx, sy);
	//render_text("Speed:", face, 0.6, -1, sx, sy);
	
}
