#ifndef HAND_H
#define HAND_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "pipeline.hpp"

static const unsigned int NUM_SPHERES = 21;
static const unsigned int NUM_CYLINDERS = 15;
static const unsigned int NUM_PARAMETERS = 27;

class Hand
{
  public:
    Hand(double[NUM_PARAMETERS]);

    inline void addToTileArrays(
        glm::mat4* spheres,
        glm::mat4* cylinders,
        unsigned int tile,
        glm::mat4 proj)
    {
      for (unsigned int i = 0; i < NUM_SPHERES; i++)
        spheres[tile * NUM_SPHERES + i] = proj * sphereWVPs[i];

      for (unsigned int i = 0; i < NUM_CYLINDERS; i++)
        cylinders[tile * NUM_CYLINDERS + i] = proj * cylinderWVPs[i];
    }

    inline void addToWVPArrays(
        glm::mat4* spheres,
        glm::mat4* cylinders,
        unsigned int tile,
        glm::mat4 proj)
    {
      for (unsigned int i = 0; i < NUM_SPHERES; i++)
        spheres[tile * NUM_SPHERES + i] = proj * sphereWVPs[i];

      for (unsigned int i = 0; i < NUM_CYLINDERS; i++)
        cylinders[tile * NUM_CYLINDERS + i] = proj * cylinderWVPs[i];
    }

    inline void addToWVArrays(
        glm::mat4* spheres,
        glm::mat4* cylinders,
        unsigned int tile,
        glm::mat4 proj)
    {
      for (unsigned int i = 0; i < NUM_SPHERES; i++)
        //spheres[tile * NUM_SPHERES + i] = proj * sphereWVPs[i];
        spheres[tile * NUM_SPHERES + i] = sphereWVPs[i];

      for (unsigned int i = 0; i < NUM_CYLINDERS; i++)
        //cylinders[tile * NUM_CYLINDERS + i] = proj * cylinderWVPs[i];
        cylinders[tile * NUM_CYLINDERS + i] = cylinderWVPs[i];
    }

    inline void printPosition()
    {
      glm::vec4 v(1.0f);
      v[0] = v[1] = v[2] = 0;
      glm::vec4 r;
      r = sphereWVPs[18] * v;
      std::cerr << "Thumb: " << -r[0] << " " << r[1] << " " << r[2] << std::endl;
      r = sphereWVPs[3] * v;
      std::cerr << "Index: " << -r[0] << " " << r[1] << " " << r[2] << std::endl;
      r = sphereWVPs[7] * v;
      std::cerr << "Middle: " << -r[0] << " " << r[1] << " " << r[2] << std::endl;
      r = sphereWVPs[11] * v;
      std::cerr << "Ring: " << -r[0] << " " << r[1] << " " << r[2] << std::endl;
      r = sphereWVPs[15] * v;
      std::cerr << "Little: " << -r[0] << " " << r[1] << " " << r[2] << std::endl;
    }

    static bool outOfBounds(unsigned int param, double val)
    {
      switch(param)
      {
        case 0:
        case 1:
        case 2:
          break;
        case 3:
        case 4:
        case 5:
        case 6:
          break;
          // Index
        case 7:
          if (val < -M_PI/8 || val > M_PI/8)
            return true;
          break;
        case 8:
        case 9:
        case 10:
          if (val < 0 || val > M_PI/2)
            return true;
          break;
          // Middle
        case 11:
          if (val < -M_PI/8 || val > M_PI/8)
            return true;
          break;
        case 12:
        case 13:
        case 14:
          if (val < 0 || val > M_PI/2)
            return true;
          break;
          // Ring
        case 15:
          if (val < -M_PI/8 || val > M_PI/8)
            return true;
          break;
        case 16:
        case 17:
        case 18:
          if (val < 0 || val > M_PI/2)
            return true;
          break;
          // Little
        case 19:
          if (val < -M_PI/8 || val > M_PI/8)
            return true;
          break;
        case 20:
        case 21:
        case 22:
          if (val < 0 || val > M_PI/2)
            return true;
          break;
          // Thumb
//        case 23:
//          if (val > 0.68 || val < 0.28)
//            return true;
//          break;
//        case 24:
//          if (val > M_PI/8 || val < -M_PI/8)
//            return true;
//          break;
//        case 25:
//          if (val < -M_PI/16 || val > M_PI/2)
//            return true;
//        case 26:
//          if (val < -M_PI/8 || val > M_PI/2)
//            return true;
        case 23:
          if (val < -M_PI/8 || val > M_PI / 4)
            return true;
          break;
        case 24:
          if (val < -M_PI/8 || val > M_PI / 4)
            return true;
          break;
        case 25:
          if (val < -M_PI/16 || val > M_PI/2)
            return true;
        case 26:
          if (val < -M_PI/4 || val > M_PI/2)
            return true;

          break;
        default:
          return false;
      }
      return false;
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
