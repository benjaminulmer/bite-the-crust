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

glm::vec3 graphNode::getPosition()
{
	return position;
}

std::vector<graphNode*> graphNode::getNeighbours()
{
	return neighbours;
}

bool graphNode::sameLocation(graphNode * comparing)
{
	glm::vec3 comparingPosition = comparing->getPosition();

	if(position.x == comparingPosition.x && position.y == comparingPosition.y && position.z == comparingPosition.z)
		return true;
	return false;
}

bool graphNode::sameLocation(glm::vec3 comparingPosition)
{

	if(position.x == comparingPosition.x && position.y == comparingPosition.y && position.z == comparingPosition.z)
		return true;
	return false;
}
