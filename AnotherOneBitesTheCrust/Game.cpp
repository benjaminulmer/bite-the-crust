#include "Game.h"
#include "DrivingInput.h"

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
void Game::run() {
	// Preload data, initialize subsystems, anything to do before entering the main loop
	initSystems();
	setupEntities();

	inputEngine = new InputEngine();
	physicsEngine = new PhysicsEngine();

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				//blue background

	renderingEngine.init();
<<<<<<< HEAD
	renderingEngine.testOBJLoading();

=======
>>>>>>> refs/remotes/origin/Ben
}

void Game::setupEntities() {
	Renderable* plane = new Renderable();
	//add vertices and colors
	plane->addPoint(vec3(2,-2,2),vec3(1,0,0));
	plane->addPoint(vec3(2,-2,-2),vec3(0,1,0));
	plane->addPoint(vec3(-2,-2,-2),vec3(0,0,1));
	plane->addPoint(vec3(-2,-2,2),vec3(1,1,1));
	//faces
	plane->createFace(0);
	plane->createFace(1);
	plane->createFace(2);
	plane->createFace(2);
	plane->createFace(3);
	plane->createFace(0);

	renderables.push_back(plane);
	renderingEngine.assignBuffers(plane);


	Renderable* triangle = new Renderable();
	//vertices and corresponding colors
	triangle->addPoint(vec3(0,0,0), vec3(1,0,0));
	triangle->addPoint(vec3(0,1,0), vec3(0,1,0));
	triangle->addPoint(vec3(1,1,0), vec3(0,0,1));
	//faces
	triangle->createFace(0);
	triangle->createFace(1);
	triangle->createFace(2);

	renderables.push_back(triangle);
	renderingEngine.assignBuffers(triangle);

	Renderable* vehicle = new Renderable();
	vehicle->addPoint(vec3(0,0,0), vec3(1,0,0));
	vehicle->addPoint(vec3(2.5,0,0), vec3(0,1,0));
	vehicle->addPoint(vec3(2.5,2,0), vec3(0,0,1));
	vehicle->addPoint(vec3(0,2,0), vec3(1,1,1));
	vehicle->addPoint(vec3(0,0,-5), vec3(0,1,1));
	vehicle->addPoint(vec3(2.5,0,-5), vec3(1,0,1));
	vehicle->addPoint(vec3(2.5,2,-5), vec3(1,1,0));
	vehicle->addPoint(vec3(0,2,-5), vec3(1,1,1));

	vehicle->createFace(0);
	vehicle->createFace(1);
	vehicle->createFace(2);
	vehicle->createFace(2);
	vehicle->createFace(3);
	vehicle->createFace(0);
	
	vehicle->createFace(1);
	vehicle->createFace(5);
	vehicle->createFace(6);
	vehicle->createFace(6);
	vehicle->createFace(2);
	vehicle->createFace(1);

	vehicle->createFace(0);
	vehicle->createFace(4);
	vehicle->createFace(7);
	vehicle->createFace(7);
	vehicle->createFace(3);
	vehicle->createFace(0);

	vehicle->createFace(4);
	vehicle->createFace(5);
	vehicle->createFace(6);
	vehicle->createFace(6);
	vehicle->createFace(7);
	vehicle->createFace(4);

	vehicle->createFace(0);
	vehicle->createFace(1);
	vehicle->createFace(5);
	vehicle->createFace(5);
	vehicle->createFace(4);
	vehicle->createFace(0);

	vehicle->createFace(2);
	vehicle->createFace(6);
	vehicle->createFace(7);
	vehicle->createFace(7);
	vehicle->createFace(3);
	vehicle->createFace(2);

	renderables.push_back(vehicle);
	renderingEngine.assignBuffers(vehicle);

	Entity* ground = new Entity();
	ground->setPosition(vec3(0,0,0));
	ground->setRenderable(plane);
	entities.push_back(ground);

	Entity* tri = new Entity();
	tri->setPosition(vec3(-1,1, 1.0));		//change position here
	tri->setRenderable(triangle);
	//entities.push_back(tri);

	Entity* vcl = new Entity();
	vcl->setRenderable(vehicle);
	entities.push_back(vcl);

	//set camera
	camera.setPosition(glm::vec3(4,3,8));			//location of camera
	camera.setLookAtPosition(glm::vec3(0,0,0));		//where camera is pointing
	camera.setUpVector(glm::vec3(0,1,0));			//orientation on camera
	renderingEngine.updateView(camera);
}


void Game::mainLoop() {
	unsigned int oldTimeMs = SDL_GetTicks();
	
	float x = 0;
	// Game loop
	while (gameState!= GameState::EXIT) {
		processSDLEvents();

		// Update the player and AI cars
		DrivingInput* playerInput = inputEngine->getInput();
		aiEngine->updateAI();

		// Figure out timestep and run physics
		unsigned int newTimeMs = SDL_GetTicks();
		unsigned int deltaTimeMs = newTimeMs - oldTimeMs;
		oldTimeMs = newTimeMs;

		physicsEngine->simulate(deltaTimeMs, playerInput);

		// Render
		//renderingEngine->pushEntities();

		x+=0.0001;
		//cout << x << endl;
		//entities[1]->setPosition(vec3(x,0,0));

		//display
		//renderingEngine.displayFunc(entities);
		renderingEngine.draw();
		//swap buffers
		SDL_GL_SwapWindow(window);
		physicsEngine->fetchSimulationResults();
	}
}

void Game::processSDLEvents() {
	SDL_Event event;
    while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			gameState = GameState::EXIT;
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION || event.type == SDL_CONTROLLERBUTTONDOWN ||
			     event.type == SDL_CONTROLLERDEVICEREMOVED || event.type == SDL_CONTROLLERDEVICEADDED) {
			inputEngine->processControllerEvent(event);
		}
		else if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		{
			gameState = GameState::EXIT;
		}
		else {
			// other events, do nothing yet
		}
	}
}

Game::~Game(void)
{
	for (unsigned int i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
	for (unsigned int i = 0; i < renderables.size(); i++) {
		delete renderables[i];
	}
}
