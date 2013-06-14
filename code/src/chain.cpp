#include "chain.hpp"

using namespace std;

Chain::Chain(string name)
{
	this->name = name;
	this->parent = parent;
}

void Chain::addChain(string name)
{
	chains[name] = Chain(name, &this.leaf);
}

void Chain::addSegment(std::string name, Segment segment)
{
	segments[name] = segment;

}

Segment* getSegment(string name)
{
	// Find the segment in the map
	return segments[name];
}

Chain* getChain(string name)
{
	// Find the chain in the map
	return chains[name];
}

Segment* Chain::getRoot()
{
	return root;
}

Segment* Chain::getParent()
{
	return *parent;
}
