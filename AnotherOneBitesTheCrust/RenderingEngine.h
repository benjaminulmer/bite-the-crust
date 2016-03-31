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
#include <gtx/rotate_vector.hpp>

#include <glew.h>

#include "res_path.h"
#include "ShaderTools.h"
#include "Renderable.h"
#include "Camera.h"
#include "ContentLoading.h"
#include "Entity.h"
#include "GameState.h"
#include <sigslot.h>

class RenderingEngine :
	public sigslot::has_slots<>
{
public:
	RenderingEngine(void);
	~RenderingEngine(void);	

	void displayFunc(std::vector<Entity*> entities);
	void displayFuncTex(std::vector<Entity*> entities);
	glm::mat4 calculateDefaultModel(glm::mat4 model, Entity * entity);

	void generateIDs();
	void deleteIDs();
	void loadProjectionMatrix();
	void updateView(Camera& c);
	void assignBuffersTex(Renderable* r);
	void deleteBuffers(Renderable* r);

	/////////////////////////////////////////////TEXTURE TESTING//////////////////////////////////////////////////////
	void testDraw();
	void testOBJLoading();
	GLuint textureProgramID;

	std::vector <glm::vec3> textureVerts;
	std::vector <glm::vec3> textureNorms;
	std::vector <glm::vec2> textureUVs;

	GLuint Texture;
	GLuint TextureID;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////MISC////////////////////////////////////
	GLuint shadowProgramID;
	GLuint sphereVAO;
	GLuint sphereVertBuffer;
	GLuint sphereColorBuffer;
	std::vector<glm::vec3> shadowVertices;

	GLuint skyVAO;
	GLuint skyVertBuffer;
	GLuint skyUVBuffer;
	GLuint skyNormalBuffer;
	std::vector<glm::vec3> skyVertices;
	std::vector <glm::vec3> skyNorms;
	std::vector <glm::vec2> skyUVs;
	GLuint skyTex;
	void setupMiscBuffers();
	void drawShadow(glm::vec3 position);
	void drawSkybox(glm::vec3 position);

	void initText2D(const char * texturePath);
	void printText2D(const char * text, int x, int y, int size);
	void printBanner(const char * text, int x, int y, int size, glm::vec3 color);
	void printText2Doutline(const char * text, int x, int y, int size, glm::vec4 color, bool invert);
	void cleanupText2D();

	GLuint textProgramID;			//shader
	GLuint colorID;
	GLuint textTextureID;			//texture
	GLuint Text2DVertexBufferID;	//vertex buffer
	GLuint Text2DUVBufferID;		//UV buffer

	//IDs for shaders
	GLuint mvpID;
	GLuint vID;
	GLuint mID;
	GLuint tID;
	GLuint normalID;
	GLuint lightPos;
	GLuint lightPow;
	GLuint ambientScale;
	GLint basicmvpID;

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


	GLuint textVAO;

	//minimap stuff
	void setupMinimap(Map map);
	void drawMinimap(Vehicle* vans[4]);
	void updateHouseColor(Map *map, Tile* tile, glm::vec3 color);
	void updateDeliveryLocation(glm::vec3 pos);
	void drawDelivery();
	std::vector<glm::vec3> mmRoadVerts;
	std::vector<float> mmRoadColors;
	GLuint mmRoadVAO;
	GLuint mmRoadVertBuffer;
	GLuint mmRoadColorBuffer;
	std::vector<glm::vec3> mmHouseVerts;
	std::vector<glm::vec3> mmHouseColors;
	GLuint mmHouseVAO;
	GLuint mmHouseVertBuffer;
	GLuint mmHouseColorBuffer;
	std::vector<glm::vec3> mmVanVerts;
	std::vector<float> mmVanColors;
	GLuint mmVanVAOs[4];
	GLuint mmVanVertBuffer;
	GLuint mmVanColorBuffers[4];
	std::vector<glm::vec3> mmDeliveryVerts;
	std::vector<glm::vec3> mmDeliveryColors;
	GLuint mmDeliveryVAO;
	GLuint mmDeliveryVertBuffer;
	GLuint mmDeliveryColorBuffer;
	
	glm::mat4 mmV;
	glm::mat4 mmM;
	glm::vec3 mmScale;
	glm::vec3 mmCenter;
	glm::vec3 shift;
	glm::vec3 deliveryPosition;

	//testing stuf for danny
	void setupNodes(std::vector <glm::vec3> verts, glm::vec3 color);
	void drawNodes(int size, std::string style);
	GLuint nodeVAO;
	GLuint nodeVertBuffer;
	GLuint nodeColorBuffer;

	//intro stuff
	void setupIntro();
	void displayIntro(int index);
	void displayMenu();
	void displayPause();
	std::vector<Entity*> introEntities;
	std::vector<Entity*> menuEntities;
	std::vector<Entity*> pausedEntities;
	glm::mat4 introM;
	glm::mat4 introV;

	void menuInput(InputType type);
	void pauseInput(InputType type);
	void updateMenu();
	void updatePaused();
	GLuint selected;
	GLuint unselected;

	int currentMenuSelection;
	int currentPauseSelection;
	int menuState;

	sigslot::signal1<GameState> gameStateSelected;
};

