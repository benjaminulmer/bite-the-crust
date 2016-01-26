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

class Game
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
	 void mainLoop();
	 void processSDLEvents();
	 void quitGame();
};

