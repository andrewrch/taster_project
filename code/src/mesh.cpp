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
*/

#include <assert.h>
#include <cmath>
#include "mesh.hpp"

using namespace std;

#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define WVP_LOCATION 2
#define WORLD_LOCATION 6

static const int NUMBER_OF_PRIMITIVES = 2;

Mesh::Mesh() :
  VAO(0),
  entries(NUMBER_OF_PRIMITIVES)
{
    ZERO_MEM(buffers);
}

Mesh::~Mesh()
{
    clear();
}


void Mesh::clear()
{
    if (buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(buffers), buffers);
    }
       
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

bool Mesh::init(
    double sphereRadius,
    unsigned int sphereRings,
    unsigned int sphereSectors,
    double cylinderRadius,
    double cylinderHeight,
    unsigned int cylinderSides)
{
    // Release the previously loaded mesh (if it exists)
    clear();
 
    // Create the VAO
    glGenVertexArrays(1, &VAO);   
    glBindVertexArray(VAO);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(buffers), buffers);
    // Create entries for all primitives used for modelling
    entries.resize(NUMBER_OF_PRIMITIVES);
    // Vectors to hold all data about primitives
    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<unsigned int> indices;

    // Init sphere first
    unsigned int numIndices = (sphereRings - 1) * sphereSectors * 6;
    unsigned int numVertices = sphereRings * sphereSectors;

    entries[0].baseIndex = 0;
    entries[0].baseVertex = 0;
    entries[0].numIndices = numIndices;

    // Reserve space in the vectors for the sphere
    //positions.reserve(numVertices);
    //normals.reserve(numVertices);
    //indices.reserve(numIndices);

    initSphere(
        sphereRadius, 
        sphereRings, 
        sphereSectors,
        positions,
        normals,
        indices);

    entries[1].baseIndex = numIndices;
    entries[1].baseVertex = numVertices;

    numIndices = 6 * (cylinderSides - 1);
    numVertices = 2 * (cylinderSides - 1);
    entries[1].numIndices = numIndices;

    // Reserve space in the vectors for the cylinder
    //positions.reserve(numVertices);
    //normals.reserve(numVertices);
    //indices.reserve(numIndices);

    initCylinder(
        cylinderRadius,
        cylinderHeight,
        cylinderSides,
        positions,
        normals,
        indices);
    
        //positions[entries[1].baseVertex + 5] = glm::vec3(0, 0, 0); 

    //for (int i = entries[1].baseVertex ; i < positions.size() + 1; i++)
    //  printf("%f %f %f\n", positions[i].x, positions[i].y, positions[i].z);

    // Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VB]);
    glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(positions[0]) * positions.size(), 
        &positions[0], 
        GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

   	glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_VB]);
  	glBufferData(
        GL_ARRAY_BUFFER, 
        sizeof(normals[0]) * normals.size(), 
        &normals[0], 
        GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
  	glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, 
        sizeof(indices[0]) * indices.size(), 
        &indices[0], 
        GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[WVP_MAT_VB]);
    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(WVP_LOCATION + i);
        glVertexAttribPointer(
            WVP_LOCATION + i, 
            4, 
            GL_FLOAT, 
            GL_FALSE, 
            sizeof(glm::mat4), 
            (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(WVP_LOCATION + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffers[WORLD_MAT_VB]);
    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(WORLD_LOCATION + i);
        glVertexAttribPointer(
            WORLD_LOCATION + i, 
            4, 
            GL_FLOAT, 
            GL_FALSE, 
            sizeof(glm::mat4), 
            (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(WORLD_LOCATION + i, 1);
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);	
    return GLCheckError();
}


/* Code here borrowed and modified from
 * http://stackoverflow.com/questions/7957254/connecting-sphere-vertices-opengl
 */
void Mesh::initSphere(
        double radius,
        unsigned int rings,
        unsigned int sectors,
        std::vector<glm::vec3>& positions,
        std::vector<glm::vec3>& normals,
        std::vector<unsigned int>& indices)
{
  float const R = 1.0f/(float)(rings-1);
  float const S = 1.0f/(float)(sectors-1);
  unsigned int r, s;

  for(r = 0; r < rings; r++) 
    for(s = 0; s < sectors; s++) 
    {
      float const y = sin( -M_PI_2 + M_PI * r * R );
      float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
      float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

      positions.push_back(glm::vec3(
            x * radius,
            y * radius,
            z * radius));

      normals.push_back(glm::vec3(x, y, z));
    }

  for(r = 0; r < rings-1; r++) 
    for(s = 0; s < sectors; s++) 
  {
    indices.push_back(r * sectors + s);
    indices.push_back(r * sectors + (s+1));
    indices.push_back((r+1) * sectors + s);
    indices.push_back(r * sectors + (s+1));
    indices.push_back((r+1) * sectors + (s+1));
    indices.push_back((r+1) * sectors + s);
  }
}

/* Code modified from some at:
 * http://stackoverflow.com/questions/4170603/how-do-i-draw-a-cylinder-in-opentk-glu-cylinder
 */
void Mesh::initCylinder(
        double radius,
        double height,
        unsigned int sides,
        std::vector<glm::vec3>& positions,
        std::vector<glm::vec3>& normals,
        std::vector<unsigned int>& indices)
{
  for (unsigned int y = 0; y < 2; y++)
  {
    for (unsigned int x = 0; x < sides; x++)  
    {
        double theta = ((double) x / (sides - 1)) * 2 * M_PI;
        glm::vec3 p(radius * cos(theta),  height * (y - height/2), radius * sin(theta));
        positions.push_back(p);
        normals.push_back(glm::vec3(p.x / radius, p.y / radius, 0));
    }
  }

  for (unsigned int x = 0; x < sides - 1; x++)
  {
    indices.push_back(x);
    indices.push_back(x + 1);
    indices.push_back(x + sides);
    
    indices.push_back(x + sides);
    indices.push_back(x + 1);
    indices.push_back(x + sides + 1);
  }
}

void Mesh::renderSpheres(
    unsigned int numSpheres, 
    const glm::mat4* sphereWVPs, 
    const glm::mat4* sphereWorlds)
{        
    glBindBuffer(GL_ARRAY_BUFFER, buffers[WVP_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numSpheres, sphereWVPs, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[WORLD_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numSpheres, sphereWorlds, GL_DYNAMIC_DRAW);

    glBindVertexArray(VAO);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, 
                                      entries[0].numIndices, 
                                      GL_UNSIGNED_INT, 
                                      (void*)(sizeof(unsigned int) * entries[0].baseIndex), 
                                      numSpheres,
                                      entries[0].baseVertex);

    glBindVertexArray(0);
}

void Mesh::renderCylinders(
    unsigned int numCylinders, 
    const glm::mat4* cylinderWVPs,
    const glm::mat4* cylinderWorlds)
{        
    glBindBuffer(GL_ARRAY_BUFFER, buffers[WVP_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCylinders, cylinderWVPs, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[WORLD_MAT_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numCylinders, cylinderWorlds, GL_DYNAMIC_DRAW);

    glBindVertexArray(VAO);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
                                      entries[1].numIndices, 
                                      GL_UNSIGNED_INT, 
                                      (void*)(sizeof(unsigned int) * entries[1].baseIndex), 
                                      numCylinders,
                                      entries[1].baseVertex);
    glBindVertexArray(0);
}
