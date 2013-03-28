#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "hand.hpp"
#include "cone.hpp"

const double Hand::diameters[] = {19.0, 21.0, 19.0, 16.0, 20.0};
const double Hand::lengths[] = {35.0, 40.0, 35.0, 25.0, 20.0};

Hand::Hand(double params[NUM_PARAMETERS]) :
  // Initialise vectors with 0 length for push_back
  sphereWVPs(0),
  cylinderWVPs(0)
{
  // Centre of palm is origin of shape - possibly move this
  // depending on performance of model in testing
  palmScale = glm::vec3(90.0f, 70.0f, 25.0f);
  // This is just for shifting the origin about
  palmPos = glm::vec3(0.0f, 0.0f, 0.0f);
  // First finger is
  finger1Pos = glm::vec3(-palmScale.x/2 + diameters[FINGER_1]/2, palmScale.y/2, 0.0f);
  finger2Pos = glm::vec3(-diameters[FINGER_2]/2, palmScale.y/2, 0.0f);
  finger3Pos = glm::vec3(diameters[FINGER_3]/2, palmScale.y/2, 0.0f);
  finger4Pos = glm::vec3(palmScale.x/2 - diameters[FINGER_4]/2, palmScale.y/2, 0.0f);
  thumbPos = glm::vec3(-palmScale.x/2 + diameters[THUMB]/2, -palmScale.y/2 + 0.5, 0.75f);

  // Going to use push_back so reserve
  // space for all matrices
//  sphereWVPs.reserve(NUM_SPHERES);
//  cylinderWVPs.reserve(NUM_CYLINDERS);

  initialiseHand(params);
}

Hand::~Hand()
{
}

void Hand::initialiseHand(double params[NUM_PARAMETERS])
{
  // Global position from params
  glm::mat4 pos = glm::translate(glm::mat4(1.0f),
      glm::vec3(params[GLOBAL_POS_X], params[GLOBAL_POS_Y], params[GLOBAL_POS_Z]));
  // Global rotation from params
//  glm::quat orientation(
//      params[GLOBAL_ROT_X],
//      params[GLOBAL_ROT_Y],
//      params[GLOBAL_ROT_Z],
//      params[GLOBAL_ROT_W]);
//
  glm::quat orientation(glm::vec3(params[GLOBAL_ROT_X], params[GLOBAL_ROT_Y], params[GLOBAL_ROT_Z]));

  // Always normalise before converting to transformation matrix
  orientation = glm::gtc::quaternion::normalize(orientation);
  glm::mat4 rot = glm::toMat4(orientation);

  // This is the global position and location to apply to all
  // primitives
  glm::mat4 g = pos * rot;
  glm::mat4 f1, f2, f3, f4, t, p;
  f1 = g * glm::translate(glm::mat4(1.0f), finger1Pos);
  f2 = g * glm::translate(glm::mat4(1.0f), finger2Pos);
  f3 = g * glm::translate(glm::mat4(1.0f), finger3Pos);
  f4 = g * glm::translate(glm::mat4(1.0f), finger4Pos);
  t  = g * glm::translate(glm::mat4(1.0f), thumbPos);
  p  = g * glm::translate(glm::mat4(1.0f), palmPos);
                
  // First make the fingers
  initialiseFinger(FINGER_1, f1, params); 
  initialiseFinger(FINGER_2, f2, params); 
  initialiseFinger(FINGER_3, f3, params); 
  initialiseFinger(FINGER_4, f4, params); 
  // Then the thumb
  initialiseThumb(t, params); 
  // And the palm, et voila
  initialisePalm(p);
}

glm::mat4 Hand::initialiseJoint(
    glm::mat4 start,
    double xRot,
    double zRot,
    double diameter,
    double length,
    double coneRatio)
{
  // Store current rotation and translation
  glm::mat4 current, c, r, s, t;

  // CYLINDER
  s = glm::scale(glm::mat4(1.0f), glm::vec3(diameter, length, diameter));
  // Shift because cylinder doesn't start at origin
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, length/2, 0.0f));

  // Basic constraints
  if (zRot < 0) zRot = 0;
  else if (zRot > M_PI/2) zRot = M_PI/2;
  if (xRot > M_PI/4) xRot = M_PI/4;
  else if (xRot < -M_PI/4) xRot = -M_PI/4;

  glm::quat o(glm::vec3(zRot, 0.0f, xRot));
  glm::gtc::quaternion::normalize(o);
  r = glm::toMat4(o);
  c = cone(coneRatio);
  current = start * r;
  cylinderWVPs.push_back(current * t * s * c);

  // SPHERE
  s = glm::scale(glm::mat4(1.0f), glm::vec3(
        coneRatio * diameter, 
        coneRatio * diameter, 
        coneRatio * diameter));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, length, 0.0f));
  current = current * t;
  sphereWVPs.push_back(current * s);

  return current;
}

void Hand::initialiseFinger(
    Digit finger,
    glm::mat4 start,
    double params[NUM_PARAMETERS])
{
  double coneRatio = 0.95;
  double lengthUpdate = 0.5;
  double diameter = diameters[finger];
  double length = lengths[finger];

  glm::mat4 s, t, r, c, current;
  // Where finger joins the palm
  s = glm::scale(s, glm::vec3(diameter, diameter, diameter));
  sphereWVPs.push_back(start * s);

  current = initialiseJoint(
      start, 
      params[getParamPos(finger, ROT_1X)],
      params[getParamPos(finger, ROT_1Z)],
      diameter,
      length,
      coneRatio);

  diameter *= coneRatio;
  length *= lengthUpdate;

  current = initialiseJoint(
      current, 
      0.0f,
      params[getParamPos(finger, ROT_2Z)],
      diameter,
      length,
      coneRatio);

  diameter *= coneRatio;
//  length *= lengthUpdate;

  current = initialiseJoint(
      current, 
      0.0f,
      params[getParamPos(finger, ROT_3Z)],
      diameter,
      length,
      coneRatio);
}

void Hand::initialisePalm(glm::mat4 start)
{
  // Where fingers meet the palm
  glm::mat4 s, t;
  s = glm::scale(glm::mat4(1.0f), glm::vec3(palmScale.x, palmScale.y / 4, palmScale.z));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, palmScale.y / 2, 0.0f));
  sphereWVPs.push_back(start * t * s);

  // The palm
  s = glm::scale(glm::mat4(1.0f), palmScale);
  t = glm::translate(glm::mat4(1.0f), palmPos);
  cylinderWVPs.push_back(start * t * s);

  // The bottom of the palm
  s = glm::scale(glm::mat4(1.0f), glm::vec3(palmScale.x, palmScale.y / 2, palmScale.z));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -palmScale.y / 2, 0.0f));
  sphereWVPs.push_back(start * t * s);
}

void Hand::initialiseThumb(
    glm::mat4 start,
    double params[])
{
  glm::mat4 r, s, it, t, current;

  double coneRatio = 0.95;
  double lengthUpdate = 0.66;
  double diameter = diameters[THUMB];
  double length = lengths[THUMB];
  // Ball of thumb
  s = glm::scale(glm::mat4(1.0f), glm::vec3(diameter * 1.5, palmScale.y, palmScale.z));
  t = glm::translate(glm::mat4(1.0f), glm::vec3(palmScale.x / 8, -0.5, palmScale.z / 4));
  // First translate sphere so it rotates about different axis.
  it = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f));
  glm::quat o(glm::vec3(
        params[getParamPos(THUMB, ROT_1X)] + M_PI/8,
        0.0f, 
        params[getParamPos(THUMB, ROT_1Z)] + M_PI/6));
  o = glm::gtc::quaternion::normalize(o);
  r = glm::toMat4(o);
  current = start * t * r;
  sphereWVPs.push_back(current * s * it);

  // Sphere where thumb joins the ball
  t = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, palmScale.y - diameter, 0.0f));
  s = glm::scale(glm::mat4(1.0f), glm::vec3(diameter, diameter, diameter));

  // Rotate so thumb movements are along the correct axis
  o = glm::quat(glm::vec3(0.0f, M_PI/2, 0.0f));
  o = glm::gtc::quaternion::normalize(o);
  r = glm::toMat4(o);

  current = current * t * r;
  sphereWVPs.push_back(current * s);

  current = initialiseJoint(
      current, 
      0.0,
      params[getParamPos(THUMB, ROT_2Z)],
      diameter,
      length,
      coneRatio);

  diameter *= coneRatio;
  //length *= lengthUpdate;

  current = initialiseJoint(
      current, 
      0.0, 
      params[getParamPos(THUMB, ROT_3Z)],
      diameter,
      length,
      coneRatio);
}

