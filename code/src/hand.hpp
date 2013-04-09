#ifndef HAND_H
#define HAND_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "pipeline.hpp"

static const unsigned int NUM_SPHERES = 22;
static const unsigned int NUM_CYLINDERS = 15;
static const unsigned int NUM_PARAMETERS = 27;

class Hand
{
  public:
    Hand(double[NUM_PARAMETERS]);
    // Push this hand into arrays for instanced rendering
//    inline void addToTileArrays(
//        glm::mat4*,         // Spheres
//        glm::mat4*,         // Cylinders
//        unsigned int,       // Tile number
//        Pipeline&);         // Pipeline for (Tile)VP info
//
//    inline void addToWVPArrays(
//        glm::mat4*,         // Spheres
//        glm::mat4*,         // Cylinders
//        unsigned int,       // Tile number
//        Pipeline&);         // Pipeline for VP info
//
//    inline void addToWVArrays(
//        glm::mat4*,         // Spheres
//        glm::mat4*,         // Cylinders
//        unsigned int,       // Tile number
//        Pipeline&);         // Pipeline for VP info

    inline void addToTileArrays(
        glm::mat4* spheres,
        glm::mat4* cylinders,
        unsigned int tile,
        Pipeline& p)
    {
      glm::mat4 t = p.getTileTrans(tile);
      for (unsigned int i = 0; i < NUM_SPHERES; i++)
        spheres[tile * NUM_SPHERES + i] = t * sphereWVPs[i];

      for (unsigned int i = 0; i < NUM_CYLINDERS; i++)
        cylinders[tile * NUM_CYLINDERS + i] = t * cylinderWVPs[i];
    }

    inline void addToWVPArrays(
        glm::mat4* spheres,
        glm::mat4* cylinders,
        unsigned int tile,
        Pipeline& p)
    {
      glm::mat4 t = p.getVPTrans();
      for (unsigned int i = 0; i < NUM_SPHERES; i++)
        spheres[tile * NUM_SPHERES + i] = t * sphereWVPs[i];

      for (unsigned int i = 0; i < NUM_CYLINDERS; i++)
        cylinders[tile * NUM_CYLINDERS + i] = t * cylinderWVPs[i];
    }

    inline void addToWVArrays(
        glm::mat4* spheres,
        glm::mat4* cylinders,
        unsigned int tile,
        Pipeline& p)
    {
      glm::mat4 t = p.getVTrans();
      for (unsigned int i = 0; i < NUM_SPHERES; i++)
        spheres[tile * NUM_SPHERES + i] = t * sphereWVPs[i];

      for (unsigned int i = 0; i < NUM_CYLINDERS; i++)
        cylinders[tile * NUM_CYLINDERS + i] = t * cylinderWVPs[i];
    }


    ~Hand();
  private:
    enum Digit {
      FINGER_1 = 0,
      FINGER_2 = 1,
      FINGER_3 = 2,
      FINGER_4 = 3,
      THUMB = 4};

    // Called from constructor
    void initialiseHand(double[NUM_PARAMETERS]);

    void initialisePalm(
        glm::mat4           // Position
        );

    void initialiseFinger(
        Digit,              // Which finger?
        glm::mat4,          // start position of finger
        double[NUM_PARAMETERS]); // Parameters

    void initialiseThumb(
        glm::mat4,
        double[NUM_PARAMETERS]);

    glm::mat4 initialiseJoint(
        glm::mat4,          // The current WVP state
        double*,              // X rotation
        double*,              // Z rotation
        double,              // Initial diameter
        double,              // Initial length
        double);             // Cone ratio

    // These hold WVP information for each primitive
    // in the hand
    std::vector<glm::mat4> sphereWVPs;
    std::vector<glm::mat4> cylinderWVPs;

    // Some attributes about the hand
    glm::vec3 
      palmPos, palmScale,
      finger1Pos, 
      finger2Pos, 
      finger3Pos, 
      finger4Pos, 
      thumbPos;

    // Gets 
    inline unsigned int getParamPos(unsigned int digit, unsigned int offset)
    {
      return 4 * (digit+1) + 3 + offset;
    }
    // Static ints for param array accesses
    // Global position
    const static unsigned int GLOBAL_POS_X = 0;
    const static unsigned int GLOBAL_POS_Y = 1;
    const static unsigned int GLOBAL_POS_Z = 2;
    // Global rotation (Quaternion)
    const static unsigned int GLOBAL_ROT_X = 3;
    const static unsigned int GLOBAL_ROT_Y = 4;
    const static unsigned int GLOBAL_ROT_Z = 5;
    const static unsigned int GLOBAL_ROT_W = 6;
    // Rotations
    const static unsigned int ROT_1X = 0;
    const static unsigned int ROT_1Z = 1;
    const static unsigned int ROT_2Z = 2;
    const static unsigned int ROT_3Z = 3;

    const static double diameters[5];
    const static double lengths[5];
};

#endif
