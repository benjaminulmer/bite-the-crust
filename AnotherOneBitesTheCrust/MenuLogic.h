#pragma once
#include <vector>
#include "NonPhysicsEntity.h"
#include "GameState.h"
#include "ContentLoading.h"
#include "RenderingEngine.h"

enum class MenuOptions
{
	PLAY = 0,
	HOW_TO,
	CONTROLS,
	STORY,
	EXIT,
	MAX,
};

enum class PauseOptions
{
	RESUME = 0,
	RESTART,
	MAIN_MENU,
	DESKTOP,
	MAX
};


class MenuLogic :
	public sigslot::has_slots<>
{
public:
	MenuLogic(RenderingEngine* re);
	~MenuLogic(void);

	RenderingEngine* renderingEngine;

	void setupMenus();

	std::vector<Entity*> introEntities;
	std::vector<Entity*> menuEntities;
	std::vector<Entity*> pausedEntities;
	std::vector<Entity*> backgroundEntities;
	std::vector<Entity*> playerSelectEntities;
	glm::mat4 menusM;
	glm::mat4 menusV;

	void menuInput(InputType type);
	void pauseInput(InputType type);
	void updateMenu();
	void updatePaused();
	GLuint selected;
	GLuint unselected;

	MenuOptions menuSelection;
	int menuDepth;
	PauseOptions pauseSelection;
	int numPlayers;

	sigslot::signal1<GameState> gameStateSelected;
};

