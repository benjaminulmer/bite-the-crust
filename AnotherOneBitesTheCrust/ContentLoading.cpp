#include "ContentLoading.h"

namespace ContentLoading {

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
	if (d.HasMember("mass")) {
		vehicle->tuning.chassisMass = (float)d["mass"].GetDouble();
	}
	vehicle->updateTuning();

	fclose(filePointer);

	return true;
}

//todo, proper error checking for the json file format, right now it just blows up 95% of the time if you got it wrong
bool loadRenderables(char* filename, std::map<std::string, Renderable*> &map) {
	FILE* filePointer;
	errno_t err = fopen_s(&filePointer, filename, "rb");
	if (err != 0) {
		printf("Error, renderables file couldn't load.");
		return false;
	}
	char readBuffer[10000];
	rapidjson::FileReadStream reader(filePointer, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(reader);
	const rapidjson::Value& renderableArray = d["renderables"];
	if (!renderableArray.IsArray()) {
		printf("Error, renderables file improperly defined.");
		return false;
	}
	for (rapidjson::SizeType i = 0; i < renderableArray.Size(); i++) {
		std::string renderableName = renderableArray[i]["name"].GetString();
		std::string renderableModelFile = renderableArray[i]["model"].GetString();
		renderableModelFile.insert(0, "res\\Models\\");
		Renderable * r = new Renderable();
		std::vector<glm::vec3> verts;
		std::vector<glm::vec3> normals;
		bool floorRes = ContentLoading::loadOBJNonIndexed(renderableModelFile.c_str(), verts, normals);
		r->setVerts(verts);
		r->setNorms(normals);
		r->setColor(glm::vec3(1.0f,1.0f,1.0f));
		map[renderableName] = r;
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

	// Read in the tiles array
	std::map<int, Tile> tiles;
	const rapidjson::Value& tileArray = d["tiles"];
	if (!tileArray.IsArray()) {
		printf("Error, map file is improperly defined.");
		return false;
	}
	for (rapidjson::SizeType i = 0; i < tileArray.Size(); i++) {
		int id = tileArray[i]["id"].GetInt();
		Tile t;
		const rapidjson::Value& entityArray = tileArray[i]["entities"];
		for (rapidjson::SizeType j = 0; j < entityArray.Size(); j++) {
			std::string model = entityArray[j]["model"].GetString();
			double x = entityArray[j]["x"].GetDouble();
			double z = entityArray[j]["z"].GetDouble();
			TileEntity e;
			e.model = model;
			e.position = glm::vec3(x, 0, z);
			t.entities.push_back(e);
		}
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
			rowTiles.push_back(tiles[id]);
		}
		map.tiles.push_back(rowTiles);
	}

	return true;
}

bool ContentLoading::loadOBJNonIndexed(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	//std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	//std::vector<glm::vec2> temp_uvs;
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
		int res = fscanf_s(file, "%s", lineHeader, sizeof(lineHeader));
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		//}else if ( strcmp( lineHeader, "vt" ) == 0 ){
		//	glm::vec2 uv;
		//	fscanf(file, "%f %f\n", &uv.x, &uv.y );
		//	uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
		//	temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
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
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		//glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		//out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}

	return true;
}


}