#include "Game.h"
#include <SDL.h>

Game::Game(void)
{
}

// The entry point of the game
void Game::run() {
	// Preload data, initialize subsystems, anything to do before entering the main loop
	SDL_Window* window = SDL_CreateWindow("AOBtC", 300, 300, 300, 300, 0);

	inputEngine = new InputEngine();

	mainLoop();
}

void Game::mainLoop() {
	// Cap the minimum timestep physics will use
	unsigned int minTimeStepMs = (unsigned int)(1000.0/60.0); // 60 FPS, converted to ms and trucated to int
	int simCount = 0;
	// Limit how many simulations we'll run when it's slow, to avoid death spiral
	int maxSimulations = 5;
	unsigned int oldTimeMs = SDL_GetTicks();
	
	// Game loop
	while (true) {
		processSDLEvents();

		// Update the player and AI cars
		inputEngine->getInput();
		aiEngine->updateAI();

		// Figure out timestep and run physics
		unsigned int newTimeMs = SDL_GetTicks();
		unsigned int timeStepMs = newTimeMs - oldTimeMs;
		oldTimeMs = newTimeMs;

		// If timestep is small enough, just use it, otherwise break down into smaller chunks
		while (timeStepMs > 0.0 && simCount < maxSimulations) {
			unsigned int deltaTimeMs = min(timeStepMs, minTimeStepMs);
			physicsEngine->simulate(deltaTimeMs);
			timeStepMs -= deltaTimeMs;
			simCount++;
		}
		simCount = 0;

		// Render
		renderingEngine->pushEntities();
		renderingEngine->draw();
	}
}

void Game::processSDLEvents() {
	SDL_Event event;
    while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			quitGame();
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION || event.type == SDL_CONTROLLERBUTTONDOWN ||
			     event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEADDED) {
			inputEngine->processControllerEvent(event);
		}
		else {
			// other events, do nothing yet
		}
	}
}

void Game::quitGame() {
	// Delete subsystems 
	delete inputEngine;

	SDL_Quit();
	exit(0);
}

Game::~Game(void)
{
}