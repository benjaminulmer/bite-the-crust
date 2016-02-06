// The entry point, which just sets up the Game and runs it

#include "Game.h"
#include <SDL.h>
#include <iostream>
#include <rapidjson\filereadstream.h>
#include <rapidjson\document.h>

int main(int argc, char *argv[]) {
	FILE* filePointer = fopen("res\\JSON\\test.json", "rb");
	if (filePointer == NULL) {
		printf("Error, file couldn't load.");
		return 0;
	}
	char readBuffer[10000];
	rapidjson::FileReadStream reader(filePointer, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(reader);
	assert(d.HasMember("hello"));
	assert(d["hello"].IsString());
	printf("hello = %s\n", d["hello"].GetString());

	Game game;
	game.run();

	fclose(filePointer);

	return 0;
} 