/*

	Copyright 2010 Etay Meiri

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <GL/freeglut.h>

#include "camera.hpp"

Camera::Camera(
  int windowWidth, 
  int windowHeight)
{
  this->windowWidth = windowWidth;
  this->windowHeight = windowHeight;
  this->pos = glm::vec3(0.0f, 0.0f, 0.0f);
  this->target = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
  this->up = glm::vec3(0.0f, 1.0f, 0.0f);

  init();
}


Camera::Camera(
  int windowWidth, 
  int windowHeight, 
  const glm::vec3& pos, 
  const glm::vec3& target, 
  const glm::vec3& up)
{
  this->windowWidth = windowWidth;
  this->windowHeight = windowHeight;
  this->pos = pos;
  this->target = glm::normalize(target);
  this->up = glm::normalize(up);
  init();
}


void Camera::init()
{
  glm::vec3 hTarget = glm::normalize(glm::vec3(target.x, 0.0, target.z));
  //HTarget.Normalize();
  
  if (hTarget.z >= 0.0f)
  {
    if (hTarget.x >= 0.0f)
      angleH = 360.0f - glm::degrees(asin(hTarget.z));
    else
      angleH = 180.0f + glm::degrees(asin(hTarget.z));
  }
  else
  {
    if (hTarget.x >= 0.0f)
    {
      angleH = glm::degrees(asin(-hTarget.z));
    }
    else
    {
      angleH = 90.0f + glm::degrees(asin(-hTarget.z));
    }
  }
  
  angleV = -glm::degrees(asin(target.y));
}

void Camera::update()
{
  //const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

  //// Rotate the view vector by the horizontal angle around the vertical axis
  //Vector3f View(1.0f, 0.0f, 0.0f);
  //View.Rotate(m_AngleH, Vaxis);
  //View.Normalize();

  //// Rotate the view vector by the vertical angle around the horizontal axis
  //Vector3f Haxis = Vaxis.Cross(View);
  //Haxis.Normalize();
  //View.Rotate(m_AngleV, Haxis);
  //  
  //m_target = View;
  //m_target.Normalize();

  //m_up = m_target.Cross(Haxis);
  //m_up.Normalize();
}
