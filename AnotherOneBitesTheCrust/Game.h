/*
  The class responsible for the game state, entity hierarchy, and game loop. Right now only worries about setting up a basic test
  entity and the rendering engine, to test out glut and passing between systems.
*/

#pragma once
#include "RenderingEngine.h"
#include "TestEntity.h"

class Game
{
public:
	static Game* GetInstance();
	static void IdleCallback();
private:
	Game(void);
	~Game(void);
	Game(Game const&); // Don't implement default copy constructor
	void operator=(Game const&); // Don't implement assignment operator

public:
	// member variables
	RenderingEngine *renderingEngine;
	vector<TestEntity*> entities;

	// member functions
	 void Run(int argc, char* argv[]);
	 void MainLoop();
};

