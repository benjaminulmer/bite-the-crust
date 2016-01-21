#include "Game.h"
#include <SDL.h>

Game::Game(void)
{
}

// The entry point of the game
void Game::Run() {
	// Preload data, initialize subsystems, anything to do before entering the main loop
	MainLoop();
}

void Game::MainLoop() {
	// Cap the minimum timestep physics will use
	unsigned int minTimeStepMs = (unsigned int)(1 / 60000.0); // 60 FPS, converted to ms and trucated to int
	int simCount;
	// Limit how many simulations we'll run when it's slow, to avoid death spiral
	int maxSimulations = 5;
	unsigned int oldTimeMs = SDL_GetTicks();
	
	// Game loop
	while (true) {
		// Update the player and AI cars
		inputEngine->ProcessEvents();
		aiEngine->UpdateAI();

		// Figure out timestep and run physics
		unsigned int newTimeMs = SDL_GetTicks();
		unsigned int timeStepMs = newTimeMs - oldTimeMs;
		oldTimeMs = newTimeMs;

		// If timestep is small enough, just use it, otherwise break down into smaller chunks
		while (timeStepMs > 0.0 && simCount < maxSimulations) {
			unsigned int deltaTimeMs = min(timeStepMs, minTimeStepMs);
			physicsEngine->Simulate(deltaTimeMs);
			timeStepMs -= deltaTimeMs;
			simCount++;
		}
		simCount = 0;

		// Render
		renderingEngine->PushEntities();
		renderingEngine->Draw();
	}
}

Game::~Game(void)
{
}