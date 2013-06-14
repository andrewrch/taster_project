/* 
 * This class represents a segment in a chain of
 * a kinematic tree.
 *
 * The joint gives information about the rotation of
 * the segment, while a seperate matrix gives a translation
 * along the segment.
 */

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

include "joint.hpp"

class Segment {

	// The chain adds segments, and needs to link up
	// segments in the chain (thus access the parent segment)
	friend class Chain;

	public:
		Segment(std::string name, glm::mat4 rotation, double length);
		~Segment();

		Segment* getParent() { return parent; }
		Segment* getChild() { return child; }

	private:
		std::string name;
		Segment* parent;
		Segment* child;
		Joint joint;
		glm::mat4 translation;
};
