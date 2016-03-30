#include "Game.h"
#include "PizzaBox.h"
#include "WheelEntity.h"
#include "ContentLoading.h"
#include "StaticEntity.h"

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
	window = nullptr;
	screenWidth = 1280;		//pro csgo resolution
	screenHeight = 720;
	gameState = GameState::MENU;
	renderingEngine = nullptr;
	physicsEngine = nullptr;
	inputEngine = nullptr;
	aiEngine = nullptr;
	audioEngine = nullptr;
	screen = nullptr;
	deliveryManager = nullptr;
	std::random_device rd;
	generator.seed(rd());
}

void Game::setGameState(GameState state)
{
	gameState = state;
}

// The entry point of the game
void Game::run()
{
	// Preload data, initialize subsystems, anything to do before entering the main loop
	initSystems();
	loadJSONfiles();
	setupEntities();
	connectSystems();

	mainLoop();
}

// Initialize all subsystems for the game
void Game::initSystems()
{
	SDL_Init(SDL_INIT_EVERYTHING);		//Initialize SDL
	window = SDL_CreateWindow("Another One Bites the Crust", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	if(window == nullptr)
	{
		fatalError("SDL Window could not be created");
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if(glContext == nullptr)
	{
		fatalError("SDL_GL context could not be created");
	}

	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if(error != GLEW_OK)
	{
		fatalError("Could not init GLEW");
	}
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);		//enable double buffering
	if( SDL_GL_SetSwapInterval( 1 ) < 0 )
	{
		printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	aiEngine = new AIEngine();
	audioEngine = new AudioEngine();
	inputEngine = new InputEngine();
	physicsEngine = new PhysicsEngine();
	renderingEngine = new RenderingEngine();
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
	Entity* e = new Entity();
	e->setRenderable(renderablesMap[name]);
	e->setTexture(textureMap[name]);

	e->setDefaultRotation(physx::PxPi *(tile->groundRotationDeg) / 180.0f, glm::vec3(0,1,0));
	e->setDefaultTranslation(pos);
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

		// TODO: get info from menu selection (ie. number of player characters)
		if(i > 0)
			players[i]->isAI = true;
		else
			players[i]->isAI = false;
	}
	// hard code textures for now
	players[0]->houseTexture = ContentLoading::loadDDS("res\\Textures\\houseRed.DDS");
	players[1]->houseTexture = ContentLoading::loadDDS("res\\Textures\\houseBlue.DDS");
	players[2]->houseTexture = ContentLoading::loadDDS("res\\Textures\\houseGreen.DDS");
	players[3]->houseTexture = ContentLoading::loadDDS("res\\Textures\\houseYellow.DDS");

	camera = new Camera(players[0]);
	renderingEngine->updateView(*camera);
}

void Game::setupVehicle(Vehicle* vehicle, physx::PxTransform transform, int num)
{
	ContentLoading::loadVehicleData("res\\JSON\\car.json", vehicle);
	vehicle->setDefaultRotation(-1.5708f, glm::vec3(0,1,0));
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
		wheel->setDefaultRotation(-1.5708f, glm::vec3(0,1,0));
		entities.push_back(wheel);
	}
	
}

// Connects systems together
void Game::connectSystems()
{
	inputEngine->menuInput.connect(renderingEngine, &RenderingEngine::menuInput);
	inputEngine->pauseInput.connect(renderingEngine, &RenderingEngine::pauseInput);
	inputEngine->endInput.connect(renderingEngine, &RenderingEngine::endInput);

	inputEngine->setInputStruct(&players[0]->input, 0);
	inputEngine->setCamera(camera, 0);

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
	deliveryManager->deliveryLocationUpdate.connect(renderingEngine, &RenderingEngine::updateDeliveryLocation);
	deliveryManager->houseColorSignal.connect(renderingEngine, &RenderingEngine::updateHouseColor);

	deliveryManager->assignDeliveries();
	physicsEngine->simulationCallback->collision.connect(audioEngine, &AudioEngine::playCollisionSound);
	physicsEngine->simulationCallback->pizzaBoxSleep.connect(deliveryManager, &DeliveryManager::pizzaLanded);
	physicsEngine->simulationCallback->inPickUpLocation.connect(deliveryManager, &DeliveryManager::refillPizza);
	deliveryManager->pizzasRefilled.connect(audioEngine, &AudioEngine::playReloadSound);

	inputEngine->pausePressed.connect(this, &Game::setGameState);
	renderingEngine->gameStateSelected.connect(this, &Game::setGameState);
}

// Main loop of the game
void Game::mainLoop()
{
	unsigned int oldTimeMs = SDL_GetTicks();
	unsigned int deltaTimeAccMs = 0;

	// Game loop
	while (gameState != GameState::EXIT)
	{
		processSDLEvents();

		//TODO: Skipable intros
		if (gameState == GameState::INTRO)
		{
			renderingEngine->displayIntro(0);
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
		else if(gameState == GameState::PLAY)
		{
			// Figure out timestep and run physics
			unsigned int newTimeMs = SDL_GetTicks();
			unsigned int deltaTimeMs = newTimeMs - oldTimeMs;
			oldTimeMs = newTimeMs;

			deltaTimeAccMs += deltaTimeMs;
			while (deltaTimeAccMs >= PHYSICS_STEP_MS)
			{
				deltaTimeAccMs -= PHYSICS_STEP_MS;
				deliveryManager->timePassed(PHYSICS_STEP_MS);

				// Update the player and AI cars
				for(int i = 0; i < MAX_PLAYERS; i++)
				{
					if(players[i]->isAI)
					{
						AICollisionEntity closest = physicsEngine->AISweep(players[i]);
						aiEngine->updateAI(players[i], deliveryManager->deliveries[players[i]], map, closest);
					}
					players[i]->update();
				}
				physicsEngine->simulate(PHYSICS_STEP_MS);	
				physicsEngine->fetchSimulationResults();
				camera->update();
			}
			
			renderingEngine->updateView(*camera);

			// Update Sound
			// TODO: update audioengine to support multiple listeners
			audioEngine->update(players[0]->getModelMatrix());

			// Display
			renderingEngine->displayFuncTex(entities);
			for(int i = 0 ; i < MAX_PLAYERS; i++)
			{
				renderingEngine->drawShadow(players[i]->getPosition());
			}
			renderingEngine->drawSkybox(players[0]->getPosition()); // TODO: See above; should render skybox for each player
			renderingEngine->drawMinimap(players); // TODO: Should support arbitrary number of vans

			// TODO: Broken record, but should draw to corresponding player's viewport
			string speed = "Speed: ";
			speed.append(to_string(players[0]->getPhysicsVehicle()->computeForwardSpeed()));
			renderingEngine->printText2D(speed.data(), 0, 690, 24);

			string frameRate = "DeltaTime: ";
			frameRate.append(to_string(deltaTimeMs));
			renderingEngine->printText2D(frameRate.data(), 0, 640, 20);

			string deltaAcc = "DeltaTimeACC: ";
			deltaAcc.append(to_string(deltaTimeAccMs));
			renderingEngine->printText2D(deltaAcc.data(), 0, 670, 20);

			string score = "Tips: $";
			score.append(to_string(deliveryManager->getScore(players[0])));
			renderingEngine->printText2D(score.data(), 1050, 690, 24);
			renderingEngine->printText2D(deliveryManager->getDeliveryText(players[0]).data(), 725, 670, 20);

			string pizzas = "Pizzas: ";
			pizzas.append(to_string(players[0]->pizzaCount));
			if(players[0]->pizzaCount > 0)
			{
				renderingEngine->printText2D(pizzas.data(), 1050, 640, 24);
			}
			else
			{
				renderingEngine->printText2Doutline(pizzas.data(), 990, 640, 30, glm::vec4(1,0,0,1), false);
			}
			renderingEngine->drawDelivery();

			//swap buffers
			SDL_GL_SwapWindow(window);
			
		}
		else if(gameState == GameState::MENU)
		{
			oldTimeMs = SDL_GetTicks();
			renderingEngine->updateMenu();
			renderingEngine->displayMenu();

			string instructions = "D-pad - Move, A - Select, B - Back";
			renderingEngine->printText2D(instructions.data(), 0, 0, 24);

			SDL_GL_SwapWindow(window);
		}
		else if(gameState == GameState::PAUSE)
		{
			oldTimeMs = SDL_GetTicks();
			renderingEngine->updatePaused();
			renderingEngine->displayPause();

			string instructions = "D-pad - Move, A - Select, B - Back";
			renderingEngine->printText2D(instructions.data(), 0, 0, 24);
			SDL_GL_SwapWindow(window);
		}
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
		else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
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
		else
		{
			// other events, do nothing yet
		}
	}
}

void Game::endGame(std::map<Vehicle*, int> scores) {
	gameState = GameState::END;
	while(gameState != GameState::EXIT)
	{
		processSDLEvents();
		renderingEngine->displayFuncTex(entities);
		for(int i = 0 ; i < MAX_PLAYERS; i++)
			renderingEngine->drawShadow(players[i]->getPosition());

		renderingEngine->drawSkybox(players[0]->getPosition());
		renderingEngine->drawMinimap(players); 

		Vehicle* winner = players[0];
		for (int i = 1; i < MAX_PLAYERS; i++) {
			if (scores[players[i]] > scores[winner])
				winner = players[i];
		}
		string winnerText = "	" + winner->colorName + " WINS			";
		renderingEngine->printBanner(winnerText.data(), 0, 720/2, 100, winner->color);
		for (int i = 0; i < MAX_PLAYERS; i++) {
			string scoreText = "TIPS: $" + std::to_string(scores[players[i]]);
			renderingEngine->printBanner(scoreText.data(), 100, 300 - i*50, 50, players[i]->color);
		}
		//press start to exit

		SDL_GL_SwapWindow(window);
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

	physx::PxVec3 velocity = transform.rotate(physx::PxVec3(0.0f, 2, 17));
	physx::PxVec3 vehicleVelocity = vehicle->getRigidDynamic()->getLinearVelocity();
	velocity += vehicleVelocity;

	physicsEngine->createEntity(pizzaBox, physicsEntityInfoMap[vehicle->pizzaBoxRenderName], transform);
	pizzaBox->getRigidDynamic()->setLinearVelocity(velocity);
	pizzaBox->getActor()->setActorFlag(physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
	entities.push_back(pizzaBox);
}

Game::~Game(void)
{
	for(int i = 0 ; i < MAX_PLAYERS; i++)
		players[i]->shootPizzaSignal.disconnect_all();

	for (unsigned int i = 0; i < entities.size(); i++)
		delete entities[i];

	std::map<std::string, Renderable*>::iterator it;
	for (it = ContentLoading::loadedModels.begin(); it != ContentLoading::loadedModels.end(); ++it)
		delete it->second;
}
