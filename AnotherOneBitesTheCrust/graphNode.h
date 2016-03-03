#pragma once
#include <vector>
#include <glm.hpp>

class graphNode
{
public:
	graphNode(void);
	~graphNode(void);

	void addNeighbour(graphNode*);
	void setPosition(glm::vec3);

private:
	std::vector<graphNode*> neighbours;
	glm::vec3 position;
};

