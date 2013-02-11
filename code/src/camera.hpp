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

#ifndef CAMERA_H
#define	CAMERA_H

#include <glm/glm.hpp>

class Camera
{
public:

    Camera(int, int);

    Camera(int, int, const glm::vec3&, const glm::vec3&, const glm::vec3&);

    void onRender();

    const glm::vec3& getPos() const
    {
        return pos;
    }

    const glm::vec3& getTarget() const
    {
        return target;
    }

    const glm::vec3& getUp() const
    {
        return up;
    }

private:

    void init();
    void update();

    glm::vec3 pos;
    glm::vec3 target;
    glm::vec3 up;

    int windowWidth;
    int windowHeight;

    float angleH;
    float angleV;
};

#endif	/* CAMERA_H */

