#include "Game.h"

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

}

void Game::setupEntities() {
	Renderable* plane = new Renderable();
	plane->addPoint(vec3(2,-2,2),vec3(1,0,0));
	plane->addPoint(vec3(2,-2,-2),vec3(0,1,0));
	plane->addPoint(vec3(-2,-2,-2),vec3(0,0,1));
	plane->addPoint(vec3(-2,-2,2),vec3(1,1,1));
	plane->addPoint(vec3(2,-2,2),vec3(1,0,0));
	plane->addPoint(vec3(-2,-2,-2),vec3(0,0,1));
	renderables.push_back(plane);
	renderingEngine.assignBuffers(plane);

	Renderable* triangle = new Renderable();
	triangle->addPoint(vec3(0,0,0), vec3(0,0,1));
	triangle->addPoint(vec3(0,1,0), vec3(1,0,1));
	triangle->addPoint(vec3(1,1,0), vec3(1,1,0));
	triangle->createFace(0);
	triangle->createFace(1);
	triangle->createFace(2);
	renderables.push_back(triangle);
	renderingEngine.assignBuffers(triangle);

	//Entity* ground = new Entity();
	//ground->setPosition(vec3(0,0,0));
	//ground->setRenderable(plane);
	//entities.push_back(ground);
	Entity* tri = new Entity();
	tri->setPosition(vec3(-1,1, 1.0));
	tri->setRenderable(triangle);
	entities.push_back(tri);

	camera.setUpVector(glm::vec3(0,1,0));
	camera.setPosition(glm::vec3(4,3,3));
	camera.setLookAtPosition(glm::vec3(0,0,0));
	renderingEngine.updateView(camera);
}


void Game::mainLoop() {
	// Cap the minimum timestep physics will use
	unsigned int minTimeStepMs = (unsigned int)(1000.0/60.0); // 60 FPS, converted to ms and trucated to int
	int simCount = 0;
	// Limit how many simulations we'll run when it's slow, to avoid death spiral
	int maxSimulations = 5;
	unsigned int oldTimeMs = SDL_GetTicks();
	
	// Game loop
	while (gameState!= GameState::EXIT) {
		processSDLEvents();

		// Update the player and AI cars
		inputEngine->getInput();
		aiEngine->updateAI();

		// Figure out timestep and run physics
		unsigned int newTimeMs = SDL_GetTicks();
		unsigned int timeStepMs = newTimeMs - oldTimeMs;
		oldTimeMs = newTimeMs;

		// If timestep is small enough, just use it, otherwise break down into smaller chunks
		while (timeStepMs > 0.0 && simCount < maxSimulations) {
			unsigned int deltaTimeMs = min(timeStepMs, minTimeStepMs);
			physicsEngine->simulate(deltaTimeMs);
			timeStepMs -= deltaTimeMs;
			simCount++;
		}
		simCount = 0;

		// Render
		//renderingEngine->pushEntities();
		renderingEngine.displayFunc(entities);

		SDL_GL_SwapWindow(window);
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
	for (int i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
	for (int i = 0; i < (int)renderables.size(); i++) {
		delete renderables[i];
	}
}