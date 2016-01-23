// The entry point, which just sets up the Game and runs it

#include "Game.h"
#include <SDL.h>
#include <iostream>

int main(int argc, char *argv[]) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	Game game;
	game.Run();

	SDL_Quit();
	return 0;
}