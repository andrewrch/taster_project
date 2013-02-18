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

#ifndef PIPELINE_H
#define	PIPELINE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Pipeline
{
  public:
    Pipeline(unsigned int t = 4)
    {
      numTiles = t;     
      scale      = glm::vec3(1.0f, 1.0f, 1.0f);
      worldPos   = glm::vec3(0.0f, 0.0f, 0.0f);
      rotateInfo = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void setScale(float scaleX, float scaleY, float scaleZ)
    {
      scale.x = scaleX;
      scale.y = scaleY;
      scale.z = scaleZ;
    }

    void setWorldPos(float x, float y, float z)
    {
      worldPos.x = x;
      worldPos.y = y;
      worldPos.z = z;
    }
    
    void setWorldPos(const glm::vec3& pos)
    {
      worldPos = pos;
    }

    void setRotate(float rotateX, float rotateY, float rotateZ)
    {
      rotateInfo.x = rotateX;
      rotateInfo.y = rotateY;
      rotateInfo.z = rotateZ;
    }

    void setPerspectiveProj(float fovy, float aspect, float zNear, float zFar)
    {
        perspectiveProj.fovy = fovy;
        perspectiveProj.aspect = aspect;
        perspectiveProj.zNear = zNear;
        perspectiveProj.zFar = zFar;
    }

    void setCamera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
    {
        camera.pos = pos;
        camera.target = target;
        camera.up = up;
    }

    const glm::mat4& getVPTrans();
    const glm::mat4& getWVPTrans();
    const glm::mat4& getWorldTrans();
    const glm::mat4& getTileTrans(unsigned int);

  private:
    unsigned int numTiles;

    glm::vec3 scale;
    glm::vec3 worldPos;
    glm::vec3 rotateInfo;

    struct {
      float fovy;
      float aspect;
      float zNear;
      float zFar;
    } perspectiveProj;

    struct {
      glm::vec3 pos;
      glm::vec3 target;
      glm::vec3 up;
    } camera;

    glm::mat4 WVPtransformation;
    glm::mat4 VPTransformation;
    glm::mat4 worldTransformation;
    glm::mat4 tileTransformation;
};

#endif	/* PIPELINE_H */

