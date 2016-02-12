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
#include "Camera.h"
#include "ContentLoading.h"

#include <SDL.h>
#include <iostream>
#include <string>
#include <GL\glew.h>
#include <sigslot.h>

using namespace std;

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

	RenderingEngine *renderingEngine;
	PhysicsEngine *physicsEngine;
	InputEngine *inputEngine;
	AIEngine *aiEngine;
	AudioEngine *audioEngine;

	void run();

private:
	void initSystems();
	void setupEntities();
	void connectSignals();
	void mainLoop();
	void processSDLEvents();
	void quitGame();
	void firePizza();

	SDL_Window* window;
	int screenWidth;
	int screenHeight;

	GameState gameState;
	Camera camera;
	Vehicle* playerVehicle;
	std::vector<Entity*> entities;
	std::vector<Renderable*> renderables;
};

