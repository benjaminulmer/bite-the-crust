#pragma once
#include <vector>
#include "DecorationEntity.h"
#include "GameState.h"
#include "ContentLoading.h"
#include "RenderingEngine.h"

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
	glm::mat4 menusM;
	glm::mat4 menusV;

	void menuInput(InputType type);
	void pauseInput(InputType type);
	void updateMenu();
	void updatePaused();
	GLuint selected;
	GLuint unselected;

	int currentMenuSelection;
	int currentPauseSelection;
	int menuState;

	sigslot::signal1<GameState> gameStateSelected;
};

