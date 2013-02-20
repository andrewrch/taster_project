#ifndef HAND_H
#define HAND_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "pipeline.hpp"

static const unsigned int HAND_SPHERES = 22;
static const unsigned int HAND_CYLINDERS = 15;
static const unsigned int NUM_PARAMETERS = 27;

// Static ints for param array accesses
// Global position
static const unsigned int GLOBAL_POS_X = 0;
static const unsigned int GLOBAL_POS_Y = 1;
static const unsigned int GLOBAL_POS_Z = 2;
// Global rotation
static const unsigned int GLOBAL_ROT_X = 3;
static const unsigned int GLOBAL_ROT_Y = 4;
static const unsigned int GLOBAL_ROT_Z = 5;
static const unsigned int GLOBAL_ROT_W = 6;

// Fingers
static const unsigned int FINGER_1 = 7;
static const unsigned int FINGER_2 = 11;
static const unsigned int FINGER_3 = 15;
static const unsigned int FINGER_4 = 19;

// Rotations
static const unsigned int ROT_1X = 0;
static const unsigned int ROT_1Z = 1;
static const unsigned int ROT_2Z = 2;
static const unsigned int ROT_3Z = 3;

// Thumb
static const unsigned int THUMB_ROT_1X = 23;
static const unsigned int THUMB_ROT_1Z = 24;
static const unsigned int THUMB_ROT_2Z = 25;
static const unsigned int THUMB_ROT_3Z = 26;

class Hand
{
  public:
    Hand(Pipeline&);
    ~Hand();
    void initialiseHand(
        glm::mat4*, 
        glm::mat4*, 
        unsigned int, 
        float[NUM_PARAMETERS]);
  private:
    Pipeline& p;
    void initialiseFinger(
        unsigned int,
        glm::mat4*, 
        glm::mat4*, 
        glm::vec3,
        unsigned int, 
        unsigned int*, 
        unsigned int*, 
        float,
        float,
        float[]);

    void initialiseThumb(
        glm::mat4*, 
        glm::mat4*, 
        glm::vec3,
        unsigned int, 
        unsigned int*, 
        unsigned int*, 
        float,
        float,
        float[]);

    void initialiseJoint(
        glm::mat4*, 
        glm::mat4*, 
        glm::vec3,
        unsigned int, 
        unsigned int*, 
        unsigned int*, 
        float,
        float,
        float[]);
};

#endif
