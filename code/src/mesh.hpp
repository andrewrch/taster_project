/* Author Andrew Chambers
 * Code heavily based on tutorials from http://ogldev.atspace.co.uk
 */

#ifndef MESH_H
#define MESH_H

#include <stdlib.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define INDEX_BUFFER 0
#define POS_VB 1
// Ignore normals for now...
#define NORMALS_VB 2
#define WVP_MAT_VB 3

// Replace this at some point
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

class Mesh
{
  public:
    // Always builds a regular "unit" shape around the origin
    Mesh(void);
    ~Mesh(void);
    void render(unsigned int numInstances, const glm::mat4* WVPMats);

  private:
    GLuint VAO;
    // indexes, pos, normals, WVP
    GLuint buffers[4];

    unsigned int numIndices;
};

#endif
