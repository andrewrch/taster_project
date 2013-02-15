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

  //for (int i = 0; i < 4; i++)
  //  printf("%f %f %f %f\n", VPTransformation[i][0], VPTransformation[i][1], VPTransformation[i][2], VPTransformation[i][3]);
  //      
  //      printf("\n");
  return VPTransformation;
}

const glm::mat4& Pipeline::getWorldTrans()
{
  //glm::mat4 scaleTrans, rotateTrans, translationTrans;

  //scaleTrans = glm::scale(glm::mat4(1.0f), scale);
  //rotateTrans = glm::rotate(glm::mat4(1.0f), 1.0f, rotateInfo);
  //translationTrans = glm::translate(glm::mat4(1.0f), worldPos);

  worldTransformation = glm::mat4(1.0f);//translationTrans * rotateTrans * scaleTrans;

  //      for (int i = 0; i < 4; i++)
  //        printf("%f %f %f %f\n", worldTransformation[i][0], worldTransformation[i][1], worldTransformation[i][2], worldTransformation[i][3]);
  //      
  //      printf("\n");
  return worldTransformation;
}

const glm::mat4& Pipeline::getWVPTrans()
{
  getWorldTrans();
  getVPTrans();

  WVPtransformation = VPTransformation * worldTransformation;
  return WVPtransformation;
}


