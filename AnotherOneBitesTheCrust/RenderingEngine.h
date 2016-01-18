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
	static RenderingEngine* GetInstance();
	static void RenderCallback();
private:
	RenderingEngine(void);
	~RenderingEngine(void);
	RenderingEngine(RenderingEngine const&); // Don't implement default copy constructor
	void operator=(RenderingEngine const&); // Don't implement assignment
public:
	// members
	double windowWidth, windowHeight;
	vector<TestEntity*> entities;

	bool Initialize(int argc, char* argv[]);
	void PushEntities(vector<TestEntity*> e);
	void Draw();
};

