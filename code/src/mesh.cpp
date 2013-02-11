#include "mesh.hpp"

// For absolute position for the shader
#define POSITION_LOCATION 0
#define NORMAL_LOCATION 1
#define WVP_LOCATION 2
#define WORLD_LOCATION 6

using namespace std;

Mesh::Mesh() 
{
  // Create the VAO
  glGenVertexArrays(1, &VAO);   
  glBindVertexArray(VAO);
  // Create the buffers for the vertices attributes
  glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(buffers), buffers);
  glBindVertexArray(0);
}

Mesh::~Mesh() 
{
  if (buffers[0] != 0) 
    glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(buffers), buffers);
       
  if (VAO != 0) 
    glDeleteVertexArrays(1, &VAO);
}

bool Mesh::setupArrays(
    vector<glm::vec3>& positions,
    vector<glm::vec3>& normals,
    vector<unsigned int>& indices)
{
  // Generate and populate the buffers with vertex attributes and the indices
  glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VB]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(POSITION_LOCATION);
  glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

  glBindBuffer(GL_ARRAY_BUFFER, buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(NORMAL_LOCATION);
  glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[WVP_MAT_VB]);
    
  for (unsigned int i = 0; i < 4 ; i++) 
  {
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

 // glBindBuffer(GL_ARRAY_BUFFER, buffers[WORLD_MAT_VB]);
 // for (unsigned int i = 0; i < 4 ; i++) 
 // {
 //   glEnableVertexAttribArray(WORLD_LOCATION + i);
 //   glVertexAttribPointer(
 //       WORLD_LOCATION + i, 
 //       4, 
 //       GL_FLOAT, 
 //       GL_FALSE, 
 //       sizeof(glm::mat4), 
 //       (const GLvoid*)(sizeof(GLfloat) * i * 4));
 //   glVertexAttribDivisor(WORLD_LOCATION + i, 1);
 // }    

  // Unbind
  glBindVertexArray(0);	
  return glGetError() == GL_NO_ERROR;
}

void Mesh::render(unsigned int numInstances, const glm::mat4* WVPMats)
{
  glBindBuffer(GL_ARRAY_BUFFER, buffers[WVP_MAT_VB]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numInstances, WVPMats, GL_DYNAMIC_DRAW);

  glBindVertexArray(VAO);

  glDrawElementsInstanced(GL_TRIANGLES, 
                          //numIndices, 
                          12, 
                          GL_UNSIGNED_INT,
                          NULL,
                          numInstances);

  glBindVertexArray(0);
}
