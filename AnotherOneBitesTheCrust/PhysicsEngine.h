#pragma once
class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void simulate(unsigned int deltaTimeMs);
};

