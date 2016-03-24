#include "Game.h"
#include "PizzaBox.h"
#include "WheelEntity.h"
#include "ContentLoading.h"
#include "StaticEntity.h"

#include <iostream>
#include <string>
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
	gameState = GameState::INTRO;
	renderingEngine = nullptr;
	physicsEngine = nullptr;
	inputEngine = nullptr;
	aiEngine = nullptr;
	audioEngine = nullptr;
	screen = nullptr;
	deliveryManager = nullptr;
}

// The entry point of the game
void Game::run()
{
	// Preload data, initialize subsystems, anything to do before entering the main loop
	initSystems();
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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				//blue background

	aiEngine = new AIEngine();
	audioEngine = new AudioEngine();
	inputEngine = new InputEngine();
	physicsEngine = new PhysicsEngine();
	renderingEngine = new RenderingEngine();
	deliveryManager = new DeliveryManager();
	renderingEngine->initText2D("res\\Fonts\\Holstein.DDS");
	renderingEngine->setupMiscBuffers();
	renderingEngine->setupIntro();

}

// Create and initialize all loaded entities in the game world
void Game::setupEntities()
{
	// Load data for entities
	if (!ContentLoading::loadEntityList("res\\JSON\\entityList.json", renderablesMap, physicsEntityInfoMap, textureMap))
		fatalError("Could not load entities list.");

	// Assign the buffers for all the renderables
	std::map<std::string, Renderable*>::iterator it;
	for (it = renderablesMap.begin(); it != renderablesMap.end(); ++it)
	{
		renderingEngine->assignBuffersTex(it->second);
	}

	// Load the map
	if (!ContentLoading::loadMap("res\\JSON\\map.json", map))
		fatalError("Could not load map file.");
	deliveryManager->map = &map;

	// Create all the entities loaded in the map
	for (unsigned int i = 0; i < map.tiles.size(); i++)
	{
		for (unsigned int j = 0; j < map.tiles[i].size(); j++)
		{
			Tile* tile = &map.tiles[i][j];

			if (tile->deliverable) {
				deliveryManager->addDeliveryLocation(tile);
			}
			if (tile->pickup) {
				// TODO make this better/less hardcoded
				physicsEngine->createPizzaPickup(physx::PxVec3((float)j*map.tileSize + map.tileSize/2, 0, (float)i*map.tileSize + map.tileSize/2), 8.0f);
			}

			Entity* ground = new Entity();
			ground->setRenderable(renderablesMap[tile->groundModel]);
			ground->setTexture(textureMap[tile->groundModel]);

			// Offset by tileSize/2 so that the corner of the map starts at 0,0 instead of -35,-35.
			ground->setDefaultRotation(physx::PxPi * (tile->groundRotationDeg) / 180.0f, glm::vec3(0,1,0));
			ground->setDefaultTranslation(glm::vec3(j*map.tileSize + map.tileSize/2, 0, i*map.tileSize + map.tileSize/2));
			tile->ground = ground;
			tile->groundTexture = textureMap[tile->groundModel];
			entities.push_back(ground);

			for (unsigned int k = 0; k < tile->entityTemplates.size(); k++)
			{
				TileEntity tileEntity = tile->entityTemplates[k];

				PhysicsEntity* e;
				if (physicsEntityInfoMap[tileEntity.name]->type == PhysicsType::DYNAMIC) {
					e = new DynamicEntity();
				} else {
					e = new StaticEntity();
					tile->staticEntities.push_back(e);
				}

				// TODO, error check that these models do exist, instead of just break
				e->setRenderable(renderablesMap[tileEntity.name]);
				e->setTexture(textureMap[tileEntity.name]);

				// Offset position based on what tile we're in
				glm::vec3 pos = tileEntity.position + glm::vec3(j * map.tileSize, 0, i * map.tileSize);

				float rotationRad = physx::PxPi * (tileEntity.rotationDeg / 180.0f);
				physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(rotationRad, physx::PxVec3(0, 1, 0)));

				physicsEngine->createEntity(e, physicsEntityInfoMap[tileEntity.name], transform);
				entities.push_back(e);

				if (tileEntity.name == "house") {
					tile->house = e;
				}
			}
		}
	}
	renderingEngine->setupMinimap(map);

	// Create vehicles
	for(int i = 0; i < MAX_PLAYERS/2; i++) // TODO: replace with MAX_VEHICLES when rest of game logic can handle
	{
		players[i] = new Vehicle(PHYSICS_STEP_MS);
		setupVehicle(players[i], physx::PxTransform(10 + 10.0f*i, 2, 20), i);

		// TODO: get info from menu selection (ie. number of player characters)
		if(i > 0)
			players[i]->isAI = true;
		else
			players[i]->isAI = false;
	}
	// hard code textures for now
	players[0]->houseTexture = ContentLoading::loadDDS("res\\Textures\\HouseTexture-red.DDS");
	players[1]->houseTexture = ContentLoading::loadDDS("res\\Textures\\HouseTexture-blue.DDS");

	camera = new Camera(players[0]);
	renderingEngine->updateView(*camera);
}

void Game::setupVehicle(Vehicle* vehicle, physx::PxTransform transform, int num)
{
	ContentLoading::loadVehicleData("res\\JSON\\car.json", vehicle);
	vehicle->setDefaultRotation(-1.5708f, glm::vec3(0,1,0));
	if(num == 0)
	{
		vehicle->setRenderable(renderablesMap["van"]);
		vehicle->setTexture(textureMap["van"]);
	}
	else if(num == 1)
	{
		vehicle->setRenderable(renderablesMap["aivan"]);
		vehicle->setTexture(textureMap["aivan"]);
	}

	// TODO get dimensions working properly for vehicle
	vehicle->tuning.chassisDims = physx::PxVec3(2, 2, 5);
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
	inputEngine->setInputStruct(&players[0]->input, 0);
	inputEngine->setCamera(camera, 0);

	inputEngine->setInputStruct(&players[0]->input, 0);

	for(int i = 0; i < MAX_PLAYERS/2; i++)
	{
		players[i]->shootPizzaSignal.connect(this, &Game::shootPizza);
		players[i]->shootPizzaSignal.connect(audioEngine, &AudioEngine::playCannonSound);
		players[i]->dryFireSignal.connect(audioEngine, &AudioEngine::playDryFireSound);
		players[i]->brakeSignal.connect(audioEngine, &AudioEngine::playBrakeSound);
		players[i]->idleSignal.connect(audioEngine, &AudioEngine::playEngineIdleSound);
		players[i]->gasSignal.connect(audioEngine, &AudioEngine::playEngineRevSound);

		deliveryManager->addPlayer(players[i]);
	}

	inputEngine->unFucker.connect(this, &Game::unFuckerTheGame);

	// TODO: Should have a textures array or something that corresponds to each player so we can add this to above loop
	deliveryManager->deliveryTextures[players[0]] = ContentLoading::loadDDS("res\\Textures\\lawnRedDeliver.DDS");
	deliveryManager->deliveryTextures[players[1]] = ContentLoading::loadDDS("res\\Textures\\lawnBlueDeliver.DDS");


	deliveryManager->gameOverSignal.connect(this, &Game::endGame);

	deliveryManager->deliveryTextures[players[0]] = ContentLoading::loadDDS("res\\Textures\\lawnRedDeliver.DDS");
	deliveryManager->deliveryTextures[players[1]] = ContentLoading::loadDDS("res\\Textures\\lawnBlueDeliver.DDS");

	deliveryManager->assignDeliveries();
	physicsEngine->simulationCallback->pizzaBoxSleep.connect(deliveryManager, &DeliveryManager::pizzaLanded);
	physicsEngine->simulationCallback->inPickUpLocation.connect(deliveryManager, &DeliveryManager::refillPizza);
	physicsEngine->simulationCallback->inPickUpLocation.connect(audioEngine, &AudioEngine::playReloadSound);
}

// Main loop of the game
void Game::mainLoop()
{
	unsigned int oldTimeMs = SDL_GetTicks();
	unsigned int deltaTimeAccMs = 0;

	// Game loop
	while (gameState != GameState::EXIT)
	{
		if (gameState == GameState::INTRO)
		{
			processSDLEvents();
			renderingEngine->displayIntro();

			//swap buffers
			SDL_GL_SwapWindow(window);
			SDL_Delay(5000);
			gameState = GameState::PLAY;
			
		}
		else if(gameState == GameState::PLAY)
		{
			processSDLEvents();

			// Figure out timestep and run physics
			unsigned int newTimeMs = SDL_GetTicks();
			unsigned int deltaTimeMs = newTimeMs - oldTimeMs;
			oldTimeMs = newTimeMs;

			deltaTimeAccMs += deltaTimeMs;
			if (deltaTimeAccMs >= PHYSICS_STEP_MS) 
			{
				deltaTimeAccMs -= PHYSICS_STEP_MS;
				deliveryManager->timePassed(PHYSICS_STEP_MS);

				// Update the player and AI cars
				for(int i = 0; i < MAX_PLAYERS/2; i++)
				{
					if(players[i]->isAI)
					{
						AICollisionEntity closest = physicsEngine->AISweep(players[i]);
						aiEngine->updateAI(players[i], deliveryManager->deliveries[players[i]], map, closest);
						renderingEngine->setupNodes(players[i]->currentPath, vec3(1,1,0)); // TODO: Remove when adding multiple AIs, otherwise will be very confusing (or change colours to match AI)
				
					}
					players[i]->update();
				}
				physicsEngine->simulate(PHYSICS_STEP_MS);

				// Update the camera
				camera->update();
				renderingEngine->updateView(*camera);		
			}
			// Update Sound
			// TODO: update audioengine to support multiple listeners
			audioEngine->update(players[0]->getModelMatrix());

			// Display
			renderingEngine->displayFuncTex(entities);
			for(int i = 0 ; i < MAX_PLAYERS/2; i++)
				renderingEngine->drawShadow(players[i]->getPosition());
			
			renderingEngine->drawNodes(players[1]->currentPath.size(), "points");
			renderingEngine->drawSkybox(players[0]->getPosition()); // TODO: See above; should render skybox for each player
			renderingEngine->drawMinimap(players[0], players[1]); // TODO: Should support arbitrary number of vans

			// TODO: Broken record, but should draw to corresponding player's viewport
			string speed = "Speed: ";
			speed.append(to_string(players[0]->getPhysicsVehicle()->computeForwardSpeed()));
			renderingEngine->printText2D(speed.data(), 0, 700, 24);

			string frameRate = "DeltaTime: ";
			frameRate.append(to_string(deltaTimeMs));
			renderingEngine->printText2D(frameRate.data(), 0, 670, 20);

			string score = "Score: ";
			score.append(to_string(deliveryManager->getScore(players[0])));
			renderingEngine->printText2D(score.data(), 1050, 700, 24);
			renderingEngine->printText2D(deliveryManager->getDeliveryText(players[0]).data(), 725, 670, 20);

			string pizzas = "Pizzas: ";
			pizzas.append(to_string(players[0]->pizzaCount));
			renderingEngine->printText2D(pizzas.data(), 1050, 640, 24);

//			renderingEngine->drawNodes(p2Vehicle->currentPath.size(), "lines");


			//swap buffers
			SDL_GL_SwapWindow(window);
			physicsEngine->fetchSimulationResults();
			//gameState = GameState::INTRO;
		}
		else if(gameState == GameState::MENU)
		{
			//menyoo logic
		}
		else if(gameState == GameState::PAUSE)
		{
			//pause menu logic
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
			inputEngine->controllerAxisMotion(event);
		}
		else if (event.type == SDL_CONTROLLERBUTTONDOWN)
		{
			inputEngine->controllerButtonDown(event);
		}
		else if (event.type == SDL_CONTROLLERBUTTONUP)
		{
			inputEngine->controllerButtonUp(event);
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
	gameState = GameState::EXIT;
	fatalError("Game over!");
}

// Creates and fires a pizza from the provided vehicle
void Game::shootPizza(Vehicle* vehicle)
{
	PizzaBox* pizzaBox = new PizzaBox(vehicle);
	pizzaBox->setRenderable(renderablesMap["box"]);
	pizzaBox->setTexture(textureMap["box"]);

	physx::PxTransform transform = vehicle->getActor()->getGlobalPose();
	physx::PxVec3 posOffset = transform.rotate(physx::PxVec3(0.0f, 1.25f, 1.0f));
	transform.p += posOffset;

	physx::PxVec3 velocity = transform.rotate(physx::PxVec3(0.0f, 0.0f, 20.0f));
	physx::PxVec3 vehicleVelocity = vehicle->getRigidDynamic()->getLinearVelocity();
	velocity += vehicleVelocity;

	physicsEngine->createEntity(pizzaBox, physicsEntityInfoMap["box"], transform);
	pizzaBox->getRigidDynamic()->setLinearVelocity(velocity);
	pizzaBox->getActor()->setActorFlag(physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
	entities.push_back(pizzaBox);
}

void Game::unFuckerTheGame()
{
	for(int i =0 ; i < MAX_PLAYERS/2; i++)
	{
		players[i]->getActor()->setGlobalPose(physx::PxTransform(10 + i*10.0f, 2, 20));
		players[i]->getPhysicsVehicle()->setToRestState();
	}

	for (unsigned int i = 0; i < 10; i++) 
	{
		camera->update();
	}
}

Game::~Game(void)
{
	for(int i = 0 ; i < MAX_PLAYERS/2; i++)
		players[i]->shootPizzaSignal.disconnect_all();

	for (unsigned int i = 0; i < entities.size(); i++)
		delete entities[i];

	std::map<std::string, Renderable*>::iterator it;
	for (it = ContentLoading::loadedModels.begin(); it != ContentLoading::loadedModels.end(); ++it)
		delete it->second;
}
