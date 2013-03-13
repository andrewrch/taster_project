/*

	Copyright 2011 Etay Meiri

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

    This has been modified fairly heavily by Andrew Chambers to apply
    to a CDT project.
*/

#ifndef MESH_H
#define	MESH_H

#include <map>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "util.h"

class Mesh
{
  public:
    Mesh();

    ~Mesh();

    bool init(
        double,         // Sphere radius 
        unsigned int,   // Sphere rings
        unsigned int,   // Sphere sectors
        double,         // Cylinder radius
        double,         // Cylinder height
        unsigned int);  // Cylinder sides

    // For convenience put in a single call?
    void renderSpheres(
        unsigned int,
        const glm::mat4*);

    void renderCylinders(
        unsigned int,
        const glm::mat4*);
        

  private:
    // Methods to initialise the primitives used for rendering
    void initSphere(
        double,                       // Sphere radius 
        unsigned int,                 // Sphere rings
        unsigned int,                 // Sphere sectors
        std::vector<glm::vec3>&,      // Positions
        std::vector<glm::vec3>&,      // Normals
        std::vector<unsigned int>&);  // Indices

    void initCylinder(
        double,                       // Cylinder radius
        double,                       // Cylinder height
        unsigned int,                 // Cylinder sides
        std::vector<glm::vec3>&,      // Positions
        std::vector<glm::vec3>&,      // Normals
        std::vector<unsigned int>&);  // Indices

    // Clear buffers and VAO
    // FIXME Might only need for destructor so consider
    // moving
    void clear();

#define INDEX_BUFFER 0    
#define POS_VB       1
#define NORMAL_VB    2
#define WVP_MAT_VB   3
//#define WORLD_MAT_VB 4

    GLuint VAO;
    GLuint axes;
    GLuint buffers[4];

    // We hold both primitives in same vertex
    // array and use BaseVertex call so require
    // information about each mesh
    struct MeshEntry {
        MeshEntry()
        {
            numIndices = 0;
            baseVertex = 0;
            baseIndex = 0;
        }
        
        unsigned int numIndices;
	      unsigned int baseVertex;
        unsigned int baseIndex;
    };
    
    std::vector<MeshEntry> entries;
};


#endif	/* MESH_H */

