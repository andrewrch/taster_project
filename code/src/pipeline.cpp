/*

	Copyright 2010 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <assert.h>

#include "pipeline.hpp"

const glm::mat4& Pipeline::getVPTrans()
{
  glm::mat4 cameraTrans, projTrans;

  cameraTrans = glm::lookAt(camera.pos, camera.target, camera.up);
  projTrans = glm::perspective(
      perspectiveProj.fovy, 
      perspectiveProj.aspect, 
      perspectiveProj.zNear,
      perspectiveProj.zFar);
    
  VPTransformation = projTrans * cameraTrans;

  return VPTransformation;
}

const glm::mat4& Pipeline::getWorldTrans()
{
  glm::mat4 scaleTrans, rotateTrans, translationTrans;

  // Can probably make this more efficient with some reading.
  scaleTrans = glm::scale(scaleTrans, scale);
//  rotateTrans = glm::rotate(rotateTrans, 45.0f, rotateInfo);
  rotateTrans = glm::toMat4(orientation);
  translationTrans = glm::translate(translationTrans, worldPos);

  glm::mat4 cone(1.0f);
  cone[0][0] = 0.7f;
  cone[1][1] = 0.7f;
  cone[2][2] = 1.0f;
  cone[3][3] = 0.7f;
  cone[3][2] = 0.3f;

  //for (int i = 0; i < 4; i++)
  //  printf("%f %f %f %f\n", cone[i][0], cone[i][1], cone[i][2], cone[i][3]);
  //printf("\n");

  worldTransformation = translationTrans * rotateTrans * scaleTrans * cone;
  return worldTransformation;
}

const glm::mat4& Pipeline::getWVPTrans()
{
  getWorldTrans();
  getVPTrans();

  WVPTransformation = VPTransformation * worldTransformation;
  return WVPTransformation;
}

const glm::mat4& Pipeline::getTileTrans(unsigned int i)
{
  //assert(i < numTiles);

  getWVPTrans();
  
  glm::mat4 scaleTrans, translateTrans;
  // Tiles are row major
  int rows, cols;
  rows = cols = floor(sqrt(numTiles));
  float scale = 2.0f / rows;
  scaleTrans = glm::scale(scaleTrans, glm::vec3(scale, scale, scale));

  float xTrans = -1.0f + (scale * (i % cols)) + scale/2;
  float yTrans = -1.0f + (scale * (i / rows)) + scale/2;
  float zTrans = 0;
  translateTrans = glm::translate(translateTrans, glm::vec3(xTrans, yTrans, zTrans));

  tileTransformation = translateTrans * scaleTrans * VPTransformation;
  return tileTransformation;
}


