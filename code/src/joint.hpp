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

class Joint {

	public:

		friend class Segment;

		Joint(glm::mat4 rotation);
		~Joint();

		Joint* getParent() { return this.parent; }
		Joint* getChild() { return this.child; }

	private:
		Joint* parent;
		Joint* child;
		glm::mat4 rotation;
};

#endif
