#pragma once
#include "RenderingEngine.h"
#include "PhysicsEngine.h"
#include "InputEngine.h"
#include "AIEngine.h"
#include "AudioEngine.h"
#include "Renderable.h"
#include "PhysicsEntityInfo.h"
#include "DeliveryManager.h"
#include "Map.h"
#include "GameState.h"

#include <SDL.h>
#include <glew.h>
#include <map>
#include <string>
#include <random>

class Game :
	public sigslot::has_slots<>
{
public:
	Game(void);
	~Game(void);
	
	AIEngine *aiEngine;
	AudioEngine *audioEngine;
	InputEngine *inputEngine;
	PhysicsEngine *physicsEngine;
	RenderingEngine *renderingEngine;
	DeliveryManager* deliveryManager;
	static const int MAX_PLAYERS = 4;
	
	void run();

	Renderable * pizza;

private:
	static const unsigned int PHYSICS_STEP_MS = 16;

	void setGameState(GameState state);
	void initSystems();
	void loadJSONfiles();
	Tile* setupTile(int i, int j);
	void setupRegularEntity(std::string name, Tile* tile, glm::vec3 pos);
	void setupPhysicsEntity(std::string name, Tile* tile, TileEntity tileEntity, glm::vec3 pos);
	void setupEntities();
	void setupVehicle(Vehicle* vehicle, physx::PxTransform transform, int num);
	void connectSystems();
	void mainLoop();
	void processSDLEvents();
	void quitGame();
	void endGame(std::map<Vehicle*, int> scores);
	void shootPizza(Vehicle* vehicle);

	GameState gameState;

	SDL_Window* window;
	SDL_Surface* screen;
	int screenWidth;
	int screenHeight;
	SDL_Surface *text;

	Camera* camera;
	Vehicle * players [MAX_PLAYERS];

	PhysicsEntityInfo* pizzaInfo;
	std::vector<Entity*> entities;
	std::map<std::string, Renderable*> renderablesMap;
	std::map<std::string, PhysicsEntityInfo*> physicsEntityInfoMap;
	std::map<std::string, GLuint> textureMap;
	Map map;

	std::mt19937 generator;
};

