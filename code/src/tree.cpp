#include "tree.hpp"

using namespace std;

Tree::Tree(string name, glm::mat4 position, glm::mat4 rotation) :
	rootChain(name, NULL)
{
	this->name = name;
	this->globalPosition = position;
	this->globalRotation = rotation;
}

Tree::~Tree()
{
}

void Tree::addChain(string name)
{
	rootChain.addChain(name);
}

void Tree::addSegment(string name, Joint joint, double length)
{
	rootChain.addSegment(string name, joint, length); 
}

Chain* Tree::getChain(string name)
{
	return rootChain.getChain(name);
}

Segment* Tree::getSegment(string name)
{
	return rootChain.getSegment(name);
}
