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

	renderingEngine.testOBJLoading();

}

void Game::setupEntities() {
	Renderable* plane = new Renderable();
	//add vertices and colors
	plane->addPoint(vec3(20,0,20),vec3(1,0,0));
	plane->addPoint(vec3(20,0,-20),vec3(0,1,0));
	plane->addPoint(vec3(-20,0,-20),vec3(0,0,1));
	plane->addPoint(vec3(-20,0,20),vec3(1,1,1));
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

	Renderable* box = new Renderable();
	box->addPoint(vec3(-0.5,-0.5,-0.5), vec3(1,0,0));
	box->addPoint(vec3(0.5,-0.5,-0.5), vec3(0,1,0));
	box->addPoint(vec3(0.5,0.5,-0.5), vec3(0,0,1));
	box->addPoint(vec3(-0.5,0.5,-0.5), vec3(1,1,1));
	box->addPoint(vec3(-0.5,-0.5,0.5), vec3(0,1,1));
	box->addPoint(vec3(0.5,-0.5,0.5), vec3(1,0,1));
	box->addPoint(vec3(0.5,0.5,0.5), vec3(1,1,0));
	box->addPoint(vec3(-0.5,0.5,0.5), vec3(1,1,1));

	box->createFace(0);
	box->createFace(1);
	box->createFace(2);
	box->createFace(2);
	box->createFace(3);
	box->createFace(0);
	
	box->createFace(1);
	box->createFace(5);
	box->createFace(6);
	box->createFace(6);
	box->createFace(2);
	box->createFace(1);

	box->createFace(0);
	box->createFace(4);
	box->createFace(7);
	box->createFace(7);
	box->createFace(3);
	box->createFace(0);

	box->createFace(4);
	box->createFace(5);
	box->createFace(6);
	box->createFace(6);
	box->createFace(7);
	box->createFace(4);

	box->createFace(0);
	box->createFace(1);
	box->createFace(5);
	box->createFace(5);
	box->createFace(4);
	box->createFace(0);

	box->createFace(2);
	box->createFace(6);
	box->createFace(7);
	box->createFace(7);
	box->createFace(3);
	box->createFace(2);

	renderables.push_back(box);
	renderingEngine.assignBuffers(box);

	Renderable* vehicle = new Renderable();
 	vehicle->addPoint(vec3(-1.75,-1,2.5), vec3(1,0,0));
 	vehicle->addPoint(vec3(1.75,-1,2.5), vec3(0,1,0));
 	vehicle->addPoint(vec3(1.75,1,2.5), vec3(0,0,1));
 	vehicle->addPoint(vec3(-1.75,1,2.5), vec3(1,1,1));
 	vehicle->addPoint(vec3(-1.75,-1,-2.5), vec3(0,1,1));
 	vehicle->addPoint(vec3(1.75,-1,-2.5), vec3(1,0,1));
 	vehicle->addPoint(vec3(1.75,1,-2.5), vec3(1,1,0));
 	vehicle->addPoint(vec3(-1.75,1,-2.5), vec3(1,1,1));
 
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

	Renderable* van = new Renderable();
	vector<vec3>vanVerts;
	vector<vec3>vanNormals;
	vector<GLuint>vanFaces;
	bool res = renderingEngine.loadOBJ("res\\Models\\Van.obj", vanVerts, vanNormals, vanFaces);

	cout << "Faces of van " << vanVerts.size() << endl;
	van->setPoints(vanVerts);
	van->setFaces(vanFaces);
	renderables.push_back(van);
	renderingEngine.assignBuffers(van);

	Entity* ground = new Entity();
	ground->setPosition(vec3(0,0,0));
	ground->setRenderable(plane);
	ground->setDefaultRotation(0,vec3(0,1,0));
	ground->setDefaultTranslation(vec3(0.0f));
	ground->setDefaultScale(vec3(1.0f));
	entities.push_back(ground);

	Entity* tri = new Entity();
	tri->setPosition(vec3(-1,1, 1.0));		//change position here
	tri->setRenderable(triangle);
	//entities.push_back(tri);

	PhysicsEntity* pizzaBox = new PhysicsEntity();
	pizzaBox->setRenderable(box);
	physicsEngine->initDynamicEntity(pizzaBox, 1.5f, 3.0f, -2.0f);
	entities.push_back(pizzaBox);

	/*PhysicsEntity* pizzaBox2 = new PhysicsEntity();
	pizzaBox2->setRenderable(box);
	pizzaBox2->setDefaultRotation(-1.5708f,vec3(0,1,0));
	pizzaBox2->setDefaultTranslation(vec3(2.5f, 2.2f, 3.0f));
	pizzaBox2->setDefaultScale(vec3(1.0f));
	physicsEngine->initDynamicEntity(pizzaBox2);
	entities.push_back(pizzaBox2);

	PhysicsEntity* pizzaBox3 = new PhysicsEntity();
	pizzaBox3->setRenderable(box);
	pizzaBox3->setDefaultRotation(-1.5708f,vec3(0,1,0));
	pizzaBox3->setDefaultTranslation(vec3(-2.5f, 2.2f, 1.0f));
	pizzaBox3->setDefaultScale(vec3(1.0f));
	physicsEngine->initDynamicEntity(pizzaBox3);
	entities.push_back(pizzaBox3);*/

	//playerVehicle = new Vehicle();
	//ContentLoading::loadVehicleData("res\\JSON\\car.json", playerVehicle);
	//playerVehicle->setRenderable(vehicle);
	//physicsEngine->initVehicle(playerVehicle);
	//entities.push_back(playerVehicle);

	playerVehicle = new Vehicle();
	ContentLoading::loadVehicleData("res\\JSON\\car.json", playerVehicle);
	playerVehicle->setRenderable(vehicle);
	physicsEngine->initVehicle(playerVehicle);
	entities.push_back(playerVehicle);

	/*playerVehicle = new Vehicle();
	ContentLoading::loadVehicleData("res\\JSON\\car.json", playerVehicle);
	playerVehicle->setRenderable(van);
	playerVehicle->setDefaultRotation(-1.5708f,vec3(0,1,0));
	playerVehicle->setDefaultTranslation(vec3(0.0f, 0.0f, -8.0f));
	playerVehicle->setDefaultScale(vec3(1.0f));
	physicsEngine->initVehicle(playerVehicle);
	entities.push_back(playerVehicle); */

	//set camera
	camera.setPosition(glm::vec3(0,3,8));			//location of camera
	camera.setLookAtPosition(glm::vec3(0,2,0));		//where camera is pointing
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
		//cout << physicsEngine->getPosX() << " " << physicsEngine->getPosY() << " " << physicsEngine->getPosZ() << endl;
		//cout << playerVehicle->getPosition().x << " " << playerVehicle->getPosition().y << " " << playerVehicle->getPosition().z << endl;

		camera.setPosition(playerVehicle->getPosition() + glm::vec3(playerVehicle->getModelMatrix() * glm::vec4(0,8,-20,0)));
		camera.setLookAtPosition(playerVehicle->getPosition());
		renderingEngine.updateView(camera);

		//display
		renderingEngine.displayFunc(entities);

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
