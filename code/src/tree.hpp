/*
 *  This is a class to represent the whole kinematic
 *  tree.  It's really a kind of wrapper for a chain
 *  which contains the global position and orientation
 *  of the tree.
 *
 *  The tree has a name, for instance "Body".  It acts
 *  almost exactly like a chain, so it can either just
 *  be a list of segments, and/or can contain a map 
 *  of other chains.
 */

#ifndef TREE_H
#define TREE_H

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "chain.hpp"

class Tree {

	public:
		Tree(std::string name, glm::mat4 position, glm::mat4 rotation);
		~Tree();
		void addChain(std::string name);
		void addSegment(std::string name, Joint joint, double length);

		Chain* getChain(std::string);

	private:
		std::string name;
		Chain rootChain;
		glm::mat4 globalPosition;
		glm::mat4 globalRotation;

};

#endif
