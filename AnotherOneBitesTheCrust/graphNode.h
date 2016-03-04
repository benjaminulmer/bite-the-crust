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
	bool sameLocation(graphNode*);
	bool sameLocation(glm::vec3);

	std::vector<graphNode*> getNeighbours();
	glm::vec3 getPosition();

private:
	std::vector<graphNode*> neighbours;
	glm::vec3 position;
};

