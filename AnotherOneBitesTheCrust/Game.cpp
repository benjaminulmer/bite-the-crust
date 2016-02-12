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

	mainLoop();
}



void Game::initSystems()
{
	SDL_Init(SDL_INIT_EVERYTHING);		//Initialize SDL

	window = SDL_CreateWindow("Another Bites The Crust", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	
	screen = SDL_GetWindowSurface(window);

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
		printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				//blue background

	renderingEngine.init();
	inputEngine = new InputEngine();
	physicsEngine = new PhysicsEngine();

	//renderingEngine.testOBJLoading();

}

void Game::setupEntities() 
{
	Renderable * floor = new Renderable();
	vector<vec3>floorVerts;
	vector<vec3>floorNormals;
	bool floorRes = renderingEngine.loadOBJNonIndexed("res\\Models\\FlatFloor.obj", floorVerts, floorNormals);
	floor->setVerts(floorVerts);
	floor->setNorms(floorNormals);
	renderables.push_back(floor);
	renderingEngine.assignBuffers(floor);

	Renderable* van = new Renderable();
	vector<vec3>vanVerts;
	vector<vec3>vanNormals;
	bool res = renderingEngine.loadOBJNonIndexed("res\\Models\\Van.obj", vanVerts, vanNormals);
	van->setVerts(vanVerts);
	van->setNorms(vanNormals);
	renderables.push_back(van);
	renderingEngine.assignBuffers(van);

	Entity* ground = new Entity();
	ground->setRenderable(floor);
	ground->setDefaultRotation(0.0f,vec3(0.0f,1.0f,0.0f));
	ground->setDefaultTranslation(vec3(0.0f,0.0f,0.0f));
	ground->setDefaultScale(vec3(1.0f));
	ground->setColor(vec3(1.0f,1.0f,1.0f));
	
	
	playerVehicle = new Vehicle();
	ContentLoading::loadVehicleData("res\\JSON\\car.json", playerVehicle);
	playerVehicle->setRenderable(van);
	playerVehicle->setDefaultRotation(-1.5708f,vec3(0.0f,1.0f,0.0f));
	playerVehicle->setDefaultTranslation(vec3(0.0f));
	playerVehicle->setDefaultScale(vec3(1.0f));
	playerVehicle->setColor(vec3(1.0f,0.0f,0.0f));
	physicsEngine->initVehicle(playerVehicle);
	entities.push_back(playerVehicle);
	entities.push_back(ground);

	//set camera
	camera.setPosition(glm::vec3(0,6,8));			//location of camera
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
		playerVehicle->handleInput(playerInput);
		aiEngine->updateAI();

		// Figure out timestep and run physics
		unsigned int newTimeMs = SDL_GetTicks();
		unsigned int deltaTimeMs = newTimeMs - oldTimeMs;
		oldTimeMs = newTimeMs;

		physicsEngine->simulate(deltaTimeMs, playerInput);
		physicsEngine->fetchSimulationResults();
		//cout << playerVehicle->getPosition().x << " " << playerVehicle->getPosition().y << " " << playerVehicle->getPosition().z << endl;

		camera.setPosition(playerVehicle->getPosition() + glm::vec3(playerVehicle->getModelMatrix() * glm::vec4(0,6,-10,0)));
		camera.setLookAtPosition(playerVehicle->getPosition());
		renderingEngine.updateView(camera);

		//display
		renderingEngine.displayFunc(entities);
		//renderingEngine.draw();


		//swap buffers
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
	for (unsigned int i = 0; i < entities.size(); i++) {
		delete entities[i];
	}
	for (unsigned int i = 0; i < renderables.size(); i++) {
		delete renderables[i];
	}
}
