// The entry point, which just sets up the Game and runs it

#include "Game.h"
#include <SDL.h>
#include <iostream>
#include <filereadstream.h>
#include <document.h>

int main(int argc, char *argv[]) {
	Game game;
	game.run();

	return 0;
} 