/* 
 * This class represents a segment in a chain of
 * a kinematic tree.
 *
 * The joint gives information about the rotation of
 * the segment, while a seperate matrix gives a translation
 * along the segment.

class Segment {
	public:
		Segment(Joint* parent, double length);
		~Segment();

	private:
		Joint* parent;
		Joint joint;
		glm::mat4 translation;
};
