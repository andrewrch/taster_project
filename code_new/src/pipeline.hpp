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
#include <glm/gtx/transform2.hpp>
#include <vector>
#include <iostream>

class Pipeline
{
  public:
    Pipeline(unsigned int w, unsigned int h, unsigned int t = 4) :
      width(w),
      height(h),
      numTiles(t),
      tileTransformations(t)
    {
      updateTransformations();
    }

    void setPerspectiveProj(float fovy, float aspect, float zNear, float zFar)
    {
      proj.fovy = fovy;
      proj.aspect = aspect;
      proj.zNear = zNear;
      proj.zFar = zFar;
      updateTransformations();
    }

    inline void setCamera(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
    {
      camera.pos = pos;
      camera.target = target;
      camera.up = up;
      updateTransformations();
    }

    inline void setTiles(unsigned int tiles) { 
      numTiles = tiles; 
      tileTransformations = std::vector<glm::mat4>(tiles);
      updateTransformations();
    }

    inline const glm::mat4& getVTrans() { return VTransformation; }
    inline const glm::mat4& getVPTrans() { return VPTransformation; }
    inline const glm::mat4& getTileTrans(unsigned int i) { return tileTransformations[i]; }

  private:
    unsigned int width;
    unsigned int height;
    unsigned int numTiles;

    struct {
      float fovy;
      float aspect;
      float zNear;
      float zFar;
    } proj;

    struct {
      glm::vec3 pos;
      glm::vec3 target;
      glm::vec3 up;
    } camera;

    glm::mat4 VPTransformation;
    glm::mat4 VTransformation;
    std::vector<glm::mat4> tileTransformations;

    void updateTransformations()
    {
      // View
      VTransformation = glm::lookAt(camera.pos, camera.target, camera.up);
      // View projection
      glm::mat4 projTrans = glm::perspective(proj.fovy, proj.aspect, proj.zNear, proj.zFar);
      VPTransformation = projTrans * VTransformation;

      // Tile transformation
      glm::mat4 scaleTrans, translateTrans;
      int x = sqrt(numTiles);
      double scale = 1.0 / x;
      scaleTrans = glm::scale(scaleTrans, glm::vec3(scale, scale, 1.0f));
      double xTrans, yTrans, zTrans;
      for (unsigned int i = 0; i < numTiles; i++)
      {
        xTrans = -1.0f + (2.0 * scale * (i % x)) + scale;
        yTrans = -1.0f + (2.0 * scale * (i / x)) + scale;
        zTrans = 0.0f;
        translateTrans = glm::translate(glm::mat4(1.0f), glm::vec3(xTrans, yTrans, zTrans));
        tileTransformations[i] = translateTrans * scaleTrans * VPTransformation;
      }
    }
};

#endif	/* PIPELINE_H */

