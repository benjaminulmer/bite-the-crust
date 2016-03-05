#include "RenderingEngine.h"
#include "Game.h"
#include <math.h>
#include <iostream>

using namespace std;
using namespace glm;

RenderingEngine::RenderingEngine()
{
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_MULTISAMPLE);
	//glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);

	generateIDs();
	loadProjectionMatrix();
}

RenderingEngine::~RenderingEngine(void) {}

//vehicle dimensions
//x: 2.5, y:2, z:5

void RenderingEngine::displayFuncTex(vector<Entity*> entities)
{
	//glClearDepth(1.0);

	glEnable(GL_DEPTH_TEST);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable(GL_BLEND);
	glUseProgram(textureProgramID);
	GLuint mvpID = glGetUniformLocation(textureProgramID, "MVP");
	GLuint vID = glGetUniformLocation(textureProgramID, "V");
	GLuint mID = glGetUniformLocation(textureProgramID, "M");
	GLuint tID = glGetUniformLocation(textureProgramID, "myTextureSampler");
	GLuint normalID = glGetUniformLocation(textureProgramID, "normalMatrix");

	glUniform3f(glGetUniformLocation(textureProgramID, "LightPosition_worldspace"), 35, 90, 35);

	for (int i = 0; i < (int)entities.size(); i++) {
		if (!entities[i]->hasRenderable())
			continue;
		M = mat4(1.0f);

		//Translations done here. Order of translations is scale, rotate, translate
		M = entities[i]->getModelMatrix();
		M = calculateDefaultModel(M, entities[i]);

		mat4 MVP = P * V * M;
		mat4 normal = glm::transpose(glm::inverse(V * M));

		glUniformMatrix4fv(mvpID, 1, GL_FALSE, value_ptr(MVP));
		glUniformMatrix4fv(vID, 1, GL_FALSE, value_ptr(V));
		glUniformMatrix4fv(mID, 1, GL_FALSE, value_ptr(M));
		glUniformMatrix4fv(normalID, 1, GL_FALSE, value_ptr(normal));


		glBindVertexArray(entities[i]->getRenderable()->getVAO());
		GLuint tex = entities[i]->getTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		//glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, 2, GL_RGBA8, 1024, 768, false );

	// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(tID, 0);
		glDrawArrays(GL_TRIANGLES, 0, entities[i]->getRenderable()->getVertices().size());
		glBindVertexArray(0);
		//glDrawElements(GL_TRIANGLES, entities[i]->getRenderable()->getFaces().size(), GL_UNSIGNED_INT, (void*)0);
	}
}

mat4 RenderingEngine::calculateDefaultModel(mat4 model, Entity * entity)
{
	//Translations done here. Order of translations is scale, translate, rotate
	model = glm::scale(model,entity->getDefaultScale());
	model = glm::translate(model, entity->getDefaultTranslation());
	model = glm::rotate(model, entity->getDefaultRotationAngle(), entity->getDefaultRotationAxis());

	return model;
}

void RenderingEngine::generateIDs()
{
	string textVsShader = "res\\Shaders\\TextVertexShader.vertexshader";
	string textFsShader = "res\\Shaders\\TextVertexShader.fragmentshader";
	string textVsSource = loadShaderStringfromFile(textVsShader);
	string textFsSource = loadShaderStringfromFile(textFsShader);
	textProgramID = CreateShaderProgram(textVsSource, textFsSource);
	glUseProgram(textProgramID);

	string texvsShader = "res\\Shaders\\textured-StandardShading.vertexshader";
	string texfsShader = "res\\Shaders\\textured-StandardShading.fragmentshader";
	string texvsSource = loadShaderStringfromFile(texvsShader);
	string texfsSource = loadShaderStringfromFile(texfsShader);
	textureProgramID = CreateShaderProgram(texvsSource, texfsSource);
	glUseProgram(textureProgramID);

	string shadowVsShader = "res\\Shaders\\basic_vs.glsl";
	string shadowFsShader = "res\\Shaders\\basic_fs.glsl";
	string shadowVsSource = loadShaderStringfromFile(shadowVsShader);
	string shadowFsSource = loadShaderStringfromFile(shadowFsShader);
	shadowProgramID = CreateShaderProgram(shadowVsSource, shadowFsSource);
	glUseProgram(shadowProgramID);
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVertBuffer);
	glGenBuffers(1, &sphereColorBuffer);

}

void RenderingEngine::loadProjectionMatrix()
{
	P = perspective(1.0472f, (float)1024/768 , 1.0f, 1000.0f);	//radians kek
	O = glm::ortho(0.0f, 5.0f, 5.0f, 0.0f, 1.0f, 100.0f);
}



void RenderingEngine::updateView(Camera& c)
{
	V = glm::lookAt(c.getPosition(), c.getLookAtPosition(), c.getUpVector());
}

void RenderingEngine::assignBuffersTex(Renderable* r) 
{
	glUseProgram(textureProgramID);

	GLuint vertexBuffer;
	GLuint uvBuffer;
	GLuint normalBuffer;
	//GLuint indexBuffer;
	GLuint vao;

	glGenVertexArrays(1, &vao);			//vao
	glGenBuffers(1, &vertexBuffer);		//vertices vbo
	glGenBuffers(1, &uvBuffer);
	glGenBuffers(1, &normalBuffer);		//color vbo

	vector<vec3> vertices = r->getVertices();
	vector<vec2> uvs = r->getUVs();
	vector<vec3> normals = r->getNormals();

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);	//buffering vertex data

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*uvs.size(), uvs.data(), GL_STATIC_DRAW);	//buffering uv data

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);		//buffering normal data

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
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(
		2,
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


void RenderingEngine::initText2D(const char * texturePath){

	// Initialize texture
	textTextureID = ContentLoading::loadDDS(texturePath);
}

void RenderingEngine::printText2D(const char * text, int x, int y, int size){


	unsigned int length = strlen(text);

	// Fill buffers
	std::vector<glm::vec2> textvertices;
	std::vector<glm::vec2> textUVs;
	for ( unsigned int i=0 ; i<length ; i++ ){
		
		glm::vec2 vertex_up_left    = glm::vec2( x+i*size     , y+size );
		glm::vec2 vertex_up_right   = glm::vec2( x+i*size+size, y+size );
		glm::vec2 vertex_down_right = glm::vec2( x+i*size+size, y      );
		glm::vec2 vertex_down_left  = glm::vec2( x+i*size     , y      );

		textvertices.push_back(vertex_up_left   );
		textvertices.push_back(vertex_down_left );
		textvertices.push_back(vertex_up_right  );

		textvertices.push_back(vertex_down_right);
		textvertices.push_back(vertex_up_right);
		textvertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character%16)/16.0f;
		float uv_y = (character/16)/16.0f;

		glm::vec2 uv_up_left    = glm::vec2( uv_x           , uv_y );
		glm::vec2 uv_up_right   = glm::vec2( uv_x+1.0f/16.0f, uv_y );
		glm::vec2 uv_down_right = glm::vec2( uv_x+1.0f/16.0f, (uv_y + 1.0f/16.0f) );
		glm::vec2 uv_down_left  = glm::vec2( uv_x           , (uv_y + 1.0f/16.0f) );
		textUVs.push_back(uv_up_left   );
		textUVs.push_back(uv_down_left );
		textUVs.push_back(uv_up_right  );

		textUVs.push_back(uv_down_right);
		textUVs.push_back(uv_up_right);
		textUVs.push_back(uv_down_left);
	}
	//GLuint textVAO;
	glGenVertexArrays(1, &textVAO);	
	glBindVertexArray(textVAO);

	glGenBuffers(1, &Text2DVertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, textvertices.size() * sizeof(glm::vec2), textvertices.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &Text2DUVBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, textUVs.size() * sizeof(glm::vec2), textUVs.data(), GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(textProgramID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textTextureID);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(glGetUniformLocation( textProgramID, "myTextureSampler" ), 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_CULL_FACE);
   // glDisable(GL_DEPTH_TEST);

	// Draw call
	glBindVertexArray(textVAO);
	glDrawArrays(GL_TRIANGLES, 0, textvertices.size() );

	glDisable(GL_BLEND);
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteVertexArrays(1, &textVAO);
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteBuffers(1, &Text2DUVBufferID);

}

void RenderingEngine::setupShadowBuffers()
{

	glUseProgram(shadowProgramID);

	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	std::vector<GLuint> faces;
	std::vector<glm::vec3> raw_verts;

	bool res = ContentLoading::loadOBJ("res\\Models\\blobShadow.obj", shadowVertices, uvs, normals, faces, raw_verts);

	glBindVertexArray(sphereVAO);

	glBindBuffer(GL_ARRAY_BUFFER, sphereVertBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3) * shadowVertices.size(),	// byte size of Vec3f, 4 of them
		shadowVertices.data(),		// pointer (Vec3f*) to contents of verts
		GL_STATIC_DRAW);	// Usage pattern of GPU buffer

							// RGB values for the 4 vertices of the quad
	vector <float> colors;

	for (unsigned int i = 0; i < shadowVertices.size(); i++)
	{
		colors.push_back(0.2f);
		colors.push_back(0.2f);
		colors.push_back(0.2f);
	}

	glBindBuffer(GL_ARRAY_BUFFER, sphereColorBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(float)*colors.size(),
		colors.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // match layout # in shader
	glBindBuffer(GL_ARRAY_BUFFER, sphereVertBuffer);
	glVertexAttribPointer(
		0,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
		);

	glEnableVertexAttribArray(1); // match layout # in shader
	glBindBuffer(GL_ARRAY_BUFFER, sphereColorBuffer);
	glVertexAttribPointer(
		1,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
		);

	glBindVertexArray(0); // reset to default		
}

void RenderingEngine::drawShadow(glm::vec3 position)
{
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shadowProgramID);

	// Use VAO that holds buffer bindings
	// and attribute config of buffers
	glBindVertexArray(sphereVAO);
	// Draw Quads, start at vertex 0, draw 4 of them (for a quad)
	GLint mvpID = glGetUniformLocation(shadowProgramID, "MVP");


	M = mat4(1.0f);
	M = translate(M, glm::vec3(position.x, 0.01, position.z));
	mat4 MVP = P * V * M;

	glUniformMatrix4fv(mvpID,		// ID
		1,		// only 1 matrix
		GL_FALSE,	// transpose matrix, Mat4f is row major
		value_ptr(MVP)	// pointer to data in Mat4f
		);
	glDrawArrays(GL_TRIANGLES, 0, shadowVertices.size());
	glBindVertexArray(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TESTING STUFF BELOW
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//void RenderingEngine::testDraw() {
//	// Draw all our entities
//
//	//glClearDepth(1.0);
//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glUseProgram(textureProgramID);
//	GLuint mvpID = glGetUniformLocation(textureProgramID, "MVP");
//
//	GLuint vID = glGetUniformLocation(textureProgramID, "V");
//	GLuint mID = glGetUniformLocation(textureProgramID, "M");
//
//	M = mat4(1.0f);
//	mat4 MVP = P * V * M;
//
//	glUniformMatrix4fv( mvpID,
//					1,
//					GL_FALSE,
//					value_ptr(MVP)
//					);
//
//	//GLint pID = glGetUniformLocation(phongProgramID, "proj_matrix");
//	glUniformMatrix4fv( vID,
//					1,
//					GL_FALSE,
//					value_ptr(V)
//					);
//
//	glUniformMatrix4fv( mID,
//					1,
//					GL_FALSE,
//					value_ptr(M)
//					);
//
//	glUniform3f(glGetUniformLocation(textureProgramID, "LightPosition_worldspace"), 35, 100, 35);
//
//	glBindVertexArray(vanVAO);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, Texture);
//	// Set our "myTextureSampler" sampler to user Texture Unit 0
//	glUniform1i(TextureID, 0);
//
//	glDrawArrays(GL_TRIANGLES, 0, textureVerts.size());
//
//	//glDrawElements(GL_TRIANGLES, phongFaces.size(), GL_UNSIGNED_INT, (void*)0);
//}
//
//
//
//void RenderingEngine::testOBJLoading()
//{
//	string vsShader = "res\\Shaders\\textured-StandardShading.vertexshader";
//	string fsShader = "res\\Shaders\\textured-StandardShading.fragmentshader";
//	string vsSource = loadShaderStringfromFile(vsShader);
//	string fsSource = loadShaderStringfromFile(fsShader);
//	textureProgramID = CreateShaderProgram(vsSource, fsSource);
//	glUseProgram(textureProgramID);
//
//	//std::vector<glm::vec3> vertices;
//	//std::vector<glm::vec2> uvs;
//	//std::vector<glm::vec3> normals; // Won't be used at the moment.
//	//std::vector<GLuint> faces;
//	bool res = ContentLoading::loadOBJ("res\\Models\\bigramp.obj", textureVerts, textureUVs, textureNorms);
//	Texture = ContentLoading::loadDDS("res\\Textures\\ramp.DDS");
//
//	cout << "Number of verts " << textureVerts.size() << endl;
//	cout << "Number of UVs " << textureUVs.size() << endl;
//	cout << "Number of normals " << textureNorms.size() << endl;
//
//	glGenVertexArrays(1, &vanVAO);
//	glBindVertexArray(vanVAO);
//
//
//	glGenBuffers(1, &vanVerts);
//	glGenBuffers(1, &vanUVs);
//	glGenBuffers(1, &vanNormals);
//
//	
//	TextureID = glGetUniformLocation(textureProgramID, "myTextureSampler");
//
//
//	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
//	glBufferData(GL_ARRAY_BUFFER, textureVerts.size() * sizeof(glm::vec3), textureVerts.data(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vanUVs);
//	glBufferData(GL_ARRAY_BUFFER, textureUVs.size() * sizeof(glm::vec2), textureUVs.data(), GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
//	glBufferData(GL_ARRAY_BUFFER, textureNorms.size() * sizeof(glm::vec3), textureNorms.data(), GL_STATIC_DRAW);
//
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
//	glBindBuffer(GL_ARRAY_BUFFER, vanUVs);
//	glVertexAttribPointer(
//		1,
//		2,
//		GL_FLOAT,
//		GL_FALSE,
//		0,
//		(void*)0);
//	
//	glEnableVertexAttribArray(2);
//	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
//	glVertexAttribPointer(
//		2,
//		3,
//		GL_FLOAT,
//		GL_FALSE,
//		0,
//		(void*)0);
//	
//
//	glBindVertexArray(0);
//}
