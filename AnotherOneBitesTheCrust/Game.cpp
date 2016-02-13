#include "Game.h"
#include "DrivingInput.h"
#include "DynamicEntity.h"

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
}

// The entry point of the game
void Game::run()
{
	// Preload data, initialize subsystems, anything to do before entering the main loop
	initSystems();
	setupEntities();
	connectSignals();

	mainLoop();
}

void Game::initSystems()
{
	SDL_Init(SDL_INIT_EVERYTHING);		//Initialize SDL
	window = SDL_CreateWindow("Another Bites The Crust", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

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

	renderingEngine = new RenderingEngine();
	inputEngine = new InputEngine();
	physicsEngine = new PhysicsEngine();

	//renderingEngine.testOBJLoading();
}

void Game::setupEntities()
{
	Renderable * floor = new Renderable();
	vector<glm::vec3>floorVerts;
	vector<glm::vec3>floorNormals;
	bool floorRes = ContentLoading::loadOBJNonIndexed("res\\Models\\FlatFloor.obj", floorVerts, floorNormals);
	floor->setVerts(floorVerts);
	floor->setNorms(floorNormals);
	renderables.push_back(floor);
	renderingEngine->assignBuffers(floor);

	Renderable * box = new Renderable();
	vector<glm::vec3>boxVerts;
	vector<glm::vec3>boxNormals;
	bool boxRes = ContentLoading::loadOBJNonIndexed("res\\Models\\PizzaBox.obj", boxVerts, boxNormals);
	box->setVerts(boxVerts);
	box->setNorms(boxNormals);
	renderables.push_back(box);
	renderingEngine->assignBuffers(box);

	// Renderable for the actual van model, not used right now
	Renderable* van = new Renderable();
	vector<glm::vec3>vanVerts;
	vector<glm::vec3>vanNormals;
	bool res = ContentLoading::loadOBJNonIndexed("res\\Models\\Van.obj", vanVerts, vanNormals);
	van->setVerts(vanVerts);
	van->setNorms(vanNormals);
	renderables.push_back(van);
	renderingEngine->assignBuffers(van);

	Entity* ground = new Entity();
	ground->setRenderable(floor);
	ground->setColor(glm::vec3(1.0f,1.0f,1.0f));
	entities.push_back(ground);

	Entity* ground2 = new Entity();
	ground2->setRenderable(floor);
	ground2->setDefaultTranslation(glm::vec3(70.0f,0.0f,0.0f));
	ground2->setColor(glm::vec3(1.0f,1.0f,0.0f));
	entities.push_back(ground2);

	Entity* ground3 = new Entity();
	ground3->setRenderable(floor);
	ground3->setDefaultTranslation(glm::vec3(70.0f,0.0f,70.0f));
	ground3->setColor(glm::vec3(1.0f,1.0f,1.0f));
	entities.push_back(ground3);

	Entity* ground4 = new Entity();
	ground4->setRenderable(floor);
	ground4->setDefaultTranslation(glm::vec3(0.0f,0.0f,70.0f));
	ground4->setColor(glm::vec3(1.0f,1.0f,0.0f));
	entities.push_back(ground4);

	playerVehicle = new Vehicle();
	ContentLoading::loadVehicleData("res\\JSON\\car.json", playerVehicle);
	playerVehicle->setRenderable(van);
	playerVehicle->setDefaultRotation(-1.5708f, glm::vec3(0,1,0));
	playerVehicle->setColor(glm::vec3(1,0,0));
	glm::vec3 d = van->getDimensions();
	cout << d.x << " " << d.y << " " << d.z << endl;
	playerVehicle->chassisDims = physx::PxVec3(2, 2, 5);
	physicsEngine->createVehicle(playerVehicle);
	entities.push_back(playerVehicle);

	//set camera
	camera.setPosition(glm::vec3(0,6,8));			//location of camera
	camera.setLookAtPosition(glm::vec3(0,0,0));		//where camera is pointing
	camera.setUpVector(glm::vec3(0,1,0));			//orientation on camera
	renderingEngine->updateView(camera);
}

void Game::connectSignals()
{
	inputEngine->DrivingSignal.connect(playerVehicle, &Vehicle::handleInput);
	inputEngine->FireSignal.connect(this, &Game::firePizza);
}

void Game::mainLoop()
{
	unsigned int oldTimeMs = SDL_GetTicks();
	
	float x = 0;
	// Game loop
	while (gameState!= GameState::EXIT)
	{
		// Update the player and AI cars
		processSDLEvents();
		aiEngine->updateAI();

		// Figure out timestep and run physics
		unsigned int newTimeMs = SDL_GetTicks();
		unsigned int deltaTimeMs = newTimeMs - oldTimeMs;
		oldTimeMs = newTimeMs;

		physicsEngine->simulate(deltaTimeMs);
		physicsEngine->fetchSimulationResults();

		// Point the camera at the car
		camera.setPosition(playerVehicle->getPosition() + glm::vec3(playerVehicle->getModelMatrix() * glm::vec4(0,8,-20,0)));
		camera.setLookAtPosition(playerVehicle->getPosition());
		renderingEngine->updateView(camera);

		//display
		renderingEngine->displayFunc(entities);
		//renderingEngine.draw();

		//swap buffers
		SDL_GL_SwapWindow(window);
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
		else if (event.type == SDL_CONTROLLERAXISMOTION || event.type == SDL_CONTROLLERBUTTONDOWN ||
			     event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEADDED)
		{
			inputEngine->processControllerEvent(event);
		}
		else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		{
			gameState = GameState::EXIT;
		}
		else
		{
			// other events, do nothing yet
		}
	}
}

void Game::firePizza()
{
	DynamicEntity* pizzaBox = new DynamicEntity();
	pizzaBox->setRenderable(renderables.at(1)); // todo, match names to renderables or something instead of hard-coded
	glm::vec3 position = playerVehicle->getPosition() + glm::vec3(playerVehicle->getModelMatrix() * glm::vec4(0, 1.0, 1.0, 0));
	glm::vec3 velocity = glm::vec3(playerVehicle->getModelMatrix() * glm::vec4(0.0, 0.0, 40.0, 0.0));
	physx::PxVec3 v = playerVehicle->getDynamicActor()->getLinearVelocity();
	velocity = velocity + glm::vec3(v.x, v.y, v.z);
	physicsEngine->createDynamicEntity(pizzaBox, position, velocity);
	pizzaBox->setColor(glm::vec3(0,1,1));
	entities.push_back(pizzaBox);
}

Game::~Game(void)
{
	for (unsigned int i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}
	for (unsigned int i = 0; i < renderables.size(); i++)
	{
		delete renderables[i];
	}
}
