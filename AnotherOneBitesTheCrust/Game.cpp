#include "Game.h"
#include "PizzaBox.h"
#include "WheelEntity.h"
#include "Camera.h"
#include "ContentLoading.h"

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
	gameState = GameState::PLAY;
	renderingEngine = nullptr;
	physicsEngine = nullptr;
	inputEngine = nullptr;
	aiEngine = nullptr;
	audioEngine = nullptr;
	screen = nullptr;
	p1Vehicle = nullptr;
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

	glClearColor(0.2f, 0.2f, 0.5f, 1.0f);				//blue background

	aiEngine = new AIEngine();
	audioEngine = new AudioEngine();
	inputEngine = new InputEngine();
	physicsEngine = new PhysicsEngine();
	renderingEngine = new RenderingEngine();
	deliveryManager = new DeliveryManager();
	renderingEngine->initText2D("res\\Fonts\\Holstein.DDS");
	renderingEngine->setupMiscBuffers();


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
					e = new PhysicsEntity();
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
			}
		}
	}
	renderingEngine->setupMinimap(map);
	// Create vehicles
	p1Vehicle = new Vehicle(PHYSICS_STEP_MS);
	setupVehicle(p1Vehicle, physx::PxTransform(10, 2, 20), 0);
	p2Vehicle = new Vehicle(PHYSICS_STEP_MS);
	setupVehicle(p2Vehicle, physx::PxTransform(30, 2, 20), 1);

	// Initialize player location buffer for camera
	for (unsigned int i = 0; i < CAMERA_POS_BUFFER_SIZE; i++)
	{
		cameraPosBuffer[i] = p1Vehicle->getPosition() + glm::vec3(p1Vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
	}
	cameraPosBufferIndex = 0;
	camera.setPosition(cameraPosBuffer[CAMERA_POS_BUFFER_SIZE]);
	camera.setLookAtPosition(p1Vehicle->getPosition());
	camera.setUpVector(glm::vec3(0,1,0));
	renderingEngine->updateView(camera);
	
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
	physx::PxRigidDynamic* actor = vehicle->getDynamicActor();
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
	inputEngine->setInputStruct(&p1Vehicle->input, 0);

	p1Vehicle->shootPizzaSignal.connect(this, &Game::shootPizza);
	p1Vehicle->brakeSignal.connect(audioEngine, &AudioEngine::playBrakeSound);
	audioEngine->playEngineIdleSound(p1Vehicle);
	/*
	p1Vehicle->idleSignal.connect(audioEngine, &AudioEngine::playEngineIdleSound);
	p1Vehicle->gasSignal.connect(audioEngine, &AudioEngine::playEngineRevSound);*/

	p2Vehicle->shootPizzaSignal.connect(this, &Game::shootPizza);
	p2Vehicle->brakeSignal.connect(audioEngine, &AudioEngine::playBrakeSound);
	audioEngine->playEngineIdleSound(p2Vehicle);
	/*
	p2Vehicle->idleSignal.connect(audioEngine, &AudioEngine::playEngineIdleSound);
	p2Vehicle->gasSignal.connect(audioEngine, &AudioEngine::playEngineRevSound);*/


	inputEngine->reverseCam.connect(&camera, &Camera::setReverseCam);
	inputEngine->unFucker.connect(this, &Game::unFuckerTheGame);

	deliveryManager->addPlayer(p1Vehicle);
	deliveryManager->addPlayer(p2Vehicle);

	deliveryManager->deliveryTextures[p1Vehicle] = ContentLoading::loadDDS("res\\Textures\\lawnRedDeliver.DDS");
	deliveryManager->deliveryTextures[p2Vehicle] = ContentLoading::loadDDS("res\\Textures\\lawnBlueDeliver.DDS");
	deliveryManager->assignDeliveries();
	physicsEngine->simulationCallback->pizzaBoxSleep.connect(deliveryManager, &DeliveryManager::pizzaLanded);
	physicsEngine->simulationCallback->inPickUpLocation.connect(deliveryManager, &DeliveryManager::refillPizza);
}

// Main loop of the game
void Game::mainLoop()
{
	unsigned int oldTimeMs = SDL_GetTicks();
	unsigned int deltaTimeAccMs = 0;

	// Game loop
	while (gameState!= GameState::EXIT)
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
			aiEngine->updateAI(p2Vehicle, deliveryManager->deliveries[p2Vehicle], map);
			p1Vehicle->update();
			p2Vehicle->update();
		
			physicsEngine->simulate(PHYSICS_STEP_MS);

			// Update the camera position buffer with new location
			if (camera.isReverseCam()) {
				cameraPosBuffer[cameraPosBufferIndex] = p1Vehicle->getPosition() + glm::vec3(p1Vehicle->getModelMatrix() * glm::vec4(0,8,15,0));
			}
			else {
				cameraPosBuffer[cameraPosBufferIndex] = p1Vehicle->getPosition() + glm::vec3(p1Vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
			}
			cameraPosBufferIndex = (cameraPosBufferIndex + 1) % CAMERA_POS_BUFFER_SIZE;

			// Set camera to look at player with a positional delay
			camera.setPosition(cameraPosBuffer[cameraPosBufferIndex]);
			camera.setLookAtPosition(p1Vehicle->getPosition());
			renderingEngine->updateView(camera);
		}
		// Update Sound
		audioEngine->update(p1Vehicle->getModelMatrix());

		// Display
		renderingEngine->displayFuncTex(entities);
		renderingEngine->drawShadow(p1Vehicle->getPosition());
		renderingEngine->drawShadow(p2Vehicle->getPosition());
		renderingEngine->drawSkybox(p1Vehicle->getPosition());
		renderingEngine->drawMinimap();

		string speed = "Speed: ";
		speed.append(to_string(p1Vehicle->getPhysicsVehicle()->computeForwardSpeed()));
		renderingEngine->printText2D(speed.data(), 0, 700, 24);

		string frameRate = "DeltaTime: ";
		frameRate.append(to_string(deltaTimeMs));
		renderingEngine->printText2D(frameRate.data(), 0, 670, 20);

		string score = "Score: ";
		score.append(to_string(deliveryManager->getScore(p1Vehicle)));
		renderingEngine->printText2D(score.data(), 1050, 700, 24);
		renderingEngine->printText2D(deliveryManager->getDeliveryText(p1Vehicle).data(), 725, 670, 20);

		string pizzas = "Pizzas: ";
		pizzas.append(to_string(p1Vehicle->pizzaCount));
		renderingEngine->printText2D(pizzas.data(), 1050, 640, 24);


		//swap buffers
		SDL_GL_SwapWindow(window);
		physicsEngine->fetchSimulationResults();
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

// Creates and fires a pizza from the provided vehicle
void Game::shootPizza(Vehicle* vehicle)
{
	PizzaBox* pizzaBox = new PizzaBox(vehicle);
	pizzaBox->setRenderable(renderablesMap["box"]);
	pizzaBox->setTexture(textureMap["box"]);

	physx::PxTransform transform = vehicle->getDynamicActor()->getGlobalPose();
	physx::PxVec3 posOffset = transform.rotate(physx::PxVec3(0.0f, 1.25f, 1.0f));
	transform.p += posOffset;

	physx::PxVec3 velocity = transform.rotate(physx::PxVec3(0.0f, 0.0f, 20.0f));
	physx::PxVec3 vehicleVelocity = vehicle->getDynamicActor()->getLinearVelocity();
	velocity += vehicleVelocity;

	physicsEngine->createEntity(pizzaBox, physicsEntityInfoMap["box"], transform);
	pizzaBox->getDynamicActor()->setLinearVelocity(velocity);
	pizzaBox->getActor()->setActorFlag(physx::PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
	entities.push_back(pizzaBox);

	audioEngine->playCannonSound(vehicle);
}

void Game::unFuckerTheGame()
{
	p1Vehicle->getDynamicActor()->setGlobalPose(physx::PxTransform(10, 2, 20));
	p2Vehicle->getDynamicActor()->setGlobalPose(physx::PxTransform(30, 2, 20));
	p1Vehicle->getPhysicsVehicle()->setToRestState();
	p2Vehicle->getPhysicsVehicle()->setToRestState();

	for (unsigned int i = 0; i < CAMERA_POS_BUFFER_SIZE; i++)
	{
		cameraPosBuffer[i] = p1Vehicle->getPosition() + glm::vec3(p1Vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
	}
	cameraPosBufferIndex = 0;
}

Game::~Game(void)
{
	p1Vehicle->shootPizzaSignal.disconnect_all();
	p2Vehicle->shootPizzaSignal.disconnect_all();
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}
	std::map<std::string, Renderable*>::iterator it;
	for (it = ContentLoading::loadedModels.begin(); it != ContentLoading::loadedModels.end(); ++it) {
		delete it->second;
	}
}
