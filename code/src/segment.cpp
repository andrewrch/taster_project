#include "segment.hpp"

using namespace std;

Segment::Segment(string name, glm::mat4 rotation, double length)
{
	// Reference copy string should be enough
	this->name = name;
	this->joint = Joint(rotation);

	this->parent = NULL;
	this->child = NULL;
	
	this->translation = glm::translation(glm::vec3(0.0, length, 0.0));
}

Segment::~Segment()
{
}
