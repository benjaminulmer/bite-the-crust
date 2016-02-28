// The entry point, which just sets up the Game and runs it

#include "Game.h"
#include <SDL.h>

int main(int argc, char *argv[]) {
	Game game;
	game.run();

	SDL_Quit();
	return 0;
} 