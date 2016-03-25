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
	//glEnable(GL_CULL_FACE);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable(GL_BLEND);
	glUseProgram(textureProgramID);


	glUniform3f(lightPos, 100.0f, 100.0f, 100.0f);
	glUniform1f(lightPow, 5000.0f);
	glUniform3f(ambientScale, 0.6f, 0.6f, 0.6f);

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

		glBindVertexArray(entities[i]->getRenderable()->vao);
		GLuint tex = entities[i]->getTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		//glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, 2, GL_RGBA8, 1024, 768, false );

	// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(tID, 0);
		//glDrawArrays(GL_TRIANGLES, 0, entities[i]->getRenderable()->verts.size());
		
		glDrawElements(GL_TRIANGLES, entities[i]->getRenderable()->drawFaces.size(), GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(0);
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
	colorID = glGetUniformLocation(textProgramID, "color");

	string texvsShader = "res\\Shaders\\textured-StandardShading.vertexshader";
	string texfsShader = "res\\Shaders\\textured-StandardShading.fragmentshader";
	string texvsSource = loadShaderStringfromFile(texvsShader);
	string texfsSource = loadShaderStringfromFile(texfsShader);
	textureProgramID = CreateShaderProgram(texvsSource, texfsSource);
	glUseProgram(textureProgramID);
	//get shader location
	mvpID = glGetUniformLocation(textureProgramID, "MVP");
	vID = glGetUniformLocation(textureProgramID, "V");
	mID = glGetUniformLocation(textureProgramID, "M");
	tID = glGetUniformLocation(textureProgramID, "myTextureSampler");
	normalID = glGetUniformLocation(textureProgramID, "normalMatrix");
	lightPos = glGetUniformLocation(textureProgramID, "LightPosition_worldspace");
	lightPow = glGetUniformLocation(textureProgramID, "LightPower");
	ambientScale = glGetUniformLocation(textureProgramID, "AmbientScale");

	string shadowVsShader = "res\\Shaders\\basic_vs.glsl";
	string shadowFsShader = "res\\Shaders\\basic_fs.glsl";
	string shadowVsSource = loadShaderStringfromFile(shadowVsShader);
	string shadowFsSource = loadShaderStringfromFile(shadowFsShader);
	basicProgramID = CreateShaderProgram(shadowVsSource, shadowFsSource);
	glUseProgram(basicProgramID);
	glGenVertexArrays(1, &sphereVAO);
	glGenBuffers(1, &sphereVertBuffer);
	glGenBuffers(1, &sphereColorBuffer);
	glGenVertexArrays(1, &mmRoadVAO);
	glGenBuffers(1, &mmRoadVertBuffer);
	glGenBuffers(1, &mmRoadColorBuffer);
	glGenVertexArrays(1, &mmHouseVAO);
	glGenBuffers(1, &mmHouseVertBuffer);
	glGenBuffers(1, &mmHouseColorBuffer);
	for (int i = 0; i < 4; i++) {
		glGenVertexArrays(1, &mmVanVAOs[i]);
		glGenBuffers(1, &mmVanColorBuffers[i]);
	}
	glGenBuffers(1, &mmVanVertBuffer);
	
	glGenVertexArrays(1, &nodeVAO);
	glGenBuffers(1, &nodeVertBuffer);
	glGenBuffers(1, &nodeColorBuffer);
	basicmvpID = glGetUniformLocation(basicProgramID, "MVP");


}

void RenderingEngine::loadProjectionMatrix()
{
	P = perspective(1.0472f, (float)1280/(float)720, 1.0f, 1000.0f);	//radians kek
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
	GLuint indexBuffer;
	GLuint vao;

	glGenVertexArrays(1, &vao);			//vao
	glGenBuffers(1, &vertexBuffer);		//vertices vbo
	glGenBuffers(1, &uvBuffer);
	glGenBuffers(1, &normalBuffer);		//color vbo
	glGenBuffers(1, &indexBuffer);

	vector<vec3> vertices = r->verts;
	vector<vec2> uvs = r->uvs;
	vector<vec3> normals = r->norms;
	vector<unsigned short> faces = r->drawFaces;

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vertices.size(), vertices.data(), GL_STATIC_DRAW);	//buffering vertex data

	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*uvs.size(), uvs.data(), GL_STATIC_DRAW);	//buffering uv data

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);		//buffering normal data

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * faces.size(), faces.data(), GL_STATIC_DRAW);

	r->vbo = vertexBuffer;
	//r->setColourVBO(normalBuffer);

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

	r->vao = vao;
	glBindVertexArray(0);
}


void RenderingEngine::deleteBuffers(Renderable *r)
{
	GLuint vao = r->vao;
	GLuint vbuf = r->vbo;
	
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbuf);
}


void RenderingEngine::initText2D(const char * texturePath){

	// Initialize texture
	textTextureID = ContentLoading::loadDDS(texturePath);
}

void RenderingEngine::printText2D(const char * text, int x, int y, int size)
{
	//printText2Doutline(text, x+1, y+1, (int)(size+0.5f), glm::vec4(0,0,0,1));
	printText2Doutline(text, x, y, size, glm::vec4(1,1,1,1));
}

void RenderingEngine::printText2Doutline(const char * text, int x, int y, int size, glm::vec4 color){


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
	//glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);
	//glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

	glUniform4f(colorID, color.x, color.y, color.z, color.a);

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


void RenderingEngine::setupMiscBuffers()
{

//loading sky
	glUseProgram(textureProgramID);
	glGenBuffers(1, &skyVertBuffer);		//vertices vbo
	glGenBuffers(1, &skyUVBuffer);
	glGenBuffers(1, &skyNormalBuffer);		//color vbo

	std::vector<GLuint> skyfaces;
	std::vector<glm::vec3> skyraw_verts;

	ContentLoading::loadOBJ("res\\Models\\Skybox.obj", skyVertices, skyUVs, skyNorms, skyfaces, skyraw_verts);

	glGenVertexArrays(1, &skyVAO);
	glBindVertexArray(skyVAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyVertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*skyVertices.size(), skyVertices.data(), GL_STATIC_DRAW);	//buffering vertex data

	glBindBuffer(GL_ARRAY_BUFFER, skyUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)*skyUVs.size(), skyUVs.data(), GL_STATIC_DRAW);	//buffering uv data

	glBindBuffer(GL_ARRAY_BUFFER, skyNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*skyNorms.size(), skyNorms.data(), GL_STATIC_DRAW);		//buffering normal data

	//bind to shaders
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, skyVertBuffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, skyUVBuffer);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, skyNormalBuffer);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	skyTex = ContentLoading::loadDDS("res\\Textures\\Skybox.DDS");

//Loading shadows
	glUseProgram(basicProgramID);

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
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void RenderingEngine::drawShadow(glm::vec3 position)
{
	//glEnable(GL_DEPTH_TEST);
	//drawing shadow
		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glUseProgram(basicProgramID);

	// Use VAO that holds buffer bindings
	// and attribute config of buffers
	glBindVertexArray(sphereVAO);
	// Draw Quads, start at vertex 0, draw 4 of them (for a quad)
	

	M = mat4(1.0f);
	M = translate(M, glm::vec3(position.x, 0.01, position.z));
	mat4 MVP = P * V * M;

	glUniformMatrix4fv(basicmvpID,		// ID
		1,		// only 1 matrix
		GL_FALSE,	// transpose matrix, Mat4f is row major
		value_ptr(MVP)	// pointer to data in Mat4f
		);
	glDrawArrays(GL_TRIANGLES, 0, shadowVertices.size());
	glBindVertexArray(0);

}

void RenderingEngine::drawSkybox(glm::vec3 position)
{
		//Drawing skybox
	glUseProgram(textureProgramID);

	glUniform3f(lightPos, 35.0f, 90.0f, 35.0f);
	glUniform1f(lightPow, 0.0f);
	glUniform3f(ambientScale, 1, 1, 1);


	M = mat4(1.0f);
	
	M = translate(M, position);
	M = scale(M, vec3(1.75));
	mat4 MVP = P * V * M;

	glUniformMatrix4fv(mvpID, 1, GL_FALSE, value_ptr(MVP));
	glUniformMatrix4fv(vID, 1, GL_FALSE, value_ptr(V));
	glUniformMatrix4fv(mID, 1, GL_FALSE, value_ptr(M));


	glBindVertexArray(skyVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyTex);
		//glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, 2, GL_RGBA8, 1024, 768, false );

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(tID, 0);
	glDrawArrays(GL_TRIANGLES, 0, skyVertices.size());
		
	glBindVertexArray(0);
}


void RenderingEngine::setupMinimap(Map map)
{
	glUseProgram(basicProgramID);

	for(unsigned int i = 0; i < map.tiles.size(); i++)
	{
		for(unsigned int j = 0; j < map.tiles[i].size(); j++)
		{
			Tile* tile = &map.tiles[i][j];

			//cout << "TEST ";
			Entity* ground = tile->ground;
			
			if(tile->groundModel == "road-straight" || tile->groundModel == "road-turn" || tile->groundModel == "road-threeway" || tile->groundModel == "road-fourway")
			{

				glm::vec3 pos = ground->getPosition();
				mmRoadVerts.push_back(pos);

				//grey
				mmRoadColors.push_back(0.6f);	//r
				mmRoadColors.push_back(0.6f);	//g
				mmRoadColors.push_back(0.6f);	//b
			}
			for(unsigned int k = 0; k < tile->entityTemplates.size(); k++)
			{
				if (tile->house)
				{

					glm::vec3 pos = ground->getPosition();
					mmHouseVerts.push_back(pos);
					//pink
					mmHouseColors.push_back(1.0f);	//r
					mmHouseColors.push_back(0.68f);	//g
					mmHouseColors.push_back(0.73f);	//b
				}
				else if(tile->pickup)
				{
					glm::vec3 pos = ground->getPosition();
					mmRoadVerts.push_back(pos);
					//pink
					mmRoadColors.push_back(1.0f);	//r
					mmRoadColors.push_back(0.55f);	//g
					mmRoadColors.push_back(0.0f);	//b
				}
			}
		}
	}

	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	std::vector<GLuint> faces;
	std::vector<glm::vec3> raw_verts;

	bool res = ContentLoading::loadOBJ("res\\Models\\mmVan.obj", mmVanVerts, uvs, normals, faces, raw_verts);

	//buffering
	glUseProgram(basicProgramID);

	glBindVertexArray(mmRoadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mmRoadVertBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3) * mmRoadVerts.size(),	// byte size of Vec3f, 4 of them
		mmRoadVerts.data(),		// pointer (Vec3f*) to contents of verts
		GL_STATIC_DRAW);	// Usage pattern of GPU buffer

							// RGB values for the 4 vertices of the quad

	glBindBuffer(GL_ARRAY_BUFFER, mmRoadColorBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(float)*mmRoadColors.size(),
		mmRoadColors.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // match layout # in shader
	glBindBuffer(GL_ARRAY_BUFFER, mmRoadVertBuffer);
	glVertexAttribPointer(
		0,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
		);

	glEnableVertexAttribArray(1); // match layout # in shader
	glBindBuffer(GL_ARRAY_BUFFER, mmRoadColorBuffer);
	glVertexAttribPointer(
		1,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
		);

	glBindVertexArray(0); // reset to default		
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	
	glBindVertexArray(mmHouseVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mmHouseVertBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3) * mmHouseVerts.size(),	// byte size of Vec3f, 4 of them
		mmHouseVerts.data(),		// pointer (Vec3f*) to contents of verts
		GL_STATIC_DRAW);	// Usage pattern of GPU buffer

							// RGB values for the 4 vertices of the quad

	glBindBuffer(GL_ARRAY_BUFFER, mmHouseColorBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(float)*mmHouseColors.size(),
		mmHouseColors.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // match layout # in shader
	glBindBuffer(GL_ARRAY_BUFFER, mmHouseVertBuffer);
	glVertexAttribPointer(
		0,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
		);

	glEnableVertexAttribArray(1); // match layout # in shader
	glBindBuffer(GL_ARRAY_BUFFER, mmHouseColorBuffer);
	glVertexAttribPointer(
		1,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
		);

	glBindVertexArray(0); // reset to default		
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	

	glBindBuffer(GL_ARRAY_BUFFER, mmVanVertBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3) * mmVanVerts.size(),	// byte size of Vec3f, 4 of them
		mmVanVerts.data(),		// pointer (Vec3f*) to contents of verts
		GL_STATIC_DRAW);	// Usage pattern of GPU buffer

							// RGB values for the 4 vertices of the quad

	for (int i = 0; i < 4; i++) {
		mmVanColors.clear();
		switch (i) {
		case 0:
			for (unsigned int j = 0; j < mmVanVerts.size(); j++) {
				mmVanColors.push_back(1.0f);
				mmVanColors.push_back(0.0f);
				mmVanColors.push_back(0.0f);
			}
			break;
		case 1:
			for (unsigned int j = 0; j < mmVanVerts.size(); j++) {
				mmVanColors.push_back(0.0f);
				mmVanColors.push_back(0.0f);
				mmVanColors.push_back(1.0f);
			}
			break;
		case 2:
			for (unsigned int j = 0; j < mmVanVerts.size(); j++) {
				mmVanColors.push_back(0.0f);
				mmVanColors.push_back(1.0f);
				mmVanColors.push_back(0.0f);
			}
			break;
		case 3:
			for (unsigned int j = 0; j < mmVanVerts.size(); j++) {
				mmVanColors.push_back(1.0f);
				mmVanColors.push_back(1.0f);
				mmVanColors.push_back(0.0f);
			}
			break;
		}

		glBindBuffer(GL_ARRAY_BUFFER, mmVanColorBuffers[i]);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(float)*mmVanColors.size(),
			mmVanColors.data(),
			GL_STATIC_DRAW);

		glBindVertexArray(mmVanVAOs[i]);
		glEnableVertexAttribArray(0); // match layout # in shader
		glBindBuffer(GL_ARRAY_BUFFER, mmVanVertBuffer);
		glVertexAttribPointer(
			0,		// attribute layout # above
			3,		// # of components (ie XYZ )
			GL_FLOAT,	// type of components
			GL_FALSE,	// need to be normalized?
			0,		// stride
			(void*)0	// array buffer offset
			);

		glEnableVertexAttribArray(1); // match layout # in shader
		glBindBuffer(GL_ARRAY_BUFFER, mmVanColorBuffers[i]);
		glVertexAttribPointer(
			1,		// attribute layout # above
			3,		// # of components (ie XYZ )
			GL_FLOAT,	// type of components
			GL_FALSE,	// need to be normalized?
			0,		// stride
			(void*)0	// array buffer offset
			);

		glBindVertexArray(0); // reset to default		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	float maxX = 0;
	float maxY = 0;
	float maxZ = 0;
	float minX = 1000;
	float minY = 1000;
	float minZ = 1000;

	//cout << "size " << mmRoadVerts.size() << endl;
	for(unsigned int i = 0; i < mmRoadVerts.size(); i++)
	{
		//cout << mmRoadVerts[i].x << " " << mmRoadVerts[i].y << " " << mmRoadVerts[i].z << " " << endl;
	}

	for(unsigned int i = 0; i < mmRoadVerts.size(); i++)
	{

		if (mmRoadVerts[i].x < minX)
			minX = mmRoadVerts[i].x;
		if (mmRoadVerts[i].y < minY)
			minY = mmRoadVerts[i].y;
		if (mmRoadVerts[i].z < minZ)
			minZ = mmRoadVerts[i].z;
	}

	for (unsigned int i = 0; i < mmRoadVerts.size(); i++)
	{
		if (mmRoadVerts[i].x > maxX)
			maxX = mmRoadVerts[i].x;
		if (mmRoadVerts[i].y > maxY)
			maxY = mmRoadVerts[i].y;
		if (mmRoadVerts[i].z > maxZ)
			maxZ = mmRoadVerts[i].z;
	}


	vec3 diameter(maxX - minX, maxY - minY, maxZ - minZ);
	float mmRadius = glm::length(diameter)*1.6;
	//cout << "radius " << mmRadius << endl;


	float centerX = (minX + maxX)/2;
	float centerY = (minY + maxY)/2;
	float centerZ = (minZ + maxZ)/2;

	mmCenter = vec3(centerX, centerY, centerZ);

	//cout << maxX << " " << maxY << " " << maxZ << " max" << endl;
	//cout << minX << " " << minY << " " << minZ << " min" << endl;

	//cout << centerX << " " << centerY << " " << centerZ << " cetner" << endl;

	mmV = glm::lookAt(
		glm::vec3(centerX, mmRadius, centerZ), // Camera is at (4,3,3), in World Space
		glm::vec3(centerX, centerY, centerZ), // and looks at the origin
		glm::vec3(0, 0, 1)  // Head is up (set to 0,-1,0 to look upside-down)
		);

	shift = vec3(2.38, 0.0, -1.7);
}

void RenderingEngine::drawMinimap(Vehicle* vans[4])
{
	 glDisable(GL_DEPTH_TEST);
	glUseProgram(basicProgramID);

	// Use VAO that holds buffer bindings
	// and attribute config of buffers
	glBindVertexArray(mmRoadVAO);
	// Draw Quads, start at vertex 0, draw 4 of them (for a quad)
	
	mmM = mat4(1.0f);
	//mmM = scale(mmM, vec3(0.5, 0.5, 0.5));
	mmM = translate(mmM,mmCenter * shift);	//positive X moves it left. Positive Z moves it up

	mat4 mmMVP = P * mmV * mmM;

	glUniformMatrix4fv(basicmvpID,		// ID
		1,		// only 1 matrix
		GL_FALSE,	// transpose matrix, Mat4f is row major
		value_ptr(mmMVP)	// pointer to data in Mat4f
		);

	GLfloat width = 20;
	glPointSize(width);

	glDrawArrays(GL_POINTS, 0, mmRoadVerts.size());
	glBindVertexArray(0);

	
	glBindVertexArray(mmHouseVAO);
	// Draw Quads, start at vertex 0, draw 4 of them (for a quad)

	glUniformMatrix4fv(basicmvpID,		// ID
		1,		// only 1 matrix
		GL_FALSE,	// transpose matrix, Mat4f is row major
		value_ptr(mmMVP)	// pointer to data in Mat4f
		);

	glPointSize(width);

	glDrawArrays(GL_POINTS, 0, mmHouseVerts.size());
	glBindVertexArray(0);

	for (int i = 0; i < 4; i++) {
		glBindVertexArray(mmVanVAOs[i]);
		mmM = mat4(1.0f);
		mmM = translate(mmM, mmCenter * shift);
		mmM = mmM * vans[i]->getModelMatrix();
		mmM = scale(mmM, vec3(2.0));
		mmM = rotate(mmM, -1.5708f, glm::vec3(0,1,0));

		mmMVP = P * mmV * mmM;
		glUniformMatrix4fv(basicmvpID,
			1,
			GL_FALSE,
			value_ptr(mmMVP)
			);
		glDrawArrays(GL_TRIANGLES, 0, mmVanVerts.size());
		glBindVertexArray(0);
	}
}

void RenderingEngine::setupNodes(vector<glm::vec3> verts, glm::vec3 color)
{

	vector<float> colors;
	for(unsigned int i = 0; i < verts.size(); i++)
	{
		colors.push_back(color.x);
		colors.push_back(color.y);
		colors.push_back(color.z);
	}

	glUseProgram(basicProgramID);

	glBindVertexArray(nodeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, nodeVertBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3) * verts.size(),	// byte size of Vec3f, 4 of them
		verts.data(),		// pointer (Vec3f*) to contents of verts
		GL_STATIC_DRAW);	// Usage pattern of GPU buffer

							// RGB values for the 4 vertices of the quad

	glBindBuffer(GL_ARRAY_BUFFER, nodeColorBuffer);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(float)*colors.size(),
		colors.data(),
		GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // match layout # in shader
	glBindBuffer(GL_ARRAY_BUFFER, nodeVertBuffer);
	glVertexAttribPointer(
		0,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
		);

	glEnableVertexAttribArray(1); // match layout # in shader
	glBindBuffer(GL_ARRAY_BUFFER, nodeColorBuffer);
	glVertexAttribPointer(
		1,		// attribute layout # above
		3,		// # of components (ie XYZ )
		GL_FLOAT,	// type of components
		GL_FALSE,	// need to be normalized?
		0,		// stride
		(void*)0	// array buffer offset
		);

	glBindVertexArray(0); // reset to default		
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void RenderingEngine::drawNodes(int size, string style)
{
	glEnable(GL_DEPTH_TEST);
	glUseProgram(basicProgramID);

	// Use VAO that holds buffer bindings
	// and attribute config of buffers
	glBindVertexArray(nodeVAO);
	// Draw Quads, start at vertex 0, draw 4 of them (for a quad)
	

	M = mat4(1.0f);
	mat4 MVP = P * V * M;

	glUniformMatrix4fv(basicmvpID,		// ID
		1,		// only 1 matrix
		GL_FALSE,	// transpose matrix, Mat4f is row major
		value_ptr(MVP)	// pointer to data in Mat4f
		);

	GLfloat width = 30;
	if(style == "lines")
	{
		glLineWidth(width);
		glDrawArrays(GL_LINE_STRIP, 0, size);
	}
	else if(style == "points")
	{
		glPointSize(width);
		glDrawArrays(GL_POINTS, 0, size);
	}
	glBindVertexArray(0);

}

void RenderingEngine::setupIntro()
{
	Renderable *logo = ContentLoading::createRenderable("res\\Models\\deliverers.obj");
	Renderable *gameBy = ContentLoading::createRenderable("res\\Models\\gameby.obj");
	Renderable *names = ContentLoading::createRenderable("res\\Models\\names.obj");
	GLuint introTexture = ContentLoading::loadDDS("res\\Textures\\intro-colored.DDS");

	assignBuffersTex(logo);
	assignBuffersTex(gameBy);
	assignBuffersTex(names);

	Entity *eLogo = new Entity();
	eLogo->setRenderable(logo);
	eLogo->setTexture(introTexture);
	introEntities.push_back(eLogo);
	
	Entity *eGameBy = new Entity();
	eGameBy->setRenderable(gameBy);
	eGameBy->setTexture(introTexture);
	//introEntities.push_back(eGameBy);

	Entity *eNames = new Entity();
	eNames->setRenderable(names);
	eNames->setTexture(introTexture);
	//introEntities.push_back(eNames);

	introM = mat4(1.0f);
	introV = glm::lookAt(
			glm::vec3(0,0,7), 
			glm::vec3(0,0,0), 
			glm::vec3(0,1,0)
		);

}

void RenderingEngine::displayIntro()
{

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable(GL_BLEND);
	glUseProgram(textureProgramID);


	glUniform3f(lightPos, 0.0f, 4.0f, 1.0f);
	glUniform1f(lightPow, 50.0f);
	glUniform3f(ambientScale, 0.5, 0.5, 0.5);

	for (int i = 0; i < (int)introEntities.size(); i++) {
		if (!introEntities[i]->hasRenderable())
			continue;
		introM = mat4(1.0f);

		//Translations done here. Order of translations is scale, rotate, translate
		introM = introEntities[i]->getModelMatrix();
		introM = calculateDefaultModel(introM, introEntities[i]);

		mat4 MVP = P * introV * introM;

		glUniformMatrix4fv(mvpID, 1, GL_FALSE, value_ptr(MVP));
		glUniformMatrix4fv(vID, 1, GL_FALSE, value_ptr(introV));
		glUniformMatrix4fv(mID, 1, GL_FALSE, value_ptr(introM));

		glBindVertexArray(introEntities[i]->getRenderable()->vao);
		GLuint tex = introEntities[i]->getTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		//glTexImage2DMultisample( GL_TEXTURE_2D_MULTISAMPLE, 2, GL_RGBA8, 1024, 768, false );

	// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(tID, 0);
		//glDrawArrays(GL_TRIANGLES, 0, entities[i]->getRenderable()->verts.size());
		
		glDrawElements(GL_TRIANGLES, introEntities[i]->getRenderable()->drawFaces.size(), GL_UNSIGNED_SHORT, (void*)0);

		glBindVertexArray(0);
	}


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

