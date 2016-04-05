#include "Game.h"
#include "PizzaBox.h"
#include "WheelEntity.h"
#include "ContentLoading.h"
#include "StaticEntity.h"
#include "DecorationEntity.h"

#include <iostream>
#include <sigslot.h>

using namespace std;

void fatalError(string errorString)
{
	cout << errorString << endl;
	cout << "Enter enter to quit...";
	cin.ignore();
	SDL_Quit();
	exit(1);
}

Game::Game(void)
{
	windowWidth = DEF_WINDOW_WIDTH;		//pro csgo resolution
	windowHeight = DEF_WINDOW_HEIGHT;
	isFullscreen = false;
	numHumans = 1;
	isVSync = true;
	gameState = GameState::MENU;

	std::random_device rd;
	generator.seed(rd());
}

void Game::setGameState(GameState state)
{
	if (state == GameState::STARTING_GAME)
	{
		numHumans = menuLogic->numPlayers;
		setupEntities();
		connectSystems();
		gameState = GameState::PLAY;
	}
	else if (state == GameState::BACK_TO_MENU)
	{
		reset();
		physicsEngine->reset();
		gameState = GameState::MENU;
	}
	else 
	{
		gameState = state;
	}
}

void Game::reset() 
{
	entities.erase(entities.begin(), entities.end());
}

// The entry point of the game
void Game::run()
{
	// Preload data, initialize subsystems, anything to do before entering the main loop
	initSystems();
	loadJSONfiles();

	inputEngine->menuInput.connect(menuLogic, &MenuLogic::menuInput);
	inputEngine->pauseInput.connect(menuLogic, &MenuLogic::pauseInput);
	inputEngine->setGameState.connect(this, &Game::setGameState);
	menuLogic->gameStateSelected.connect(this, &Game::setGameState);

	mainLoop();
}

// Initialize all subsystems for the game
void Game::initSystems()
{
	SDL_Init(SDL_INIT_EVERYTHING);		//Initialize SDL
	SDL_ShowCursor(0);
	SDL_Rect rect;
	SDL_GetDisplayBounds(0, &rect);
	displayWidth = rect.w;
	displayHeight = (displayWidth * 9)/16;
	window = SDL_CreateWindow("Another One Bites the Crust", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);
	SDL_GL_SetSwapInterval(1);
	glContext = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	aiEngine = new AIEngine();
	audioEngine = new AudioEngine();
	inputEngine = new InputEngine();
	physicsEngine = new PhysicsEngine();
	renderingEngine = new RenderingEngine();
	menuLogic = new MenuLogic(renderingEngine);
	renderingEngine->loadProjectionMatrix(windowWidth, windowHeight);
	deliveryManager = new DeliveryManager();
}

void Game::loadJSONfiles()
{
	// Load data for entities
	if (!ContentLoading::loadEntityList("res\\JSON\\entityList.json", renderablesMap, physicsEntityInfoMap, textureMap))
	{
		fatalError("Could not load entities list.");
	}

	// Assign the buffers for all the renderables
	std::map<std::string, Renderable*>::iterator it;
	for (it = renderablesMap.begin(); it != renderablesMap.end(); ++it)
	{
		renderingEngine->assignBuffersTex(it->second);
	}

	// Load the map
	if (!ContentLoading::loadMap("res\\JSON\\tiles.json", "res\\JSON\\map.json", map))
	{
		fatalError("Could not load map file.");
	}
}

Tile* Game::setupTile(int i, int j)
{
	Tile* tile = &map.tiles[i][j];

	if (tile->deliverable)
	{
		deliveryManager->addDeliveryLocation(tile);
		map.deliveryTiles.push_back(tile);
	}
	if (tile->pickup)
	{
		// TODO make this better/less hardcoded
		physicsEngine->createPizzaPickup(physx::PxVec3((float)j*map.tileSize + map.tileSize/2, 0, (float)i*map.tileSize + map.tileSize/2), 8.0f);
	}

	StaticEntity* ground = new StaticEntity();
	ground->setRenderable(renderablesMap[tile->groundModel]);
	ground->setTexture(textureMap[tile->groundModel]);

	// Offset by tileSize/2 so that the corner of the map starts at 0,0
	glm::vec3 pos = glm::vec3(j*map.tileSize + map.tileSize/2, 0, i*map.tileSize + map.tileSize/2);

	float rotationRad = physx::PxPi * (tile->groundRotationDeg / 180.0f);
	physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rotationRad, physx::PxVec3(0, 1, 0)));

	physicsEngine->createEntity(ground, physicsEntityInfoMap[tile->groundModel], transform);
	tile->ground = ground;
	tile->groundTexture = textureMap[tile->groundModel];
	entities.push_back(ground);

	return tile;
}

void Game::setupRegularEntity(std::string name, Tile* tile, glm::vec3 pos)
{
	DecorationEntity* e = new DecorationEntity();
	e->setRenderable(renderablesMap[name]);
	e->setTexture(textureMap[name]);

	e->rotate(glm::radians((float)tile->groundRotationDeg), glm::vec3(0,1,0));
	e->translate(pos);
	entities.push_back(e);
}

void Game::setupPhysicsEntity(std::string name, Tile* tile, TileEntity tileEntity, glm::vec3 pos)
{
	PhysicsEntity* e;
	if (physicsEntityInfoMap[name]->type == PhysicsType::DYNAMIC)
	{
		e = new DynamicEntity();
	} 
	else
	{
		e = new StaticEntity();
		tile->staticEntities.push_back(e);
	}

	// TODO, error check that these models do exist, instead of just break
	e->setRenderable(renderablesMap[name]);
	e->setTexture(textureMap[name]);

	std::uniform_int_distribution<int> rotationDist(tileEntity.lowerRotation, tileEntity.upperRotation);
	float rotationRad = physx::PxPi * (rotationDist(generator) / 180.0f);
	physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rotationRad, physx::PxVec3(0, 1, 0)));

	physicsEngine->createEntity(e, physicsEntityInfoMap[name], transform);
	entities.push_back(e);

	if (name.find("house") != std::string::npos)
	{
		tile->house = e;
	}
}

// Create and initialize all loaded entities in the game world
void Game::setupEntities()
{
	deliveryManager->map = &map;

	// Create all the entities loaded in the map
	for (unsigned int i = 0; i < map.tiles.size(); i++)
	{
		for (unsigned int j = 0; j < map.tiles[i].size(); j++)
		{
			Tile* tile = setupTile(i, j);
			for (unsigned int k = 0; k < tile->entityTemplates.size(); k++)
			{		
				TileEntity tileEntity = tile->entityTemplates[k];
				std::uniform_int_distribution<int> nameDist(0, tileEntity.names.size()-1);
				std::string name = tileEntity.names[nameDist(generator)];

				// Offset position based on what tile we're in
				glm::vec3 pos = tileEntity.position + glm::vec3(j * map.tileSize, 0, i * map.tileSize);
				(physicsEntityInfoMap.count(name) == 0) ? setupRegularEntity(name, tile, pos) : setupPhysicsEntity(name, tile, tileEntity, pos);
			}
		}
	}
	renderingEngine->setupMinimap(map);

	// Create vehicles
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		players[i] = new Vehicle(PHYSICS_STEP_MS);
		float rotationRad = physx::PxPi * 0.5f;
		setupVehicle(players[i], physx::PxTransform(physx::PxVec3(20, 2, 135 - 15.0*i), physx::PxQuat(rotationRad, physx::PxVec3(0,1,0))), i);

		camera[i] = new Camera(players[i]);

		// TODO: get info from menu selection (ie. number of player characters)

	}
	// hard code textures for now
	players[0]->houseTexture = ContentLoading::loadDDS("res\\Textures\\houseRed.DDS");
	players[1]->houseTexture = ContentLoading::loadDDS("res\\Textures\\houseBlue.DDS");
	players[2]->houseTexture = ContentLoading::loadDDS("res\\Textures\\houseGreen.DDS");
	players[3]->houseTexture = ContentLoading::loadDDS("res\\Textures\\houseYellow.DDS");
}

void Game::setupVehicle(Vehicle* vehicle, physx::PxTransform transform, int num)
{
	ContentLoading::loadVehicleData("res\\JSON\\car.json", vehicle);
	switch(num) {
		case 0:
			vehicle->setRenderable(renderablesMap["redVan"]);
			vehicle->setTexture(textureMap["redVan"]);
			vehicle->color = glm::vec3(1,0,0);
			vehicle->pizzaBoxRenderName = "pizzaBoxRed";
			vehicle->colorName = "Red";
			break;
		case 1:
			vehicle->setRenderable(renderablesMap["blueVan"]);
			vehicle->setTexture(textureMap["blueVan"]);
			vehicle->color = glm::vec3(0,0,1);
			vehicle->pizzaBoxRenderName = "pizzaBoxBlue";
			vehicle->colorName = "Blue";
			break;
		case 2:
			vehicle->setRenderable(renderablesMap["greenVan"]);
			vehicle->setTexture(textureMap["greenVan"]);
			vehicle->color = glm::vec3(0,1,0);
			vehicle->pizzaBoxRenderName = "pizzaBoxGreen";
			vehicle->colorName = "Green";
			break;
		case 3:
			vehicle->setRenderable(renderablesMap["yellowVan"]);
			vehicle->setTexture(textureMap["yellowVan"]);
			vehicle->color = glm::vec3(1,1,0);
			vehicle->pizzaBoxRenderName = "pizzaBoxYellow";
			vehicle->colorName = "Yellow";
			break;
	}
	physicsEngine->createVehicle(vehicle, transform);
	entities.push_back(vehicle);

	physx::PxShape* wheels[4];
	physx::PxRigidActor* actor = vehicle->getActor();
	actor->getShapes(wheels, 4);

	for (unsigned int i = 0; i < 4; i++)
	{
		WheelEntity* wheel = new WheelEntity(vehicle, wheels[i]);
		wheel->setRenderable(renderablesMap["wheel"]);
		wheel->setTexture(textureMap["wheel"]);
		entities.push_back(wheel);
	}
}

// Connects systems together
void Game::connectSystems()
{
	for (int i = 0; i < numHumans; i++) 
	{
		inputEngine->setInputStruct(&players[i]->input, i);
		inputEngine->setCamera(camera[i], i);
		players[i]->isAI = false;
	}

	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		players[i]->shootPizzaSignal.connect(this, &Game::shootPizza);
		players[i]->shootPizzaSignal.connect(audioEngine, &AudioEngine::playCannonSound);
		players[i]->dryFireSignal.connect(audioEngine, &AudioEngine::playDryFireSound);
		players[i]->brakeSignal.connect(audioEngine, &AudioEngine::playBrakeSound);
		players[i]->idleSignal.connect(audioEngine, &AudioEngine::playEngineIdleSound);
		players[i]->gasSignal.connect(audioEngine, &AudioEngine::playEngineRevSound);

		deliveryManager->addPlayer(players[i]);
	}

	// TODO: Should have a textures array or something that corresponds to each player so we can add this to above loop
	deliveryManager->deliveryTextures[players[0]] = ContentLoading::loadDDS("res\\Textures\\lawnRed.DDS");
	deliveryManager->deliveryTextures[players[1]] = ContentLoading::loadDDS("res\\Textures\\lawnBlue.DDS");
	deliveryManager->deliveryTextures[players[2]] = ContentLoading::loadDDS("res\\Textures\\lawnBlue.DDS");
	deliveryManager->deliveryTextures[players[3]] = ContentLoading::loadDDS("res\\Textures\\lawnBlue.DDS");

	deliveryManager->gameOverSignal.connect(this, &Game::endGame);
	deliveryManager->houseColorSignal.connect(renderingEngine, &RenderingEngine::updateHouseColor);

	deliveryManager->assignDeliveries();
	physicsEngine->simulationCallback->collision.connect(audioEngine, &AudioEngine::playCollisionSound);

	physicsEngine->simulationCallback->pizzaBoxSleep.connect(deliveryManager, &DeliveryManager::pizzaLanded);
	physicsEngine->simulationCallback->inPickUpLocation.connect(deliveryManager, &DeliveryManager::refillPizza);
	deliveryManager->pizzasRefilled.connect(audioEngine, &AudioEngine::playReloadSound);
}

void Game::gameDisplay(int player)
{
	renderingEngine->displayFuncTex(entities);
	for(int i = 0 ; i < MAX_PLAYERS; i++)
	{
		renderingEngine->drawShadow(players[i]->getPosition());
	}
	renderingEngine->drawSkybox(players[player]->getPosition()); // TODO: See above; should render skybox for each player
}

void Game::playHUD(int player)
{
	string speed = "Speed: ";
	speed.append(to_string(players[player]->getPhysicsVehicle()->computeForwardSpeed()));
	renderingEngine->printText2D(speed.data(), 0, 690, 24);

	string frameRate = "DeltaTime: ";
	frameRate.append(to_string(deltaTimeMs));
	renderingEngine->printText2D(frameRate.data(), 0, 640, 20);

	string deltaAcc = "DeltaTimeACC: ";
	deltaAcc.append(to_string(deltaTimeAccMs));
	renderingEngine->printText2D(deltaAcc.data(), 0, 670, 20);

	string score = "Tips: $";
	score.append(to_string(deliveryManager->getScore(players[player])));
	renderingEngine->printText2D(score.data(), 1050, 690, 24);
	renderingEngine->printText2D(deliveryManager->getDeliveryText(players[player]).data(), 725, 670, 20);

	string pizzas = "Pizzas: ";
	pizzas.append(to_string(players[player]->pizzaCount));
	(players[player]->pizzaCount > 0) ? renderingEngine->printText2D(pizzas.data(), 1050, 640, 24) : renderingEngine->printText2Doutline(pizzas.data(), 990, 640, 30, glm::vec4(1,0,0,1), false);

	(numHumans == 1) ? renderingEngine->drawMinimap(players, player, windowHeight) : renderingEngine->drawMinimap(players, player, windowHeight/2); // TODO: Should support arbitrary number of vans
}

void Game::endHUD()
{
	Vehicle* winner = players[0];
	for (int i = 1; i < MAX_PLAYERS; i++)
	{
		if (scores[players[i]] > scores[winner])
			winner = players[i];
	}
	string winnerText = "	" + winner->colorName + " WINS			";
	renderingEngine->printBanner(winnerText.data(), 0, 720/2, 100, winner->color);	
	for (int i = 0; i < MAX_PLAYERS; i++) {
		string scoreText = "TIPS: $" + std::to_string(scores[players[i]]);
		renderingEngine->printBanner(scoreText.data(), 100, 300 - i*50, 50, players[i]->color);
	}
}

void Game::playLoop()
{
	// Figure out timestep and run physics
	newTimeMs = SDL_GetTicks();
	deltaTimeMs = newTimeMs - oldTimeMs;
	oldTimeMs = newTimeMs;
	deltaTimeAccMs += deltaTimeMs;

	// slipscreen testing
	if (numHumans == 1)
	{
		renderingEngine->updateView(*camera[0]);
		renderingEngine->setTextResolution(windowWidth, windowHeight);
		gameDisplay(0);
		playHUD(0);
	}
	else if (numHumans == 2)
	{
		glViewport(0, windowHeight/2, windowWidth, windowHeight/2);
		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight/2);
		renderingEngine->updateView(*camera[0]);
		renderingEngine->setTextResolution(windowWidth*2, windowHeight);
		gameDisplay(0);
		playHUD(0);

		glViewport(0, 0, windowWidth, windowHeight/2);
		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight/2);
		renderingEngine->updateView(*camera[1]);
		gameDisplay(1);
		playHUD(1);
	}
	else if (numHumans == 3)
	{
		glViewport(0, windowHeight/2, windowWidth, windowHeight/2);
		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight/2);
		renderingEngine->updateView(*camera[0]);
		renderingEngine->setTextResolution(windowWidth*2, windowHeight);
		gameDisplay(0);
		playHUD(0);

		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight);

		glViewport(0, 0, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[1]);
		renderingEngine->setTextResolution(windowWidth, windowHeight);
		gameDisplay(1);
		playHUD(1);

		glViewport(windowWidth/2, 0, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[2]);
		renderingEngine->setTextResolution(windowWidth, windowHeight);
		gameDisplay(2);
		playHUD(2);
	}
	else
	{
		glViewport(0, windowHeight/2, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[0]);
		renderingEngine->setTextResolution(windowWidth, windowHeight);
		gameDisplay(0);
		playHUD(0);

		glViewport(windowWidth/2, windowHeight/2, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[1]);
		gameDisplay(1);
		playHUD(1);

		glViewport(0, 0, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[2]);
		gameDisplay(2);
		playHUD(2);

		glViewport(windowWidth/2, 0, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[3]);
		gameDisplay(3);
		playHUD(3);
	}
	renderingEngine->loadProjectionMatrix(windowWidth, windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);


	while (deltaTimeAccMs >= PHYSICS_STEP_MS)
	{
		if (gameState == GameState::END) break;
		deltaTimeAccMs -= PHYSICS_STEP_MS;
		physicsEngine->simulate(PHYSICS_STEP_MS);	
		deliveryManager->timePassed(PHYSICS_STEP_MS);

		// Update the player and AI cars and cameras
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			if(players[i]->isAI)
			{
				AICollisionEntity closest = physicsEngine->AISweep(players[i]);
				aiEngine->updateAI(players[i], deliveryManager->deliveries[players[i]], map, closest);
			}
			players[i]->update();
			camera[i]->update();
		}
		physicsEngine->fetchSimulationResults();
	}
	audioEngine->update(players[0]->getModelMatrix());
}

void Game::menuLoop()
{
	oldTimeMs = SDL_GetTicks();
	renderingEngine->setTextResolution(windowWidth, windowHeight);
	menuLogic->updateMenu();

	string instructions = "D-pad - Move, A - Select, B - Back";
	renderingEngine->printText2D(instructions.data(), 0, 0, 24);
}

void Game::pauseLoop()
{
	oldTimeMs = SDL_GetTicks();
	renderingEngine->setTextResolution(windowWidth, windowHeight);
	menuLogic->updatePaused();

	string instructions = "D-pad - Move, A - Select, B - Back";
	renderingEngine->printText2D(instructions.data(), 0, 0, 24);
}

void Game::endLoop()
{
	// Figure out timestep and run physics
	newTimeMs = SDL_GetTicks();
	deltaTimeMs = newTimeMs - oldTimeMs;
	oldTimeMs = newTimeMs;
	deltaTimeAccMs += deltaTimeMs;

	if (numHumans == 1)
	{
		renderingEngine->updateView(*camera[0]);
		gameDisplay(0);
		playHUD(0);
	}
	else if (numHumans == 2)
	{
		glViewport(0, windowHeight/2, windowWidth, windowHeight/2);
		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight/2);
		renderingEngine->updateView(*camera[0]);
		gameDisplay(0);

		glViewport(0, 0, windowWidth, windowHeight/2);
		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight/2);
		renderingEngine->updateView(*camera[1]);
		gameDisplay(1);
	}
	else if (numHumans == 3)
	{
		glViewport(0, windowHeight/2, windowWidth, windowHeight/2);
		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight/2);
		renderingEngine->updateView(*camera[0]);
		gameDisplay(0);

		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight);
		glViewport(0, 0, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[2]);
		gameDisplay(2);

		glViewport(windowWidth/2, 0, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[3]);
		gameDisplay(3);
	}
	else
	{
		glViewport(0, windowHeight/2, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[0]);
		gameDisplay(0);

		glViewport(windowWidth/2, windowHeight/2, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[1]);
		gameDisplay(1);

		glViewport(0, 0, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[2]);
		gameDisplay(2);

		glViewport(windowWidth/2, 0, windowWidth/2, windowHeight/2);
		renderingEngine->updateView(*camera[3]);
		gameDisplay(3);
	}
	renderingEngine->loadProjectionMatrix(windowWidth, windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	endHUD();

	while (deltaTimeAccMs >= PHYSICS_STEP_MS)
	{
		deltaTimeAccMs -= PHYSICS_STEP_MS;
		physicsEngine->simulate(PHYSICS_STEP_MS);	

		// Update the player and AI cars and cameras
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			players[i]->update();
			camera[i]->update();
		}
		physicsEngine->fetchSimulationResults();
	}
	audioEngine->update(players[0]->getModelMatrix());
}

// Main loop of the game
void Game::mainLoop()
{
	oldTimeMs = SDL_GetTicks();
	deltaTimeAccMs = 0;

	// Game loop
	while (gameState != GameState::EXIT)
	{
		processSDLEvents();

		//TODO: Skipable intros
		if (gameState == GameState::INTRO)
		{
			//renderingEngine->displayIntro(0);
			SDL_GL_SwapWindow(window);
			SDL_Delay(3000);

			//renderingEngine->displayIntro(1);
			//SDL_GL_SwapWindow(window);
			//SDL_Delay(3000);

			//renderingEngine->displayIntro(2);
			//SDL_GL_SwapWindow(window);
			//SDL_Delay(3000);

			gameState = GameState::MENU;
		}
		else if (gameState == GameState::PLAY)
		{
			playLoop();
		}
		else if (gameState == GameState::MENU)
		{
			menuLoop();
		}
		else if (gameState == GameState::PAUSE)
		{
			pauseLoop();
		}
		else if (gameState == GameState::END)
		{
			endLoop();
		}
		SDL_GL_SwapWindow(window);
	}
}

void Game::endGame(std::map<Vehicle*, int> scores) {
	gameState = GameState::END;
	this->scores = scores;
}

void Game::toggleVSync()
{
	if (isVSync) 
	{
		SDL_GL_SetSwapInterval(0);
		isVSync = false;
	}
	else
	{
		SDL_GL_SetSwapInterval(1);
		isVSync = true;
	}
}

void Game::toggleFullscreen()
{
	if (isFullscreen)
	{
		windowWidth = DEF_WINDOW_WIDTH;
		windowHeight = DEF_WINDOW_HEIGHT;
		SDL_SetWindowFullscreen(window, 0);
		SDL_SetWindowSize(window, windowWidth, windowHeight);
		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);

		isFullscreen = false;
	}
	else 
	{
		windowWidth = displayWidth;
		windowHeight = displayHeight;
		SDL_SetWindowSize(window, windowWidth, windowHeight);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

		renderingEngine->loadProjectionMatrix(windowWidth, windowHeight);
		glViewport(0, 0, windowWidth, windowHeight);

		isFullscreen = true;
	}
}

// Loop over all SDL events since last frame and call appropriate for each type of event
void Game::processSDLEvents()
{
	SDL_Event event;
    while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			gameState = GameState::EXIT;
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION) 
		{
			inputEngine->controllerAxisMotion(event, gameState);
		}
		else if (event.type == SDL_CONTROLLERBUTTONDOWN)
		{
			inputEngine->controllerButtonDown(event, gameState);
		}
		else if (event.type == SDL_CONTROLLERBUTTONUP)
		{
			inputEngine->controllerButtonUp(event, gameState);
		}
		else if (event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEADDED)
		{
			inputEngine->openControllers();
		}
		else if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				gameState = GameState::EXIT;
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_V)
			{
				toggleVSync();
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_F)
			{
				toggleFullscreen();
			}
		}
	}
}

// Creates and fires a pizza from the provided vehicle
void Game::shootPizza(Vehicle* vehicle)
{
	PizzaBox* pizzaBox = new PizzaBox(vehicle);
	pizzaBox->setRenderable(renderablesMap[vehicle->pizzaBoxRenderName]);
	pizzaBox->setTexture(textureMap[vehicle->pizzaBoxRenderName]);

	physx::PxTransform transform = vehicle->getActor()->getGlobalPose();
	physx::PxVec3 posOffset = transform.rotate(physx::PxVec3(0.0f, 1.25f, 1.0f));
	transform.p += posOffset;

	physx::PxVec3 velocity = transform.rotate(physx::PxVec3(0.0f, 2, 21));
	physx::PxVec3 vehicleVelocity = vehicle->getRigidDynamic()->getLinearVelocity();
	velocity += vehicleVelocity;

	physicsEngine->createEntity(pizzaBox, physicsEntityInfoMap[vehicle->pizzaBoxRenderName], transform);
	pizzaBox->getRigidDynamic()->setLinearVelocity(velocity);
	pizzaBox->getActor()->setActorFlag(physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
	entities.push_back(pizzaBox);
}

Game::~Game(void)
{
	/*for (unsigned int i = 0; i < entities.size(); i++)
		delete entities[i];*/

	std::map<std::string, Renderable*>::iterator it;
	for (it = ContentLoading::loadedModels.begin(); it != ContentLoading::loadedModels.end(); ++it)
		delete it->second;
}
