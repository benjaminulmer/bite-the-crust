#pragma once
#include "Vehicle.h"
#include "glm.hpp"
#include <sigslot.h>

enum class ArrowState
{
	LEFT,
	RIGHT,
	ON_SCREEN
};

class Camera
	: public Entity, public sigslot::has_slots<>
{
public:
	Camera(Vehicle* vehicle);
	~Camera(void);

	static const int BUFFER_SIZE = 10;

	void setPosition(glm::vec3 v);
	glm::vec3 getPosition();
	glm::vec3 getUpVector();
	glm::vec3 getLookAtPosition();

	glm::vec3 posBufferForward[BUFFER_SIZE];
	glm::vec3 posBufferReverse[BUFFER_SIZE];
	int posBufferIndex;

	void update();

	ArrowState arrowState;
	void setReverseCam(bool val);
	bool isReverseCam();

private:
	glm::vec3 position;
	glm::vec3 upVector;
	glm::vec3 lookAtPosition;
	bool reverseCam;

	Vehicle* vehicle;
};

