#pragma once
class PhysicsEngine
{
public:
	PhysicsEngine(void);
	~PhysicsEngine(void);

	void Simulate(unsigned int deltaTimeMs);
};

