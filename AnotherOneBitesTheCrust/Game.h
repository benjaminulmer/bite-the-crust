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

	Renderable * pizza;

private:
	static const unsigned int PHYSICS_STEP_MS = 16;
	static const int MAX_PLAYERS = 4;

	void initSystems();
	void setupEntities();
	void setupVehicle(Vehicle* vehicle, physx::PxTransform transform, int num);
	void connectSystems();
	void mainLoop();
	void processSDLEvents();
	void quitGame();
	void endGame(std::map<Vehicle*, int> scores);
	void shootPizza(Vehicle* vehicle);

	// Temporary
	void Game::unFuckerTheGame();

	SDL_Window* window;
	SDL_Surface* screen;
	int screenWidth;
	int screenHeight;

	SDL_Surface *text;
	
	GameState gameState;

	// TODO support for more cameras (one per player)
	Camera* camera;
	Vehicle * players [MAX_PLAYERS];

	PhysicsEntityInfo* pizzaInfo;

	std::vector<Entity*> entities;
	std::map<std::string, Renderable*> renderablesMap;
	std::map<std::string, PhysicsEntityInfo*> physicsEntityInfoMap;
	std::map<std::string, GLuint> textureMap;
	Map map;
};

