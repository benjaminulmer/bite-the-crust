/*
  The class responsible for the game state, entity hierarchy, and game loop. Right now only worries about setting up a basic test
  entity and the rendering engine, to test out glut and passing between systems.
*/

#pragma once
#include "RenderingEngine.h"
#include "PhysicsEngine.h"
#include "InputEngine.h"
#include "AIEngine.h"
#include "AudioEngine.h"
#include "Renderable.h"
#include "Entity.h"
#include "PhysicsEntityInfo.h"
#include "DeliveryManager.h"
#include "Map.h"
#include <SDL.h>
#include <glew.h>
#include <map>

enum class GameState
{
	MENU,
	PLAY,
	PAUSE,
	EXIT
};

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
	
	void run();

private:
	void initSystems();
	void setupEntities();
	void connectSystems();
	void mainLoop();
	void processSDLEvents();
	void quitGame();
	void shootPizza(Vehicle* vehicle);

	SDL_Window* window;
	SDL_Surface* screen;
	int screenWidth;
	int screenHeight;

	SDL_Surface *text;
	
	GameState gameState;
	Camera camera;
	Vehicle* p1Vehicle, *p2Vehicle;

	PhysicsEntityInfo* pizzaInfo;

	std::vector<Entity*> entities;
	std::map<std::string, Renderable*> renderablesMap;
	std::map<std::string, PhysicsEntityInfo*> physicsEntityInfoMap;
	Map map;

	// vehicle location for previous frames 
	static const int CAMERA_POS_BUFFER_SIZE = 10;
	glm::vec3 cameraPosBuffer[CAMERA_POS_BUFFER_SIZE];
	int cameraPosBufferIndex;
};

