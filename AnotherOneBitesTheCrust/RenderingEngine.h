/*
* The class responsible for rendering functions. Implemented as a singleton for the sake of GLUT 
  needing a function callback to display.
*/

#pragma once
#include <vector>
#include "TestEntity.h"

using namespace std;

class RenderingEngine
{
public:
	RenderingEngine(void);
	~RenderingEngine(void);

	void PushEntities();
	void Draw();
};

