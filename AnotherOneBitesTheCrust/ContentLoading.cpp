#include "ContentLoading.h"

namespace ContentLoading {

// Loads and stores tuning data for vehicle from provided file
bool loadVehicleData(char* filename, Vehicle* vehicle) {
	FILE* filePointer;
	errno_t err = fopen_s(&filePointer, filename, "rb");
	if (err != 0) {
		printf("Error, vehicle file couldn't load.");
		return false;
	}
	char readBuffer[10000];
	rapidjson::FileReadStream reader(filePointer, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(reader);

	// Ben added this. Very ugle, should be done better probably
	if (d.HasMember("mass")) {
		vehicle->tuning.chassisMass = (float)d["mass"].GetDouble();
	}
	if (d.HasMember("chassisMOIscalarX")) {
		vehicle->tuning.chassisMOIscalar.x = (float)d["chassisMOIscalarX"].GetDouble();
	}
	if (d.HasMember("chassisMOIscalarY")) {
		vehicle->tuning.chassisMOIscalar.y = (float)d["chassisMOIscalarY"].GetDouble();
	}
	if (d.HasMember("chassisMOIscalarZ")) {
		vehicle->tuning.chassisMOIscalar.z = (float)d["chassisMOIscalarZ"].GetDouble();
	}
	if (d.HasMember("chassisCMOffsetX")) {
		vehicle->tuning.chassisCMOffset.x = (float)d["chassisCMOffsetX"].GetDouble();
	}
	if (d.HasMember("chassisCMOffsetY")) {
		vehicle->tuning.chassisCMOffset.y = (float)d["chassisCMOffsetY"].GetDouble();
	}
	if (d.HasMember("chassisCMOffsetZ")) {
		vehicle->tuning.chassisCMOffset.z = (float)d["chassisCMOffsetZ"].GetDouble();
	}
	if (d.HasMember("chassisStaticFriction")) {
		vehicle->tuning.chassisStaticFriction = (float)d["chassisStaticFriction"].GetDouble();
	}
	if (d.HasMember("chassisDynamicFriction")) {
		vehicle->tuning.chassisDynamicFriction = (float)d["chassisDynamicFriction"].GetDouble();
	}
	if (d.HasMember("chassisRestitution")) {
		vehicle->tuning.chassisRestitution = (float)d["chassisRestitution"].GetDouble();
	}
	if (d.HasMember("wheelMass")) {
		vehicle->tuning.wheelMass = (float)d["wheelMass"].GetDouble();
	}
	if (d.HasMember("wheelMOIscalar")) {
		vehicle->tuning.wheelMOIscalar = (float)d["wheelMOIscalar"].GetDouble();
	}
	if (d.HasMember("wheelDamping")) {
		vehicle->tuning.wheelDamping = (float)d["wheelDamping"].GetDouble();
	}
	if (d.HasMember("wheelStaticFriction")) {
		vehicle->tuning.wheelStaticFriction = (float)d["wheelStaticFriction"].GetDouble();
	}
	if (d.HasMember("wheelDynamicFriction")) {
		vehicle->tuning.wheelDynamicFriction = (float)d["wheelDynamicFriction"].GetDouble();
	}
	if (d.HasMember("wheelRestitution")) {
		vehicle->tuning.wheelRestitution = (float)d["wheelRestitution"].GetDouble();
	}
	if (d.HasMember("maxBrakeTorque")) {
		vehicle->tuning.maxBrakeTorque = (float)d["maxBrakeTorque"].GetDouble();
	}
	if (d.HasMember("maxHandBrakeTorque")) {
		vehicle->tuning.maxHandBrakeTorque = (float)d["maxHandBrakeTorque"].GetDouble();
	}
	if (d.HasMember("maxSteerDegrees")) {
		vehicle->tuning.maxSteerDegrees = d["maxSteerDegrees"].GetInt();
	}
	if (d.HasMember("engineTorque")) {
		vehicle->tuning.engineTorque = (float)d["engineTorque"].GetDouble();
	}
	if (d.HasMember("engineRPM")) {
		vehicle->tuning.engineRPM = (float)d["engineRPM"].GetDouble();
	}
	if (d.HasMember("engineMOI")) {
		vehicle->tuning.engineMOI = (float)d["engineMOI"].GetDouble();
	}
	if (d.HasMember("gearSwitchTime")) {
		vehicle->tuning.gearSwitchTime = (float)d["gearSwitchTime"].GetDouble();
	}
	if (d.HasMember("gearFinalRatio")) {
		vehicle->tuning.gearFinalRatio = (float)d["gearFinalRatio"].GetDouble();
	}
	if (d.HasMember("clutchStrength")) {
		vehicle->tuning.clutchStrength = (float)d["clutchStrength"].GetDouble();
	}

	fclose(filePointer);

	return true;
}

// Verifies entity list is well formated
bool verifyEntityList(const rapidjson::Document &d) {
	if (!d.HasMember("entities")) {
		printf("Missing entities array.");
		return false;
	}
	for (rapidjson::SizeType i = 0; i < d["entities"].Size(); i++) {
		const rapidjson::Value& entry = d["entities"][i];
		if (!entry.HasMember("name")) {
			printf("Entry %d is missing a name.", i);
			return false;
		}
		if (!entry.HasMember("model")) {
			printf("Entry %d is missing a model.", i);
			return false;
		}
		if (!entry.HasMember("physics")) {
			printf("Entry %d is missing a physics file.", i);
			return false;
		}
	}
	return true;
}

// Load entities and their information
bool ContentLoading::loadEntityList(char* filename, std::map<std::string, Renderable*> &modelMap, std::map<std::string, PhysicsEntityInfo*> &physicsMap,
									std::map<std::string, GLuint> &textureMap) {
	FILE* filePointer;
	errno_t err = fopen_s(&filePointer, filename, "rb");
	if (err != 0) {
		printf("Error, entity list file couldn't load.");
		return false;
	}
	char readBuffer[10000];
	rapidjson::FileReadStream reader(filePointer, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(reader);

	if (!verifyEntityList(d)) {
		return false;
	}

	const rapidjson::Value& entitiesArray = d["entities"];
	for (rapidjson::SizeType i = 0; i < entitiesArray.Size(); i++) {
		std::string name = entitiesArray[i]["name"].GetString();

		std::string renderableModelFile = entitiesArray[i]["model"].GetString();
		renderableModelFile.insert(0, "res\\Models\\");
		Renderable* r = createRenderable(renderableModelFile);
		modelMap[name] = r;

		std::string physicsDataName = entitiesArray[i]["physics"].GetString();
		physicsDataName.insert(0, "res\\JSON\\Physics\\");
		PhysicsEntityInfo* info = createPhysicsInfo(physicsDataName.c_str(), r);
		physicsMap[name] = info;

		if (entitiesArray[i].HasMember("texture")) {
			std::string textureName = entitiesArray[i]["texture"].GetString();
			textureName.insert(0, "res\\Textures\\");
			textureMap[name] = loadDDS(textureName.c_str());
		}
	}

	fclose(filePointer);
	return true;
}

// Create renderable from obj file
Renderable* createRenderable(std::string modelFile) {
	Renderable * r = new Renderable();
	std::vector<glm::vec3> verts;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	bool res = ContentLoading::loadOBJ(modelFile.c_str(), verts, uvs, normals);

	r->setVerts(verts);
	r->setUVs(uvs);
	r->setNorms(normals);

	return r;
}

// Create physics information from JSON file. Uses obj if needed/specified
PhysicsEntityInfo* createPhysicsInfo(const char* filename, Renderable* model) {
	FILE* filePointer;
	errno_t err = fopen_s(&filePointer, filename, "rb");
	if (err != 0) {
		printf("Error, physics info file %s couldn't load.", filename);
		return nullptr;
	}
	char readBuffer[10000];
	rapidjson::FileReadStream reader(filePointer, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(reader);

	PhysicsEntityInfo* info = new PhysicsEntityInfo();
	if (d.HasMember("type")) {
		std::string type = d["type"].GetString();
		if (type == "dynamic") {
			info->type = PhysicsType::DYNAMIC;
			info->dynamicInfo = new DynamicInfo();
		} else if (type == "static") {
			info->type = PhysicsType::STATIC;
		} else {
			// Static by default
			info->type = PhysicsType::STATIC;
		}
	}
	if (d.HasMember("dynamicInfo")) {
		const rapidjson::Value& dynamicInfo = d["dynamicInfo"];
		if (dynamicInfo.HasMember("density")) {
			info->dynamicInfo->density = (float)dynamicInfo["density"].GetDouble();
		}
		if (dynamicInfo.HasMember("linearDamping")) {
			info->dynamicInfo->linearDamping = (float)dynamicInfo["linearDamping"].GetDouble();
		}
		if (dynamicInfo.HasMember("angularDamping")) {
			info->dynamicInfo->angularDamping = (float)dynamicInfo["angularDamping"].GetDouble();
		}
	}
	if (d.HasMember("geometry")) {
		const rapidjson::Value& geometry = d["geometry"];
		for (rapidjson::SizeType i = 0; i < geometry.Size(); i++) {
			std::string shapeName = geometry[i]["shape"].GetString();
			ShapeInfo* shape;
			if (shapeName == "box") {
				BoxInfo* box = new BoxInfo();
				box->geometry = Geometry::BOX;
				// Use the model dimensions by default
				glm::vec3 d = model->getDimensions();
				box->halfX = d.x * 0.5f;
				box->halfY = d.y * 0.5f;
				box->halfZ = d.z * 0.5f;
				// Overwrite with specifics if they're there
				if (geometry[i].HasMember("halfX"))
					box->halfX = (float)geometry[i]["halfX"].GetDouble();
				if (geometry[i].HasMember("halfY"))
					box->halfY = (float)geometry[i]["halfY"].GetDouble();
				if (geometry[i].HasMember("halfZ"))
					box->halfZ = (float)geometry[i]["halfZ"].GetDouble();
				shape = box;
			}

			// Added by Ben for testing
			else if (shapeName == "convexMesh")
			{
				ConvexMeshInfo* convexMesh = new ConvexMeshInfo();
				convexMesh->geometry = Geometry::CONVEX_MESH;
				convexMesh->verts = model->getVertices();
				shape = convexMesh;
			}
			else if (shapeName == "triangleMesh")
			{
				TriangleMeshInfo* triangleMesh = new TriangleMeshInfo();
				triangleMesh->geometry = Geometry::TRIANGLE_MESH;
				triangleMesh->verts = model->getVertices();
				triangleMesh->faces = model->getFaces();
				shape = triangleMesh;
			}
			
			
			if (geometry[i].HasMember("flag0")) {
				shape->filterFlag0 = stringToFlag(geometry[i]["flag0"].GetString());
			} else {
				shape->filterFlag0 = FilterFlag::OBSTACLE;
			}
			if (geometry[i].HasMember("flag1")) {
				shape->filterFlag1 = stringToFlag(geometry[i]["flag1"].GetString());
			} else {
				shape->filterFlag1 = FilterFlag::OBSTACLE_AGAINST;
			}
			if (geometry[i].HasMember("isDrivable")) {
				shape->isDrivable = (geometry[i]["isDrivable"].GetInt() != 0);
				std::cout << "TEST: " << geometry[i]["isDrivable"].GetInt() << std::endl;
			}
			info->shapeInfo.push_back(shape);
		}

	} else {
		// Give a default box around the model
		BoxInfo* box = new BoxInfo();
		box->geometry = Geometry::BOX;
		glm::vec3 d = model->getDimensions();
		box->halfX = d.x * 0.5f;
		box->halfY = d.y * 0.5f;
		box->halfZ = d.z * 0.5f;
		box->filterFlag0 = FilterFlag::OBSTACLE;
		box->filterFlag1 = FilterFlag::OBSTACLE_AGAINST;
		info->shapeInfo.push_back(box);
	}

	fclose(filePointer);
	return info;
}

bool validateMap(rapidjson::Document &d) {
	if (!d.HasMember("tiles")) {
		printf("Map file missing tiles array.");
		return false;
	}
	for (rapidjson::SizeType i = 0; i < d["tiles"].Size(); i++) {
		rapidjson::Value& entry = d["tiles"][i];
		if (!entry.HasMember("id")) {
			printf("Tile %d missing id.", i);
			return false;
		}
		if (!entry.HasMember("ground")) {
			printf("Tile %d missing ground.", i);
			return false;
		}
		if (!entry.HasMember("entities")) {
			printf("Tile id %d is missing its entities array.", entry["id"]);
			return false;
		}
		for (rapidjson::SizeType j = 0; j < entry["entities"].Size(); j++) {
			if (!entry["entities"][j].HasMember("model")) {
				printf("Entity %d in tile %d is missing a model.", j, entry["id"]);
				return false;
			}
		}

	}
	if (!d.HasMember("map")) {
		printf("Map file missing map member.");
		return false;
	}
	if (!d["map"].HasMember("tile size")) {
		printf("Map file missing tile size");
		return false;
	}
	if (!d["map"].HasMember("tiles")) {
		printf("Map file missing map tiles array");
		return false;
	}
	return true;
}

//todo, proper error checking for the json file format, right now it just blows up 95% of the time if you got it wrong
bool ContentLoading::loadMap(char* filename, Map &map) {
	FILE* filePointer;
	errno_t err = fopen_s(&filePointer, filename, "rb");
	if (err != 0) {
		printf("Error, map file couldn't load.");
		return false;
	}
	char readBuffer[10000];
	rapidjson::FileReadStream reader(filePointer, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(reader);
	if (!validateMap(d))
		return false;

	// Read in the tiles array
	std::map<int, Tile> tiles;
	std::map<int, std::vector<NodeTemplate>> nodes;
	const rapidjson::Value& tileArray = d["tiles"];
	if (!tileArray.IsArray()) {
		printf("Error, map file is improperly defined.");
		return false;
	}
	for (rapidjson::SizeType i = 0; i < tileArray.Size(); i++) {
		int id = tileArray[i]["id"].GetInt();
		std::string ground = tileArray[i]["ground"].GetString();
		Tile t;
		t.groundModel = ground;
		const rapidjson::Value& entityArray = tileArray[i]["entities"];
		for (rapidjson::SizeType j = 0; j < entityArray.Size(); j++) {
			std::string model = entityArray[j]["model"].GetString();
			double x = 0;
			double y = 0;
			double z = 0;
			if (entityArray[j].HasMember("x"))
				x = entityArray[j]["x"].GetDouble();
			if (entityArray[j].HasMember("y"))
				y = entityArray[j]["y"].GetDouble();
			if (entityArray[j].HasMember("z"))
				z = entityArray[j]["z"].GetDouble();


			TileEntity e;
			e.model = model;
			e.position = glm::vec3(x, y, z);
			t.entities.push_back(e);
		}

		const rapidjson::Value& nodeArray = tileArray[i]["nodes"];
		std::vector<NodeTemplate> tileNodes;
		// Setting positions
		for (rapidjson::SizeType j = 0; j < nodeArray.Size(); j++) 
		{
			NodeTemplate current;

			double x, z;
			if (nodeArray[j].HasMember("x"))
				x = nodeArray[j]["x"].GetDouble();
			if (nodeArray[j].HasMember("z"))
				z = nodeArray[j]["z"].GetDouble();

			current.position = glm::vec3(x,0,z);
			const rapidjson::Value& neighboursArray = nodeArray[j]["neighbours"];
			for (rapidjson::SizeType k = 0; k < neighboursArray.Size(); k++) 
			{
				int index = neighboursArray[k].GetInt();
				current.neighbours.push_back(index);
			}

			tileNodes.push_back(current);
		}
		nodes[id] = tileNodes;
		tiles[id] = t;
	}

	// Construct the map 
	int tileSize = d["map"]["tile size"].GetInt();
	map.tileSize = tileSize;
	const rapidjson::Value& mapTilesArray = d["map"]["tiles"];
	for (rapidjson::SizeType i = 0; i < mapTilesArray.Size(); i++) {
		const rapidjson::Value& row = mapTilesArray[i];
		std::vector<Tile> rowTiles;
		for (rapidjson::SizeType j = 0; j < row.Size(); j++) {
			int id = row[j].GetInt();
			std::vector<NodeTemplate> tileNodes = nodes[id];
			Tile t = tiles[id];

			// Positions of nodes
			for(NodeTemplate n : tileNodes)
			{
				// changing relative coordinates to global coordinates
				glm::vec3 pos = n.position;
				pos *= tileSize;
				pos.x += (tileSize * j);
				pos.z += (tileSize * i); 

				graphNode * newNode = new graphNode();
				newNode->setPosition(pos);
				t.nodes.push_back(newNode);
			}
			// Local Connections
			for(int k = 0; k < tileNodes.size(); k++)
			{
				NodeTemplate current = tileNodes.at(k);
				for(int l = 0; l < current.neighbours.size(); l++)
				{
					int index = current.neighbours[l];
					t.nodes.at(k)->addNeighbour(t.nodes.at(index));
				}
			}

			rowTiles.push_back(t);
		}
		map.tiles.push_back(rowTiles);
	}

	// Add connections for graph nodes between tiles
	std::vector<graphNode*> allNodes;
	for(std::vector<Tile> vt : map.tiles)
	{
		for(Tile t : vt)
		{
			allNodes.insert(allNodes.begin(), t.nodes.begin(), t.nodes.end());
		}
	}
	for(int i = 0; i < allNodes.size(); i++)
	{
		graphNode * current = allNodes.at(i);

		for(int j = i+1; j < allNodes.size(); j++)
		{
			graphNode * comparing = allNodes.at(j);

			if(current->sameLocation(comparing))
			{
				// May need to actually merge these instead of just connecting them
				current->addNeighbour(comparing);
				comparing->addNeighbour(current);
			}
		}
	}


	map.allNodes.insert(map.allNodes.end(), allNodes.begin(), allNodes.end());
	fclose(filePointer);
	return true;
}

bool ContentLoading::loadOBJ(
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

	FILE * file; 
	errno_t err = fopen_s(&file, path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
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

	fclose(file);
	return true;
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII
GLuint ContentLoading::loadDDS(const char * imagepath)
{

	unsigned char header[124];

	/* try to open the file */ 
	FILE* fp;
	errno_t err = fopen_s(&fp, imagepath, "rb"); 
	if (fp == NULL){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); 
		return 0;
	}
   
	/* verify the type of file */ 
	char filecode[4]; 
	fread(filecode, 1, 4, fp); 
	if (strncmp(filecode, "DDS ", 4) != 0) { 
		fclose(fp); 
		return 0; 
	}
	
	/* get the surface desc */ 
	fread(&header, 124, 1, fp); 

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);
 
	unsigned char * buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */ 
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
	int read = fread(buffer, 1, bufsize, fp); 
	/* close the file pointer */ 
	fclose(fp);

	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	unsigned int format;
	switch(fourCC) 
	{ 
	case FOURCC_DXT1: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
		break; 
	case FOURCC_DXT3: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
		break; 
	case FOURCC_DXT5: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
		break; 
	default: 
		free(buffer); 
		return 0; 
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //uncomment to disable mipmaps

	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	unsigned int offset = 0;

	/* load the mipmaps */ 
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
			0, size, buffer + offset); 
	 
		offset += size; 
		width  /= 2; 
		height /= 2; 

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if(width < 1) width = 1;
		if(height < 1) height = 1;

	} 

	free(buffer); 
	fclose(fp);

	return textureID;
}
}