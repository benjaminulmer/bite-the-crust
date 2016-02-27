#include "Game.h"
#include "DynamicEntity.h"
#include "Camera.h"
#include "ContentLoading.h"

#include <foundation/PxTransform.h> 

#include <iostream>
#include <string>
#include <sigslot.h>

using namespace std;

void fatalError(string errorString)
{
	cout << errorString << endl;
	cout << "Enter any key to quit...";
	int tmp;
	cin >> tmp;
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

void Game::initSystems()
{
	SDL_Init(SDL_INIT_EVERYTHING);		//Initialize SDL
	window = SDL_CreateWindow("Another One Bites The Crust", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

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
}

void Game::setupEntities()
{
	ContentLoading::loadEntityList("res\\JSON\\entityList.json", renderablesMap, physicsEntityInfoMap);
	//ContentLoading::loadRenderables("res\\JSON\\renderables.json", renderablesMap);
	// Assign the buffers for all the renderables
	std::map<std::string, Renderable*>::iterator it;
	for (it = renderablesMap.begin(); it != renderablesMap.end(); ++it) {
		renderingEngine->assignBuffers(it->second);
	}
	// Set up the colours, since we don't have textures yet
	renderablesMap["floor2"]->setColor(glm::vec3(1,1,0));
	renderablesMap["box"]->setColor(glm::vec3(0,1,1));
	renderablesMap["van"]->setColor(glm::vec3(1,0,0));

	ContentLoading::loadMap("res\\JSON\\map.json", map);
	// Create all the entities loaded in the map
	for (unsigned int i = 0; i < map.tiles.size(); i++) {
		for (unsigned int j = 0; j < map.tiles[i].size(); j++) {
			Tile tile = map.tiles[i][j];
			for (unsigned int k = 0; k < tile.entities.size(); k++) {
				TileEntity tileEntity = tile.entities[k];

				DynamicEntity* e = new DynamicEntity();
				// todo, error check that these models do exist, instead of just break
				e->setRenderable(renderablesMap[tileEntity.model]);
				e->setDefaultTranslation(e->getRenderable()->getCenter());

				// Offset position based on what tile we're in
				glm::vec3 pos = tileEntity.position + glm::vec3(i * map.tileSize, 0, j * map.tileSize);
				physx::PxTransform transform(physx::PxVec3(pos.x, pos.y + 5, pos.z), physx::PxQuat(physx::PxIdentity));

				physicsEngine->createEntity(e, physicsEntityInfoMap[tileEntity.model], transform);
				entities.push_back(e);
			}
		}
	}

	Entity* ground = new Entity();
	ground->setRenderable(renderablesMap["floor"]);
	entities.push_back(ground);

	Entity* ground2 = new Entity();
	ground2->setRenderable(renderablesMap["floor2"]);
	ground2->setDefaultTranslation(glm::vec3(70.0f,0.0f,0.0f));
	entities.push_back(ground2);

	Entity* ground3 = new Entity();
	ground3->setRenderable(renderablesMap["floor"]);
	ground3->setDefaultTranslation(glm::vec3(70.0f,0.0f,70.0f));
	entities.push_back(ground3);

	Entity* ground4 = new Entity();
	ground4->setRenderable(renderablesMap["floor2"]);
	ground4->setDefaultTranslation(glm::vec3(0.0f,0.0f,70.0f));
	entities.push_back(ground4);

	/**********************************************************
						Creating Vechicles
	**********************************************************/
	p1Vehicle = new Vehicle();
	ContentLoading::loadVehicleData("res\\JSON\\car.json", p1Vehicle);
	p1Vehicle->setRenderable(renderablesMap["van"]);
	p1Vehicle->setDefaultRotation(-1.5708f, glm::vec3(0,1,0));
	p1Vehicle->setDefaultTranslation(renderablesMap["van"]->getCenter());

	// TODO get dimensions working properly for vehicle
	p1Vehicle->tuning.chassisDims = physx::PxVec3(2, 2, 5);
	physicsEngine->createVehicle(p1Vehicle, physx::PxTransform(physx::PxVec3(0, 2, 0), physx::PxQuat(physx::PxIdentity)));
	entities.push_back(p1Vehicle);

	//// Player 2 (ie. AI)
	p2Vehicle = new Vehicle();
	ContentLoading::loadVehicleData("res\\JSON\\car.json", p2Vehicle);
	p2Vehicle->setRenderable(renderablesMap["van"]);
	p2Vehicle->setDefaultRotation(-1.5708f, glm::vec3(0,1,0));
	p2Vehicle->setDefaultTranslation(renderablesMap["van"]->getCenter());

	// TODO get dimensions working properly for vehicle
	p2Vehicle->tuning.chassisDims = physx::PxVec3(2, 2, 5);
	physicsEngine->createVehicle(p2Vehicle, physx::PxTransform(physx::PxVec3(10, 2, 0), physx::PxQuat(physx::PxIdentity)));
	entities.push_back(p2Vehicle);

	for (unsigned int i = 0; i < CAMERA_POS_BUFFER_SIZE; i++)
	{
		cameraPosBuffer[i] = p1Vehicle->getPosition() + glm::vec3(p1Vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
	}
	cameraPosBufferIndex = 0;
	camera.setUpVector(glm::vec3(0,1,0));


	// ******** Hardcoded for now - Should be data driven *********** //
	pizzaInfo = new PhysicsEntityInfo();
	pizzaInfo->type = PhysicsType::DYNAMIC;
	pizzaInfo->dynamicInfo = new DynamicInfo();
	BoxInfo* shape = new BoxInfo();
	shape->geometry = Geometry::BOX;
	glm::vec3 d = renderablesMap["box"]->getDimensions();
	shape->halfX = d.x * 0.5f;
	shape->halfY = d.y * 0.5f;
	shape->halfZ = d.z * 0.5f;
	shape->filterFlag0 = FilterFlag::DRIVABLE_OBSTACLE;
	shape->filterFlag1 = FilterFlag::DRIVABLE_OBSTACLE_AGAINST;
	pizzaInfo->shapeInfo.push_back(shape);
}

// TODO decide how signals will be used and set them up
void Game::connectSystems()
{
	inputEngine->setInputStruct(&p1Vehicle->input, 0);

	p1Vehicle->ShootPizzaSignal.connect(this, &Game::shootPizza);
	p2Vehicle->ShootPizzaSignal.connect(this, &Game::shootPizza);
}

void Game::mainLoop()
{
	unsigned int oldTimeMs = SDL_GetTicks();
	unsigned int deltaTimeAccMs = 0;
	unsigned int physicsStepSize = 16;
	
	physicsEngine->createTrigger();

	// Game loop
	while (gameState!= GameState::EXIT)
	{
		// Update Sound
		audioEngine->update();

		processSDLEvents();

		// Figure out timestep and run physics
		unsigned int newTimeMs = SDL_GetTicks();
		unsigned int deltaTimeMs = newTimeMs - oldTimeMs;
		oldTimeMs = newTimeMs;

		deltaTimeAccMs += deltaTimeMs;
		if (deltaTimeAccMs >= physicsStepSize) 
		{
			deltaTimeAccMs -= physicsStepSize;

			// Update the player and AI cars
			aiEngine->updateAI(p2Vehicle);
			p1Vehicle->handleInput();
			p2Vehicle->handleInput();
		
			physicsEngine->simulate(physicsStepSize);

			// Update the camera position buffer with new location
			cameraPosBuffer[cameraPosBufferIndex] = p1Vehicle->getPosition() + glm::vec3(p1Vehicle->getModelMatrix() * glm::vec4(0,8,-15,0));
			cameraPosBufferIndex = (cameraPosBufferIndex + 1) % CAMERA_POS_BUFFER_SIZE;

			// Set camera to look at player with a positional delay
			camera.setPosition(cameraPosBuffer[cameraPosBufferIndex]);
			camera.setLookAtPosition(p1Vehicle->getPosition());
			renderingEngine->updateView(camera);
		}
		// Display
		renderingEngine->displayFunc(entities);

		// Swap buffers
		SDL_GL_SwapWindow(window);
		physicsEngine->fetchSimulationResults();
	}
}

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

// TODO move this to a different file
void Game::shootPizza(Vehicle* vehicle)
{
	DynamicEntity* pizzaBox = new DynamicEntity();
	pizzaBox->setRenderable(renderablesMap["box"]); // TODO, match names to renderables or something instead of hard-coded

	physx::PxTransform transform = vehicle->getDynamicActor()->getGlobalPose();
	physx::PxVec3 posOffset = transform.rotate(physx::PxVec3(0.0f, 1.2f, 1.0f));
	transform.p += posOffset;

	physx::PxVec3 velocity = transform.rotate(physx::PxVec3(0.0f, 0.0f, 20.0f));
	physx::PxVec3 vehicleVelocity = vehicle->getDynamicActor()->getLinearVelocity();
	velocity += vehicleVelocity;

	physicsEngine->createEntity(pizzaBox, pizzaInfo, transform);
	pizzaBox->getDynamicActor()->setLinearVelocity(velocity);
	entities.push_back(pizzaBox);
}

Game::~Game(void)
{
	p1Vehicle->ShootPizzaSignal.disconnect_all();
	p2Vehicle->ShootPizzaSignal.disconnect_all();
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}
	std::map<std::string, Renderable*>::iterator it;
	for (it = renderablesMap.begin(); it != renderablesMap.end(); ++it) {
		delete it->second;
	}
}
