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

glm::mat4 Hand::initialiseJoint(
    unsigned int finger,
    unsigned int joint,
    glm::mat4* spheres, 
    glm::mat4* cylinders, 
    glm::mat4* startTR,
    unsigned int tile,
    unsigned int* currentSphere,
    unsigned int* currentCylinder,
    float startRadius,
    float startLength,
    float params[NUM_PARAMETERS])



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
  glm::mat4 globalPos, tileProj, globalRot, s, t, r, c, tr;
  glm::quat orientation, globalOrientation;
  float coneRatio, coneLength;

  globalPos = glm::translate(glm::mat4(1.0f), 
      glm::vec3(params[GLOBAL_POS_X], params[GLOBAL_POS_Y], params[GLOBAL_POS_Z]));
  globalOrientation = glm::quat(
      params[GLOBAL_ROT_X], params[GLOBAL_ROT_Y], params[GLOBAL_ROT_Z], params[GLOBAL_ROT_W]);
  tileProj = p.getTileProj(tile);
  
  // SPHERE
  s = glm::scale(s, glm::vec3(startRadius, startRadius, startRadius));
  t = glm::translate(t, startPosition);
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = tileProj * t * s;

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
  cylinders[tile * HAND_CYLINDERS + (*currentCylinder)++] = tileProj * tr * s * c;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * startRadius, 
        coneRatio * startRadius, 
        coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength, 0.0f));
  tr = tr * t;
  temp = tr * s;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = tileProj * temp;

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
  temp = tr * s * c;
  cylinders[tile * HAND_CYLINDERS + (*currentCylinder)++] = tileProj * temp;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * startRadius, 
        coneRatio * startRadius, 
        coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength, 0.0f));
  tr = tr * t;
  temp = tr * s;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = tileProj * temp;

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
  temp = tr * s * c;
  cylinders[tile * HAND_CYLINDERS + (*currentCylinder)++] = tileProj * temp;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * coneRatio * startRadius, 
        coneRatio * coneRatio * startRadius, 
        coneRatio * coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength, 0.0f));
  tr = tr * t;
  temp = tr * s;
  spheres[tile * HAND_SPHERES + (*currentSphere)++] = tileProj * temp;
}

void Hand::initialiseThumb(
    glm::mat4* spheresTiled, 
    glm::mat4* spheres, 
    glm::mat4* cylindersTiled, 
    glm::mat4* cylinders, 
    glm::vec3 startPosition,
    unsigned int tile,
    unsigned int* currentSphere,
    unsigned int* currentCylinder,
    float startRadius,
    float startLength,
    float params[])
{
  glm::mat4 r, s, it, t, tr, c, temp;

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
  temp = tr * s * it;
  spheres[tile * HAND_SPHERES + *currentSphere] = p.getVPTrans() * temp;
  spheresTiled[tile * HAND_SPHERES + (*currentSphere)++] = tileProj * temp;

  // Sphere
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.5f, 0.0f));
  s = glm::scale(glm::mat4(1.0f), glm::vec3(startRadius, startRadius, startRadius));
  tr = tr * t;
  temp = tr * s;
  spheres[tile * HAND_SPHERES + *currentSphere] = p.getVPTrans() * temp;
  spheresTiled[tile * HAND_SPHERES + (*currentSphere)++] = tileProj * temp;

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
  temp = tr * s * c;
  cylinders[tile * HAND_CYLINDERS + *currentCylinder] = p.getVPTrans() * temp;
  cylindersTiled[tile * HAND_CYLINDERS + (*currentCylinder)++] = tileProj * temp;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * startRadius, 
        coneRatio * startRadius, 
        coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength, 0.0f));
  tr = tr * t;
  temp = tr * s;
  spheres[tile * HAND_SPHERES + *currentSphere] = p.getVPTrans() * temp;
  spheresTiled[tile * HAND_SPHERES + (*currentSphere)++] = tileProj * temp;

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
  temp = tr * s * c;
  cylinders[tile * HAND_CYLINDERS + *currentCylinder] = p.getVPTrans() * temp;
  cylindersTiled[tile * HAND_CYLINDERS + (*currentCylinder)++] = tileProj * temp;

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * startRadius, 
        coneRatio * startRadius, 
        coneRatio * startRadius));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, coneLength - 0.2, 0.0f));
  tr = tr * t;
  temp = tr * s;
  spheres[tile * HAND_SPHERES + *currentSphere] = p.getVPTrans() * temp;
  spheresTiled[tile * HAND_SPHERES + (*currentSphere)++] = tileProj * temp;
}

void Hand::initialiseHand(
    glm::mat4* spheresTiled, 
    glm::mat4* spheres, 
    glm::mat4* cylindersTiled, 
    glm::mat4* cylinders, 
    unsigned int tile,
    float params[])
{
  unsigned int currentCylinder = 0;
  unsigned int currentSphere = 0;
  // First finger
  initialiseFinger(
      FINGER_1,
      spheresTiled, 
      spheres, 
      cylindersTiled, 
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
      spheresTiled, 
      spheres, 
      cylindersTiled, 
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
      spheresTiled, 
      spheres, 
      cylindersTiled, 
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
      spheresTiled, 
      spheres, 
      cylindersTiled, 
      cylinders, 
      glm::vec3(3.2, 4.0, 0.0), 
      tile, 
      &currentSphere, 
      &currentCylinder, 
      0.8f,
      1.5f,
      params);

  // Where fingers meet the palm
  glm::mat4 s, t, r, temp;
  s = glm::scale(glm::mat4(1.0f), glm::vec3(4.5f, 0.5f, 1.5f));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 4.0f, 0.0f));
  temp = t * s;
  spheres[tile * HAND_SPHERES + currentSphere] = p.getVPTrans() * temp;
  spheresTiled[tile * HAND_SPHERES + currentSphere++] = tileProj * temp;

  // The palm
  s = glm::scale(glm::mat4(1.0f), glm::vec3(4.5f, 4.0f, 1.5f));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
  temp = t * s;
  cylinders[tile * HAND_CYLINDERS + currentCylinder] = p.getVPTrans() * temp;
  cylindersTiled[tile * HAND_CYLINDERS + currentCylinder++] = tileProj * temp;

  // The bottom of the palm
  s = glm::scale(glm::mat4(1.0f), glm::vec3(4.5f, 0.5f, 1.5f));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f));
  temp = t * s;
  spheres[tile * HAND_SPHERES + currentSphere] = p.getVPTrans() * temp;
  spheresTiled[tile * HAND_SPHERES + currentSphere++] = tileProj * temp;

  // The thumb
  initialiseThumb(
      spheresTiled, 
      spheres, 
      cylindersTiled, 
      cylinders, 
      glm::vec3(3.2, 4.0, 0.0), 
      tile, 
      &currentSphere, 
      &currentCylinder, 
      1.2f,
      1.4f,
      params);

}
