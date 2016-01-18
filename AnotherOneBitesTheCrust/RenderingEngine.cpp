#include "RenderingEngine.h"
#include "TestEntity.h"
#include "Game.h"
#include <math.h>
#include <iostream>
#include <GL/glut.h>

RenderingEngine::RenderingEngine() {}

// Singleton for the sake of glut needing a static callback to render
RenderingEngine* RenderingEngine::GetInstance() {
	static RenderingEngine instance;
	return &instance;
}

void RenderingEngine::RenderCallback() {
	RenderingEngine::GetInstance()->Draw();
}

// Initialize GLUT, return false if it fails. Could take in command line commands, for glutInit.
bool RenderingEngine::Initialize(int argc, char* argv[]) 
{
	// Initialize some variables
	windowWidth = 1280.0;
	windowHeight = 800.0;
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutCreateWindow("AoBtC Test");
	glutDisplayFunc(RenderCallback);
	glutIdleFunc(Game::IdleCallback);
	return true;
}

// Push all the entity data to the renderer. In the future this will be a list of renderableEntities, which have
// VBOs and the position/orientation needed for figuring out where in the world they are
void RenderingEngine::PushEntities(vector<TestEntity*> e) {
	entities = e;
}

void RenderingEngine::Draw() {
	// Draw all our entities
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	// Our entities will have been preloaded into vertex buffers so we'll just be iterating over them, 
	// instead of doing every vertex every time like this.
	glBegin(GL_POLYGON);
	for (int i = 0; i < entities.size(); i++) {
		TestEntity *entity = entities[i];
		for (int j = 0; j < entity->vertices.size(); j++) {
			glVertex3f(entity->vertices[j].x, entity->vertices[j].y, entity->vertices[j].z);
		}
	}
	glEnd();
	glFlush();
	
	glutSwapBuffers();
}

RenderingEngine::~RenderingEngine(void) {}
