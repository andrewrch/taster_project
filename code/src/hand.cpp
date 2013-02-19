#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "hand.hpp"
#include "cone.hpp"

Hand::Hand(Pipeline& pipe) : p(pipe)
{
}

Hand::~Hand()
{
}

void Hand::initialiseFinger(
    unsigned int finger,
    glm::mat4* spheres, 
    glm::mat4* cylinders, 
    glm::vec3 startPosition,
    unsigned int tile,
    unsigned int* currentSphere,
    unsigned int* currentCylinder,
    float startRadius,
    float startLength,
    float params[NUM_PARAMETERS])
{
  glm::mat4 s, t, r, c, tr;
  glm::quat orientation;
  float coneRatio, coneLength;
  // Holds scale of current obj
  s = glm::scale(s, glm::vec3(startRadius, startRadius, startRadius));

  // SPHERE
  t = glm::translate(t, startPosition);
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = p.getTileTrans(tile) * t * s;

  // CYLINDER
  coneRatio = 0.95f;
  coneLength = startLength;
  s = glm::scale(glm::mat4(1.0f), glm::vec3(startRadius, coneLength, startRadius));
  orientation = glm::quat(glm::vec3(
        glm::radians(params[finger + ROT_1X]), 
        0.0f, 
        glm::radians(params[finger + ROT_1Z])));
  glm::gtc::quaternion::normalize(orientation);
  r = glm::toMat4(orientation);
  c = cone(coneRatio);
  tr = t * r;
  cylinders[tile * HAND_CYLINDERS + (*currentCylinder)++] = p.getTileTrans(tile) * tr * s * c;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * startRadius, 
        coneRatio * startRadius, 
        coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength, 0.0f));
  tr = tr * t;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = p.getTileTrans(tile) * tr * s;

  // CYLINDER
  coneLength = coneLength * 0.66f;

  orientation = glm::quat(glm::vec3(
        0.0f, 
        0.0f, 
        glm::radians(params[finger + ROT_2Z])));
  glm::gtc::quaternion::normalize(orientation);
  r = glm::toMat4(orientation);
  s = glm::scale(glm::mat4(1.0f), glm::vec3(coneRatio * startRadius, coneLength, coneRatio * startRadius));
  coneRatio = coneRatio * 0.95f;
  c = cone(coneRatio);
  tr = tr * r;
  cylinders[tile * HAND_CYLINDERS + (*currentCylinder)++] = p.getTileTrans(tile) * tr * s * c;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * startRadius, 
        coneRatio * startRadius, 
        coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength, 0.0f));
  tr = tr * t;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = p.getTileTrans(tile) * tr * s;

  // CYLINDER
  coneLength = coneLength * 0.66f;

  orientation = glm::quat(glm::vec3(
        0.0f, 
        0.0f, 
        glm::radians(params[finger + ROT_3Z])));
  glm::gtc::quaternion::normalize(orientation);
  r = glm::toMat4(orientation);
  s = glm::scale(glm::mat4(1.0f), glm::vec3(coneRatio * startRadius, coneLength, coneRatio * startRadius));
  coneRatio = coneRatio * 0.95f;
  c = cone(coneRatio);
  tr = tr * r;
  cylinders[tile * HAND_CYLINDERS + (*currentCylinder)++] = p.getTileTrans(tile) * tr * s * c;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * coneRatio * startRadius, 
        coneRatio * coneRatio * startRadius, 
        coneRatio * coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength, 0.0f));
  tr = tr * t;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = p.getTileTrans(tile) * tr * s;
}

void Hand::initialiseThumb(
    glm::mat4* spheres, 
    glm::mat4* cylinders, 
    glm::vec3 startPosition,
    unsigned int tile,
    unsigned int* currentSphere,
    unsigned int* currentCylinder,
    float startRadius,
    float startLength,
    float params[])
{
  glm::mat4 r, s, it, t, tr, c;
  float coneRatio, coneLength;
  glm::quat orientation; 
  // Ball of thumb
  s = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 1.5f));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.3f));
  it = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
  orientation = glm::quat(glm::vec3(
        glm::radians(params[THUMB_ROT_1X]), 
        0.0f, 
        glm::radians(params[THUMB_ROT_1Z])));
  glm::gtc::quaternion::normalize(orientation);
  r = glm::toMat4(orientation);
  tr = t * r;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = p.getTileTrans(tile) * tr * s * it;

  // Sphere
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
  tr = tr * t;
  s = glm::scale(glm::mat4(1.0f), glm::vec3(startRadius, startRadius, startRadius));
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = p.getTileTrans(tile) * tr * s;

  // CYLINDER
  coneLength = startLength;
  coneRatio = 0.95f;
  c = cone(coneRatio);
  s = glm::scale(glm::mat4(1.0f), glm::vec3(startRadius, coneLength, startRadius));
  orientation = glm::quat(glm::vec3(
        0.0f, 
        0.0f, 
        glm::radians(params[THUMB_ROT_2Z])));
  glm::gtc::quaternion::normalize(orientation);
  r = glm::toMat4(orientation);
  tr = tr * r;
  cylinders[tile * HAND_CYLINDERS + (*currentCylinder)++] = p.getTileTrans(tile) * tr * s * c;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * startRadius, 
        coneRatio * startRadius, 
        coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength, 0.0f));
  tr = tr * t;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = p.getTileTrans(tile) * tr * s;

  //CYLINDER
  coneLength = coneLength * 0.66f;
  s = glm::scale(glm::mat4(1.0f), glm::vec3(coneRatio * startRadius, coneLength, coneRatio * startRadius));
  coneRatio = coneRatio * 0.95f;
  c = cone(coneRatio);
  orientation = glm::quat(glm::vec3(
        0.0f, 
        0.0f, 
        glm::radians(params[THUMB_ROT_3Z])));
  glm::gtc::quaternion::normalize(orientation);
  r = glm::toMat4(orientation);
  tr = tr * r;
  cylinders[tile * HAND_CYLINDERS + (*currentCylinder)++] = p.getTileTrans(tile) * tr * s * c;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * startRadius, 
        coneRatio * startRadius, 
        coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength - 0.2, 0.0f));
  tr = tr * t;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = p.getTileTrans(tile) * tr * s;
}

void Hand::initialiseHand(
    glm::mat4* spheres, 
    glm::mat4* cylinders, 
    unsigned int tile,
    float params[])
{
  unsigned int currentCylinder = 0;
  unsigned int currentSphere = 0;
  // First finger
  initialiseFinger(
      FINGER_1,
      spheres, 
      cylinders, 
      glm::vec3(0.0, 4.0, 0.0), 
      tile, 
      &currentSphere, 
      &currentCylinder, 
      1.0f,
      2.0f,
      params);
  // Second finger
  initialiseFinger(
      FINGER_2,
      spheres, 
      cylinders, 
      glm::vec3(1.1, 4.0, 0.0), 
      tile, 
      &currentSphere, 
      &currentCylinder, 
      1.0f,
      2.4f,
      params);
  // Middle finger
  initialiseFinger(
      FINGER_3,
      spheres, 
      cylinders, 
      glm::vec3(2.2, 4.0, 0.0), 
      tile, 
      &currentSphere, 
      &currentCylinder, 
      1.0f,
      2.0f,
      params);
   // Little finger
   initialiseFinger(
      FINGER_4,
      spheres, 
      cylinders, 
      glm::vec3(3.2, 4.0, 0.0), 
      tile, 
      &currentSphere, 
      &currentCylinder, 
      0.8f,
      1.5f,
      params);

  // Where fingers meet the palm
  glm::mat4 s, t, r;
  s = glm::scale(glm::mat4(1.0f), glm::vec3(4.5f, 0.5f, 1.5f));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 4.0f, 0.0f));
  spheres[tile * HAND_SPHERES + currentSphere++] = p.getTileTrans(tile) * t * s;

  // The palm
  s = glm::scale(glm::mat4(1.0f), glm::vec3(4.5f, 4.0f, 1.5f));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
  cylinders[tile * HAND_CYLINDERS + currentCylinder++] = p.getTileTrans(tile) * t * s;

  // The bottom of the palm
  s = glm::scale(glm::mat4(1.0f), glm::vec3(4.5f, 0.5f, 1.5f));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
  spheres[tile * HAND_SPHERES + currentSphere++] = p.getTileTrans(tile) * t * s;

  // The thumb
  initialiseThumb(
      spheres, 
      cylinders, 
      glm::vec3(3.2, 4.0, 0.0), 
      tile, 
      &currentSphere, 
      &currentCylinder, 
      1.2f,
      1.4f,
      params);

}
