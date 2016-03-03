#include "graphNode.h"


graphNode::graphNode(void)
{
	
}


graphNode::~graphNode(void)
{
}

void graphNode::addNeighbour(graphNode * newNeighbour)
{
	neighbours.push_back(newNeighbour);
}

void graphNode::setPosition(glm::vec3 position)
{
	this->position = position;
}
