#include "Game.h"
#include <GL/glut.h>


Game::Game(void)
{
	// Set up the game state
	renderingEngine = RenderingEngine::GetInstance();
	// We'll be preloading actual data, but for now I'm just defining examples by hand
	TestEntity *cube = new TestEntity;
	cube->vertices.push_back(Vector3f(-0.5, -0.5, 0.0));
	cube->vertices.push_back(Vector3f(-0.5, 0.5, 0.0));
	cube->vertices.push_back(Vector3f(0.5, 0.5, 0.0));
	cube->vertices.push_back(Vector3f(0.5, -0.5, 0.0));
	cube->position = Vector3f(0, 0, 0);
	entities.push_back(cube);
}

// Get the singleton instance of the Game. 
//Only has to be singleton so that the loop can be a static function for the GLUT callback.
Game* Game::GetInstance() {
	static Game instance;
	return &instance;
}

// The callback for glut to use in idle, so that we can control our game loop. I'd rather we didn't need this.
void Game::IdleCallback() {
	Game::GetInstance()->MainLoop();
}

// The entry point of the game, which could take in console arguments, which we might want to pass to the renderer for GLUT
void Game::Run(int argc, char* argv[]) {
	// Initialize the engines
	renderingEngine->Initialize(argc, argv);

	// I'd like to have the main loop here, but GLUT requires control of the loop
	glutMainLoop();
}

void Game::MainLoop() {
	// This is where the time stepping would go, and the other main steps, like input and simulation
	renderingEngine->PushEntities(entities);
	glutPostRedisplay();
}

Game::~Game(void)
{
	for (int i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
}