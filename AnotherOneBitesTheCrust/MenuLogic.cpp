#include "MenuLogic.h"

MenuLogic::MenuLogic(RenderingEngine* re)
{
	renderingEngine = re;
	menuState = 0;
	currentMenuSelection = 1;
	currentPauseSelection = 1;

	setupMenus();
}

void MenuLogic::setupMenus()
{
	Renderable *logo = ContentLoading::createRenderable("res\\Models\\deliverers.obj");
	Renderable *gameBy = ContentLoading::createRenderable("res\\Models\\gameby.obj");
	Renderable *names = ContentLoading::createRenderable("res\\Models\\names.obj");
	GLuint introTexture = ContentLoading::loadDDS("res\\Textures\\intro-colored.DDS");

	renderingEngine->assignBuffersTex(logo);
	renderingEngine->assignBuffersTex(gameBy);
	renderingEngine->assignBuffersTex(names);

	Entity *eLogo = new Entity();
	eLogo->setRenderable(logo);
	eLogo->setTexture(introTexture);
	introEntities.push_back(eLogo);
	
	Entity *eGameBy = new Entity();
	eGameBy->setRenderable(gameBy);
	eGameBy->setTexture(introTexture);
	introEntities.push_back(eGameBy);

	Entity *eNames = new Entity();
	eNames->setRenderable(names);
	eNames->setTexture(introTexture);
	introEntities.push_back(eNames);

	menusM = glm::mat4(1.0f);
	menusV = glm::lookAt(
			glm::vec3(0,0,7), 
			glm::vec3(0,0,0), 
			glm::vec3(0,1,0)
		);


	//loading menus stuff
	Renderable *aobtc = ContentLoading::createRenderable("res\\Models\\AOBTC.obj");
	Renderable *play = ContentLoading::createRenderable("res\\Models\\play.obj");
	Renderable *howtoplay = ContentLoading::createRenderable("res\\Models\\howtoplay.obj");
	Renderable *controls = ContentLoading::createRenderable("res\\Models\\controls.obj");
	Renderable *story = ContentLoading::createRenderable("res\\Models\\story.obj");
	Renderable *exit = ContentLoading::createRenderable("res\\Models\\exit.obj");
	Renderable *billboard = ContentLoading::createRenderable("res\\Models\\Billboard.obj");
	
	GLuint aobtcTexture = ContentLoading::loadDDS("res\\Textures\\AOBTC-colored.DDS");
	GLuint billboardHowto = ContentLoading::loadDDS("res\\Textures\\Billboard-howto.DDS");
	GLuint billboardControls = ContentLoading::loadDDS("res\\Textures\\Billboard-controls.DDS");
	selected = ContentLoading::loadDDS("res\\Textures\\selected.DDS");
	unselected = ContentLoading::loadDDS("res\\Textures\\unselected.DDS");
	
	renderingEngine->assignBuffersTex(aobtc);
	renderingEngine->assignBuffersTex(play);
	renderingEngine->assignBuffersTex(howtoplay);
	renderingEngine->assignBuffersTex(controls);
	renderingEngine->assignBuffersTex(story);
	renderingEngine->assignBuffersTex(exit);
	renderingEngine->assignBuffersTex(billboard);

	Entity *eAOBTC = new Entity();
	eAOBTC->setRenderable(aobtc);
	eAOBTC->setTexture(aobtcTexture);
	menuEntities.push_back(eAOBTC);

	Entity *ePlay = new Entity();
	ePlay->setRenderable(play);
	ePlay->setTexture(selected);
	menuEntities.push_back(ePlay);

	Entity *eHowToPlay = new Entity();
	eHowToPlay->setRenderable(howtoplay);
	eHowToPlay->setTexture(unselected);
	menuEntities.push_back(eHowToPlay);

	Entity *eControls = new Entity();
	eControls->setRenderable(controls);
	eControls->setTexture(unselected);
	menuEntities.push_back(eControls);

	Entity *eStory = new Entity();
	eStory->setRenderable(story);
	eStory->setTexture(unselected);
	menuEntities.push_back(eStory);

	Entity *eExit = new Entity();
	eExit->setRenderable(exit);
	eExit->setTexture(unselected);
	menuEntities.push_back(eExit);

	DecorationEntity *eBillboardHowto = new DecorationEntity();
	eBillboardHowto->setRenderable(billboard);
	eBillboardHowto->setTexture(billboardHowto);
	eBillboardHowto->translate(glm::vec3(13,0,0));
	menuEntities.push_back(eBillboardHowto);

	DecorationEntity *eBillboardControls = new DecorationEntity();
	eBillboardControls->setRenderable(billboard);
	eBillboardControls->setTexture(billboardControls);
	eBillboardControls->translate(glm::vec3(-13,0,0));
	menuEntities.push_back(eBillboardControls);

	//loading paused stuff
	Renderable *paused = ContentLoading::createRenderable("res\\Models\\paused.obj");
	Renderable *resume = ContentLoading::createRenderable("res\\Models\\resume.obj");
	Renderable *restart = ContentLoading::createRenderable("res\\Models\\restart.obj");
	Renderable *exitMain = ContentLoading::createRenderable("res\\Models\\exittomain.obj");
	Renderable *exitDesk = ContentLoading::createRenderable("res\\Models\\exittodesk.obj");
	renderingEngine->assignBuffersTex(paused);
	renderingEngine->assignBuffersTex(resume);
	renderingEngine->assignBuffersTex(restart);
	renderingEngine->assignBuffersTex(exitMain);
	renderingEngine->assignBuffersTex(exitDesk);

	Entity *ePaused = new Entity();
	ePaused->setRenderable(paused);
	ePaused->setTexture(aobtcTexture);
	pausedEntities.push_back(ePaused);

	Entity *eResume = new Entity();
	eResume->setRenderable(resume);
	eResume->setTexture(selected);
	pausedEntities.push_back(eResume);

	Entity *eRestart = new Entity();
	eRestart->setRenderable(restart);
	eRestart->setTexture(unselected);
	pausedEntities.push_back(eRestart);

	Entity *eExitMain = new Entity();
	eExitMain->setRenderable(exitMain);
	eExitMain->setTexture(unselected);
	pausedEntities.push_back(eExitMain);

	Entity *eExitDesk = new Entity();
	eExitDesk->setRenderable(exitDesk);
	eExitDesk->setTexture(unselected);
	pausedEntities.push_back(eExitDesk);
}

void MenuLogic::updateMenu()
{
	for(unsigned int i = 1; i < menuEntities.size()-2; i++)
	{
		if(i == currentMenuSelection)
		{
			menuEntities[i]->setTexture(selected);
		}
		else
		{
			menuEntities[i]->setTexture(unselected);
		}
	}
	renderingEngine->displayMenu(menuEntities, menusM, menusV);
}

void MenuLogic::updatePaused()
{
	for(unsigned int i = 1; i < pausedEntities.size(); i++)
	{
		if(i == currentPauseSelection)
		{
			pausedEntities[i]->setTexture(selected);
		}
		else
		{
			pausedEntities[i]->setTexture(unselected);
		}
	}
	renderingEngine->displayMenu(pausedEntities, menusM, menusV);
}

void MenuLogic::menuInput(InputType type)
{
	if (type == InputType::UP)
	{
		if(menuState == 0)
			currentMenuSelection -= 1;
		if(currentMenuSelection < 1)
			currentMenuSelection = 1;
	}
	else if (type == InputType::DOWN)
	{
		if(menuState == 0)
			currentMenuSelection += 1;
		if(currentMenuSelection > menuEntities.size() - 3)
			currentMenuSelection = menuEntities.size() - 3;
	}
	else if (type == InputType::ENTER)
	{
		if(currentMenuSelection == 1)			//PLAY
		{
			gameStateSelected(GameState::STARTING_GAME);
			menusV = glm::lookAt(
			glm::vec3(0,0,7), 
			glm::vec3(0,0,0), 
			glm::vec3(0,1,0));
		}
		else if(currentMenuSelection == 2)	//HOW TO PLAY
		{
			if(menuState == 0)
				menusV = translate(menusV, glm::vec3(-13,0,0));
			menuState = 1;
		}
		else if(currentMenuSelection == 3)	//CONTROLS
		{
			if(menuState == 0)
				menusV = translate(menusV, glm::vec3(13,0,0));
			menuState = 1;
		}
		else if(currentMenuSelection == menuEntities.size()- 3)	//EXIT
		{
			gameStateSelected(GameState::EXIT);
		}
	}
	else if (type == InputType::BACK)
	{
		if(menuState != 0)
		{
			menusV = glm::lookAt(
				glm::vec3(0,0,7), 
				glm::vec3(0,0,0), 
				glm::vec3(0,1,0)
			);
			menuState = 0;
		}
	}
}

void MenuLogic::pauseInput(InputType type)
{
	if (type == InputType::UP)
	{
		currentPauseSelection -= 1;
		if(currentPauseSelection < 1)
			currentPauseSelection = 1;
	}
	else if (type == InputType::DOWN)
	{
		currentPauseSelection += 1;
		if(currentPauseSelection > pausedEntities.size() - 1)
			currentPauseSelection = pausedEntities.size() - 1;
	}
	else if (type == InputType::ENTER)
	{
		if(currentPauseSelection == 1)
		{
			gameStateSelected(GameState::PLAY);
			currentPauseSelection = 1;
		}
		else if(currentPauseSelection == pausedEntities.size()-2)
			gameStateSelected(GameState::BACK_TO_MENU);
		else if(currentPauseSelection == pausedEntities.size()-1)
			gameStateSelected(GameState::EXIT);
	}
	else if (type == InputType::BACK)
	{
		gameStateSelected(GameState::PLAY);
		currentPauseSelection = 1;
	}
}

MenuLogic::~MenuLogic(void)
{
}
