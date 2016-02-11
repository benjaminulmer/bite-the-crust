#include "RenderingEngine.h"
#include "Game.h"
#include <math.h>
#include <iostream>

RenderingEngine::RenderingEngine() {}

RenderingEngine::~RenderingEngine(void) {}
// Push all the entity data to the renderer.

bool RenderingEngine::loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec3> & out_normals,
	std::vector<GLuint> & out_faces
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
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2] );
			if (matches != 6){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			out_faces.push_back(vertexIndex[0]-1);
			out_faces.push_back(vertexIndex[1]-1);
			out_faces.push_back(vertexIndex[2]-1);
			/*uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);*/
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
			out_normals.push_back(vec3(normalIndex[0]-1, normalIndex[1]-1, normalIndex[2]-1));
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	//for( unsigned int i=0; i<vertexIndices.size(); i++ ){

	//	// Get the indices of its attributes
	//	unsigned int vertexIndex = vertexIndices[i];
	//	//unsigned int uvIndex = uvIndices[i];
	//	unsigned int normalIndex = normalIndices[i];
	//	
	//	// Get the attributes thanks to the index
	//	glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
	//	//glm::vec2 uv = temp_uvs[ uvIndex-1 ];
	//	glm::vec3 normal = temp_normals[ normalIndex-1 ];
	//	
	//	// Put the attributes in buffers
	//	out_vertices.push_back(vertex);
	//	//out_uvs     .push_back(uv);
	//	out_normals .push_back(normal);
	//
	//}

	out_vertices = temp_vertices;
	//out_normals = temp_normals;
	/*for(int i = 0; i < vertexIndices.size(); i++)
	{
		out_faces.push_back(vertexIndices[i]);
	}
*/
	return true;
}

bool RenderingEngine::loadOBJNonIndexed(
	const char * path, 
	std::vector<glm::vec4> & out_vertices, 
	//std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec4> temp_vertices; 
	//std::vector<glm::vec2> temp_uvs;
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
			glm::vec4 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vec4(vertex.x,vertex.y,vertex.z,1.0f));
		//}else if ( strcmp( lineHeader, "vt" ) == 0 ){
		//	glm::vec2 uv;
		//	fscanf(file, "%f %f\n", &uv.x, &uv.y );
		//	uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
		//	temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
			if (matches != 6){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			//uvIndices    .push_back(uvIndex[0]);
			//uvIndices    .push_back(uvIndex[1]);
			//uvIndices    .push_back(uvIndex[2]);
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
		//unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec4 vertex = temp_vertices[ vertexIndex-1 ];
		//glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		//out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}

	return true;
}


bool RenderingEngine::loadOBJPhong(
	const char * path, 
	std::vector<glm::vec4> & out_vertices, 
	std::vector<glm::vec3> & out_normals,
	std::vector<GLuint> & out_faces
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec4> temp_vertices; 
	temp_vertices.push_back(vec4(0,0,0,0));
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;
	temp_normals.push_back(vec3(0,0,0));


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
			glm::vec4 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(glm::vec4(vertex.x,vertex.y,vertex.z,1.0f));
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
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2] );
			if (matches != 6){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				return false;
			}
			out_faces.push_back(vertexIndex[0]);
			out_faces.push_back(vertexIndex[1]);
			out_faces.push_back(vertexIndex[2]);
			/*uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);*/
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
			out_normals.push_back(temp_normals.at(normalIndex[0]));
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	//for( unsigned int i=0; i<vertexIndices.size(); i++ ){

	//	// Get the indices of its attributes
	//	unsigned int vertexIndex = vertexIndices[i];
	//	//unsigned int uvIndex = uvIndices[i];
	//	unsigned int normalIndex = normalIndices[i];
	//	
	//	// Get the attributes thanks to the index
	//	glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
	//	//glm::vec2 uv = temp_uvs[ uvIndex-1 ];
	//	glm::vec3 normal = temp_normals[ normalIndex-1 ];
	//	
	//	// Put the attributes in buffers
	//	out_vertices.push_back(vertex);
	//	//out_uvs     .push_back(uv);
	//	out_normals .push_back(normal);
	//
	//}

	out_vertices = temp_vertices;
	//out_normals = temp_normals;
	/*for(int i = 0; i < vertexIndices.size(); i++)
	{
		out_faces.push_back(vertexIndices[i]);
	}
*/
	return true;
}


void RenderingEngine::pushEntities() {
}

void RenderingEngine::draw() {
	// Draw all our entities

	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(phongProgramID);
	GLint mvID = glGetUniformLocation(phongProgramID, "mv_matrix");

	M = mat4(1.0f);
	M = glm::rotate(M, 1.5f, vec3(0,1,0));
	mat4 MV = V * M;

	glUniformMatrix4fv( mvID,
					1,
					GL_FALSE,
					value_ptr(MV)
					);

	GLint pID = glGetUniformLocation(phongProgramID, "proj_matrix");
		glUniformMatrix4fv( pID,
					1,
					GL_FALSE,
					value_ptr(P)
					);

	glBindVertexArray(vanVAO);
	glDrawArrays(GL_TRIANGLES, 0, phongVerts.size());
	//glDrawElements(GL_TRIANGLES, phongFaces.size(), GL_UNSIGNED_INT, (void*)0);
}

//vehicle dimensions
//x: 2.5, y:2, z:5


void RenderingEngine::displayFunc(vector<Entity*> entities)
{

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glUseProgram(phongProgramID);
	GLint mvID = glGetUniformLocation(phongProgramID, "mv_matrix");
	GLint pID = glGetUniformLocation(phongProgramID, "proj_matrix");

	for (int i = 0; i < (int)entities.size(); i++) {
		if (!entities[i]->hasRenderable())
			continue;
		M = mat4(1.0f);

		//Translations done here. Order of translations is scale, rotate, translate

		//M = glm::scale(M, vec3(x,y,z))
		//M = glm::rotate(M, 45.0f, vec3(0,1,0));
		M = entities[i]->getModelMatrix();
		
		//M = glm::rotate(M, -1.5708f, vec3(0,1,0));

		M = calculateDefaultModel(M, entities[i]);

		MV = V * M;
		glUniformMatrix4fv( mvID,
					1,
					GL_FALSE,
					value_ptr(MV)
					);

		glUniformMatrix4fv( pID,
					1,
					GL_FALSE,
					value_ptr(P)
					);

		glUniform3f(glGetUniformLocation(phongProgramID, "diffuse_albedo"), entities[i]->getColor().x, entities[i]->getColor().y, entities[i]->getColor().z);
		glUniform3f(glGetUniformLocation(phongProgramID, "ambient"), entities[i]->getAmbient().x, entities[i]->getAmbient().y, entities[i]->getAmbient().z);

		glBindVertexArray(entities[i]->getRenderable()->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, entities[i]->getRenderable()->getVertices().size());
		//glDrawElements(GL_TRIANGLES, entities[i]->getRenderable()->getFaces().size(), GL_UNSIGNED_INT, (void*)0);
	}
}

mat4 RenderingEngine::calculateDefaultModel(mat4 model, Entity * entity)
{
	
	//Translations done here. Order of translations is scale, rotate, translate
	
	model = glm::scale(model,entity->getDefaultScale());
	model = glm::translate(model, entity->getDefaultTranslation());
	model = glm::rotate(model, entity->getDefaultRotationAngle(), entity->getDefaultRotationAxis());
	return model;
}

void RenderingEngine::generateIDs()
{
	cout << "Generating IDs" << endl;
	string vsShader = "res\\Shaders\\per-fragment-phong.vs.glsl";
	string fsShader = "res\\Shaders\\per-fragment-phong.fs.glsl";
	string vsSource = loadShaderStringfromFile(vsShader);
	string fsSource = loadShaderStringfromFile(fsShader);
	phongProgramID = CreateShaderProgram(vsSource, fsSource);
	glUseProgram(phongProgramID);
}

void RenderingEngine::loadProjectionMatrix()
{
	P = perspective(45.0f, (float)1024/768 , 0.1f, 100.0f);
}


void RenderingEngine::updateView(Camera& c) {
	V = glm::lookAt(c.getPosition(), c.getLookAtPosition(), c.getUpVector());
}

void RenderingEngine::init()
{
	cout << "Initialize" << endl;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	generateIDs();
	loadProjectionMatrix();
}

void RenderingEngine::assignBuffers(Renderable* r) {
	GLuint vertexBuffer;
	GLuint normalBuffer;
	//GLuint indexBuffer;
	GLuint vao;


	glGenVertexArrays(1, &vao);			//vao
	glGenBuffers(1, &vertexBuffer);		//vertices vbo
	glGenBuffers(1, &normalBuffer);		//color vbo
	//glGenBuffers(1, &indexBuffer);		//faces
	

	vector<vec4> vertices = r->getVertices();
	//vector<vec3> colours = r->getColours();
	vector<vec3> normals = r->getNormals();
	//vector<GLuint> faces = r->getFaces();

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*vertices.size(), vertices.data(), GL_STATIC_DRAW);	//buffering vertex data

	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*normals.size(), normals.data(), GL_STATIC_DRAW);		//buffering color data

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);			//bind faces
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * faces.size(), faces.data(), GL_STATIC_DRAW);

	//r->setVertexVBO(vertexBuffer);
	//r->setColourVBO(colourBuffer);

	//bind to shaders
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,
		4,
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

	glBindVertexArray(0);
	r->setVAO(vao);
}

void RenderingEngine::deleteBuffers(Renderable *r) {
	GLuint vao = r->getVAO();
	GLuint vbuf = r->getVertexVBO();
	GLuint cbuf = r->getColourVBO();
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbuf);
	glDeleteBuffers(1, &cbuf);
}


void RenderingEngine::testOBJLoading()
{
	string vsShader = "res\\Shaders\\per-fragment-phong.vs.glsl";
	string fsShader = "res\\Shaders\\per-fragment-phong.fs.glsl";
	string vsSource = loadShaderStringfromFile(vsShader);
	string fsSource = loadShaderStringfromFile(fsShader);
	phongProgramID = CreateShaderProgram(vsSource, fsSource);
	glUseProgram(phongProgramID);

	//std::vector<glm::vec3> vertices;
	//std::vector<glm::vec2> uvs;
	//std::vector<glm::vec3> normals; // Won't be used at the moment.
	//std::vector<GLuint> faces;
	bool res = loadOBJNonIndexed("res\\Models\\Van.obj", phongVerts, phongNorms);


	cout << "Number of verts " << phongVerts.size() << endl;
	//cout << "NUmber of faces " << phongFaces.size()/3 << endl;
	cout << "Number of normals " << phongNorms.size() << endl;

	glGenVertexArrays(1, &vanVAO);
	glGenBuffers(1, &vanVerts);
	glGenBuffers(1, &vanNormals);
	glGenBuffers(1, &vanIndexBuffer);

	glBindVertexArray(vanVAO);

	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
	glBufferData(GL_ARRAY_BUFFER, phongVerts.size() * sizeof(glm::vec4), phongVerts.data(), GL_STATIC_DRAW);

	//for(int i = 0; i < phongVerts.size(); i++)
	//{
	//	colors.push_back(vec3(1,1,1));
	//}

	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
	glBufferData(GL_ARRAY_BUFFER, phongNorms.size() * sizeof(glm::vec3), phongNorms.data(), GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vanIndexBuffer);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, phongFaces.size() * sizeof(GLuint), phongFaces.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vanVerts);
	glVertexAttribPointer(
		0,
		4,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vanNormals);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0);
	
		glBindVertexArray(0);
}