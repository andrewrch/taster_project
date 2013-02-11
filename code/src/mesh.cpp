#include "mesh.hpp"

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

void Mesh::render(unsigned int numInstances, const glm::mat4* WVPMats)
{
  glBindBuffer(GL_ARRAY_BUFFER, buffers[WVP_MAT_VB]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * numInstances, WVPMats, GL_DYNAMIC_DRAW);

  glBindVertexArray(VAO);
    
  glDrawElementsInstanced(GL_TRIANGLES, 
                          numIndices, 
                          GL_UNSIGNED_INT,
                          NULL,
//                          (void*)(sizeof(unsigned int) * baseIndex), 
                          numInstances);

  glBindVertexArray(0);
}
