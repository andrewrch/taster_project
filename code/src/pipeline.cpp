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

const glm::mat4& Pipeline::getTileTrans(unsigned int i)
{
  getVPTrans();
  
  glm::mat4 scaleTrans, translateTrans;
  // Tiles are row major
  int x = sqrt(numTiles);
  float scale = 1.0 / x;
  scaleTrans = glm::scale(scaleTrans, glm::vec3(scale, scale, 1.0f));
  float xTrans = -1.0f + (2.0 * scale * (i % x)) + scale;
  float yTrans = -1.0f + (2.0 * scale * (i / x)) + scale;
  float zTrans = -0.0f;
  translateTrans = glm::translate(translateTrans, glm::vec3(xTrans, yTrans, zTrans));

  tileTransformation = translateTrans * scaleTrans * VPTransformation;
  return tileTransformation;
}


