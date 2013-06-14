/*
 * This class represents a chain of segments.
 * 
 * Typically this will be a limb, or finger in 
 * a kinematic tree.
 *
 * The leaf in a Chain can be either a segment
 * or a map of Chains.  This accomodates things like
 * a hand at the end of an arm.
 *
 * If the map is empty then this is a terminating chain
 * e.g. a finger.
 *
 * If the map is not empty and contains other chains 
 * then this is a kind of limb, e.g. an arm.
 */

#ifndef CHAIN_H
#define CHAIN_H

#include <map>
#include <string>

#include "segment.hpp"

class Chain {

	public:
		Chain(std::string name, Segment** parent);
		~Chain();

		void addChain(std::string name);
		void addSegment(std::string name, glm::mat4 rotation, double length);

		Segment* getRoot();
		Segment* getParent();

		Chain* getChain(std::string);
		Segment* getSegment(std::string);

	private:
		std::string name;
		// Pointer to a pointer since a new segment may be
		// added to the end of the parent chain after the 
		// creation of this chain
		Segment** parent;
		Segment* root;
		Segment* leaf;

		// Contain the segments and leaf chains for
		// easy lookup
		std::map<std::string, Segment> segments;
		std::map<std::string, Chain> chains;

};

#endif
