// The entry point, which just sets up the Game and runs it

#include "Game.h"

int main(int argc, char *argv[]) {
	Game* game = Game::GetInstance();
	game->Run(argc, argv);
}