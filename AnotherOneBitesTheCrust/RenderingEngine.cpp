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

	glEnable(GL_CULL_FACE);

	generateIDs();
	loadProjectionMatrix();
}

RenderingEngine::~RenderingEngine(void) {}

//vehicle dimensions
//x: 2.5, y:2, z:5
void RenderingEngine::displayFunc(vector<Entity*> entities)
{
	//glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
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
		glBindVertexArray(0);
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

	string textVsShader = "res\\Shaders\\TextVertexShader.vertexshader";
	string textFsShader = "res\\Shaders\\TextVertexShader.fragmentshader";
	string textVsSource = loadShaderStringfromFile(textVsShader);
	string textFsSource = loadShaderStringfromFile(textFsShader);
	textProgramID = CreateShaderProgram(textVsSource, textFsSource);
	glUseProgram(textProgramID);
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
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
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

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TESTING STUFF BELOW
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool RenderingEngine::loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}

	return true;
}


void RenderingEngine::testDraw() {
	// Draw all our entities

	//glClearDepth(1.0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(textureProgramID);
	GLuint mvpID = glGetUniformLocation(textureProgramID, "MVP");

	GLuint vID = glGetUniformLocation(textureProgramID, "V");
	GLuint mID = glGetUniformLocation(textureProgramID, "M");

	M = mat4(1.0f);
	M = glm::rotate(M, -1.5f, vec3(0,1,0));
	mat4 MVP = P * V * M;

	glUniformMatrix4fv( mvpID,
					1,
					GL_FALSE,
					value_ptr(MVP)
					);

	//GLint pID = glGetUniformLocation(phongProgramID, "proj_matrix");
	glUniformMatrix4fv( vID,
					1,
					GL_FALSE,
					value_ptr(V)
					);

	glUniformMatrix4fv( mID,
					1,
					GL_FALSE,
					value_ptr(M)
					);

	glUniform3f(glGetUniformLocation(textureProgramID, "LightPosition_worldspace"), 35, 100, 35);

	glBindVertexArray(vanVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(TextureID, 0);

	glDrawArrays(GL_TRIANGLES, 0, textureVerts.size());

	//glDrawElements(GL_TRIANGLES, phongFaces.size(), GL_UNSIGNED_INT, (void*)0);
}

void RenderingEngine::testOBJLoading()
{
	string vsShader = "res\\Shaders\\textured-StandardShading.vertexshader";
	string fsShader = "res\\Shaders\\textured-StandardShading.fragmentshader";
	string vsSource = loadShaderStringfromFile(vsShader);
	string fsSource = loadShaderStringfromFile(fsShader);
	textureProgramID = CreateShaderProgram(vsSource, fsSource);
	glUseProgram(textureProgramID);

	//std::vector<glm::vec3> vertices;
	//std::vector<glm::vec2> uvs;
	//std::vector<glm::vec3> normals; // Won't be used at the moment.
	//std::vector<GLuint> faces;
	bool res = RenderingEngine::loadOBJ("res\\Models\\Van_textured\\Van-with-uv.obj", textureVerts, textureUVs, textureNorms);


	cout << "Number of verts " << textureVerts.size() << endl;
	cout << "Number of UVs " << textureUVs.size() << endl;
	cout << "Number of normals " << textureNorms.size() << endl;

	glGenVertexArrays(1, &vanVAO);
	glBindVertexArray(vanVAO);


	glGenBuffers(1, &vanVerts);
	glGenBuffers(1, &vanUVs);
	glGenBuffers(1, &vanNormals);

	Texture = ContentLoading::loadDDS("res\\Models\\Van_textured\\Van-uv-colored-test.DDS");
	TextureID = glGetUniformLocation(textureProgramID, "myTextureSampler");


	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
	glBufferData(GL_ARRAY_BUFFER, textureVerts.size() * sizeof(glm::vec3), textureVerts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vanUVs);
	glBufferData(GL_ARRAY_BUFFER, textureUVs.size() * sizeof(glm::vec2), textureUVs.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
	glBufferData(GL_ARRAY_BUFFER, textureNorms.size() * sizeof(glm::vec3), textureNorms.data(), GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vanUVs);
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
	glVertexAttribPointer(
		2,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	

	glBindVertexArray(0);
}

//
//
//int RenderingEngine::init_resourses()
//{
//
//	cout << "Initializing text resourses " << endl;
//
//	if(FT_Init_FreeType(&ft))
//	{
//		cout << "Could not init freetype library " << endl;
//		return 0;
//	}
//
//	if(FT_New_Face(ft, "res\\Fonts\\CodePredators-Regular.ttf", 0, &face))
//	{
//		cout << "Could not open font" << endl;
//		return 0;
//	}
//
//	
//	GLuint attribute_coord = glGetUniformLocation(textProgramID, "coord");
//	GLuint uniform_tex = glGetUniformLocation(textProgramID, "tex");
//	GLuint uniform_color = glGetUniformLocation(textProgramID, "color");
//
//	glGenBuffers(1, &textVBO);
//	glGenVertexArrays(1, &textVAO);
//    glGenTextures(1, &texture);
//    glGenSamplers(1, &sampler);
//    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	cout << "Finished initializing text resourses" << endl;
//	return 1;
//}
//
//void RenderingEngine::render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy) {
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    const FT_GlyphSlot glyph = face->glyph;
//
//    for(auto c : str) {
//        if(FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
//            continue;
//
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
//                     glyph->bitmap.width, glyph->bitmap.rows,
//                     0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
//
//        const float vx = x + glyph->bitmap_left * sx;
//        const float vy = y + glyph->bitmap_top * sy;
//        const float w = glyph->bitmap.width * sx;
//        const float h = glyph->bitmap.rows * sy;
//
//        struct {
//            float x, y, s, t;
//        } data[6] = {
//            {vx    , vy    , 0, 0},
//            {vx    , vy - h, 0, 1},
//            {vx + w, vy    , 1, 0},
//            {vx + w, vy    , 1, 0},
//            {vx    , vy - h, 0, 1},
//            {vx + w, vy - h, 1, 1}
//        };
//
//        glBufferData(GL_ARRAY_BUFFER, sizeof(data)*sizeof(float), data, GL_DYNAMIC_DRAW);
//        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
//        glDrawArrays(GL_TRIANGLES, 0, sizeof(data));
//
//        x += (glyph->advance.x >> 6) * sx;
//        y += (glyph->advance.y >> 6) * sy;
//    }
//
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
//}
//
//void RenderingEngine::displayText() {
//
//	//glClear(GL_COLOR_BUFFER_BIT);
//	glEnable(GL_BLEND);
//	glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
//	float sx = 2.0 / 1024;
//	float sy = 2.0 / 768;
//	glUseProgram(textProgramID);
//    glBindAttribLocation(textProgramID, 0, "in_Position");
//    GLuint texUniform = glGetUniformLocation(textProgramID, "tex");
//    GLuint colorUniform = glGetUniformLocation(textProgramID, "color");
//	
//	glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, texture);
//    glBindSampler(0, sampler);
//    glBindVertexArray(textVAO);
//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
//    glUseProgram(textProgramID);
//    glUniform4f(colorUniform, 0, 0, 0, 1);
//    glUniform1i(texUniform, 0);
//
//    FT_Set_Pixel_Sizes(face, 0, 30);
//    render_text("Another", face, -1, 0.93, sx, sy);
//	//render_text("Speed:", face, 0.6, -1, sx, sy);
//	
//}
