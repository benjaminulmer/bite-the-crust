#include "Game.h"
#include "PizzaBox.h"
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
	screenWidth = 1024;		//pro csgo resolution
	screenHeight = 768;
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
}

// Create and initialize all loaded entities in the game world
void Game::setupEntities()
{
	// Load data for entities
	if (!ContentLoading::loadEntityList("res\\JSON\\entityList.json", renderablesMap, physicsEntityInfoMap, textureMap))
		fatalError("Could not load entities list.");

	// Assign the buffers for all the renderables
	std::map<std::string, Renderable*>::iterator it;
	for (it = renderablesMap.begin(); it != renderablesMap.end(); ++it) {
		renderingEngine->assignBuffersTex(it->second);
	}

	// Load the map
	if (!ContentLoading::loadMap("res\\JSON\\map.json", map))
		fatalError("Could not load map file.");
	deliveryManager->map = &map;

	// Create all the entities loaded in the map
	for (unsigned int i = 0; i < map.tiles.size(); i++) {
		for (unsigned int j = 0; j < map.tiles[i].size(); j++) {
			deliveryManager->addDeliveryLocation(&map.tiles[i][j]);

			Tile* tile = &map.tiles[i][j];
			Entity* ground = new Entity();
			ground->setRenderable(renderablesMap[tile->groundModel]);
			ground->setTexture(textureMap[tile->groundModel]);

			// Offset by tileSize/2 so that the corner of the map starts at 0,0 instead of -35,-35.
			ground->setDefaultTranslation(glm::vec3(i*map.tileSize + map.tileSize/2, 0, j*map.tileSize + map.tileSize/2));
			tile->ground = ground;
			tile->groundTexture = textureMap[tile->groundModel];
			entities.push_back(ground);

			for (unsigned int k = 0; k < tile->entities.size(); k++) {
				TileEntity tileEntity = tile->entities[k];

				PhysicsEntity* e = new PhysicsEntity();
				// TODO, error check that these models do exist, instead of just break
				e->setRenderable(renderablesMap[tileEntity.model]);

				e->setDefaultTranslation(e->getRenderable()->getCenter());
				e->setTexture(textureMap[tileEntity.model]);

				// Offset position based on what tile we're in
				glm::vec3 pos = tileEntity.position + glm::vec3(i * map.tileSize + map.tileSize/2, 0, j * map.tileSize + map.tileSize/2);
				physx::PxTransform transform(physx::PxVec3(pos.x, pos.y, pos.z), physx::PxQuat(physx::PxIdentity));

				physicsEngine->createEntity(e, physicsEntityInfoMap[tileEntity.model], transform);
				entities.push_back(e);
			}
		}
	}

	/**********************************************************
						Creating Vechicles
	**********************************************************/
	p1Vehicle = new Vehicle(PHYSICS_STEP_MS);
	ContentLoading::loadVehicleData("res\\JSON\\car.json", p1Vehicle);
	p1Vehicle->setRenderable(renderablesMap["van"]);
	p1Vehicle->setTexture(textureMap["van"]);
	p1Vehicle->setDefaultRotation(-1.5708f, glm::vec3(0,1,0));
	p1Vehicle->setDefaultTranslation(renderablesMap["van"]->getCenter());

	// TODO get dimensions working properly for vehicle
	p1Vehicle->tuning.chassisDims = physx::PxVec3(2, 2, 5);
	physicsEngine->createVehicle(p1Vehicle, physx::PxTransform(physx::PxVec3(0, 2, 0), physx::PxQuat(physx::PxIdentity)));
	entities.push_back(p1Vehicle);

	//// Player 2 (ie. AI)
	p2Vehicle = new Vehicle(PHYSICS_STEP_MS);
	ContentLoading::loadVehicleData("res\\JSON\\car.json", p2Vehicle);
	p2Vehicle->setRenderable(renderablesMap["van"]);
	p2Vehicle->setTexture(textureMap["van"]);
	p2Vehicle->setDefaultRotation(-1.5708f, glm::vec3(0,1,0));
	p2Vehicle->setDefaultTranslation(renderablesMap["van"]->getCenter());

	// TODO get dimensions working properly for vehicle
	p2Vehicle->tuning.chassisDims = physx::PxVec3(2, 2, 5);
	physicsEngine->createVehicle(p2Vehicle, physx::PxTransform(physx::PxVec3(10, 2, 0), physx::PxQuat(physx::PxIdentity)));
	entities.push_back(p2Vehicle);

	// Initialize player location buffer for camera
	for (unsigned int i = 0; i < CAMERA_POS_BUFFER_SIZE; i++)
	{
		cameraPosBuffer[i] = p1Vehicle->getPosition() + glm::vec3(p1Vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
	}
	cameraPosBufferIndex = 0;
	camera.setUpVector(glm::vec3(0,1,0));

	physicsEngine->createTrigger();
}

// Connects systems together
void Game::connectSystems()
{
	inputEngine->setInputStruct(&p1Vehicle->input, 0);

	p1Vehicle->shootPizzaSignal.connect(this, &Game::shootPizza);
	p2Vehicle->shootPizzaSignal.connect(this, &Game::shootPizza);

	deliveryManager->addPlayer(p1Vehicle);
	deliveryManager->addPlayer(p2Vehicle);
	deliveryManager->deliveryTextures[p1Vehicle] = ContentLoading::loadDDS("res\\Textures\\DeliverFloor.DDS");
	deliveryManager->deliveryTextures[p2Vehicle] = ContentLoading::loadDDS("res\\Textures\\AIDeliverFloor.DDS");
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
			aiEngine->updateAI(p2Vehicle, map);
			p1Vehicle->update();
			p2Vehicle->update();
		
			physicsEngine->simulate(PHYSICS_STEP_MS);

			// Update the camera position buffer with new location
			cameraPosBuffer[cameraPosBufferIndex] = p1Vehicle->getPosition() + glm::vec3(p1Vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
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
		//renderingEngine->displayFuncTex(pizzaEntities);
		///test drawing
		//renderingEngine->testDraw();

		string speed = "Speed: ";
		speed.append(to_string(p1Vehicle->getPhysicsVehicle()->computeForwardSpeed()));
		renderingEngine->printText2D(speed.data(), 0, 740, 24);

		string score = "Score: ";
		score.append(to_string(deliveryManager->getScore(p1Vehicle)));
		renderingEngine->printText2D(score.data(), 800, 740, 24);
		
		renderingEngine->printText2D(deliveryManager->getDeliveryText(p1Vehicle).data(), 500, 700, 20);

		string pizzas = "Pizzas: ";
		pizzas.append(to_string(p1Vehicle->pizzaCount));
		renderingEngine->printText2D(pizzas.data(), 800, 680, 24);

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

Game::~Game(void)
{
	p1Vehicle->shootPizzaSignal.disconnect_all();
	p2Vehicle->shootPizzaSignal.disconnect_all();
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}
	std::map<std::string, Renderable*>::iterator it;
	for (it = renderablesMap.begin(); it != renderablesMap.end(); ++it) {
		delete it->second;
	}
}
