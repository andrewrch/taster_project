#ifndef CONE_H
#define CONE_H

#include <glm/glm.hpp>

glm::mat4 cone(float ratio)
{
  glm::mat4 c(1.0f);

  c[0][0] = ratio;
  c[1][1] = 1.0f;
  c[2][2] = ratio;
  c[3][3] = ratio;
  c[1][3] = 1.0f - ratio;

  return c;
}

#endif
