#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"

#define WINDOW_WIDTH 1024 
#define WINDOW_HEIGHT 1024

GLuint gWVPLocation;

static void render()
{
  glClear(GL_COLOR_BUFFER_BIT);

  //p.Rotate(0.0f, Scale, 0.0f);
  //p.WorldPos(0.0f, 0.0f, 3.0f);
  //p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
  //p.SetPerspectiveProj(60.0f, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 1.0f, 100.0f);
  //
  glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*) &glm::perspective(60.0f, 1.0f, 1.0f, 100.0f)[0][0]);

//  glEnableVertexAttribArray(0);
//  glBindBuffer(GL_ARRAY_BUFFER, VBO);
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
//  glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
//  glDisableVertexAttribArray(0);
  
  glutSwapBuffers();
}

// Keyboard callbacks
static void keyboard(unsigned char Key, int x, int y)
{
  switch (Key) {
    case 'q':
      exit(0);
  }
}

static void createVertexBuffer()
{
  glm::vec3 Vertices[4];
  Vertices[0] = glm::vec3(-1.0f, -1.0f, 0.5773f);
  Vertices[1] = glm::vec3(0.0f, -1.0f, -1.15475);
  Vertices[2] = glm::vec3(1.0f, -1.0f, 0.5773f);
  Vertices[3] = glm::vec3(0.0f, 1.0f, 0.0f);

 	 glGenBuffers(1, &VBO);
	 glBindBuffer(GL_ARRAY_BUFFER, VBO);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void createIndexBuffer()
{
  unsigned int indices[] = { 0, 3, 1,
                1, 3, 2,
                2, 3, 0,
                0, 2, 1 };

  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

int main(int argc, char** argv)
{
  // Will eventually render to a framebuffer object
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Drawing stuff...");
  glutDisplayFunc(render);
  glutIdleFunc(render);
  glutKeyboardFunc(keyboard);

  // Must be done after glut is initialized!
  GLenum res = glewInit();
  if (res != GLEW_OK) {
   fprintf(stderr, " GLEW Error: '%s'\n", glewGetErrorString(res));
   return 1;
  }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  createVertexBuffer();
  createIndexBuffer();

  //CompileShaders();
  Shader shader;
  // Load shaders from file
  shader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/vs.glslv");
  shader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/fs.glslf");

  // Compile and then link the shaders
  shader.createAndLinkProgram();
  gWVPLocation = shader.addUniform("gWVP");
  printf("Uniform location: %d\n", gWVPLocation);
  //gWVPLocation = shader("gWVP");
  shader.use();

  glutMainLoop();

  shader.unUse();
  return 0;
}
