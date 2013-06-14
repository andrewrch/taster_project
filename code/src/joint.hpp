/*
 * This class represents a joint in a kinematic
 * tree.
 *
 * The joint has an associated rotation, a parent,
 * and a child.
 *
 * Each joint can have a maximum of 1 child.
 */

#ifndef JOINT_H
#define JOINT_H

#include <string>
#include <unordered_map>

class Joint {

	public:
		Joint(Joint* parent);
		~Joint();

		Joint* getParent() { return this.parent; }
		Joint* getChild() { return this.child; }

	private:
		Joint* parent, child;
		std::unordered_map<std::string, Chain> chain;

		glm::mat4 rotation;
};

#endif
