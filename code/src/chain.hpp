/*
 * This class represents a chain of segments.
 * 
 * Typically this will be a limb, or finger in 
 * a kinematic tree.
 *
 * The leaf in a Chain can be either a segment
 * or a map of Chains.  This accomodates things like
 * a hand at the end of an arm.
 */
class Chain {

	public:
		Chain(glm::mat4 origin);
		~Chain();

		Joint* getRoot();
		Joint* getParent();

	private:
		Joint root;
		Joint* parent;
		//Joint* leaf;

};
