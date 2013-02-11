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

GLuint VBO;
GLuint IBO;
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

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
  glDisableVertexAttribArray(0);

  glutSwapBuffers();
}

static void keyboard(unsigned char Key, int x, int y)
{
  switch (Key) {
    case 'q':
      exit(0);
  }
}

static void initializeGlutCallbacks()
{
  glutDisplayFunc(render);
  glutIdleFunc(render);
  glutKeyboardFunc(keyboard);
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
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Drawing");

  initializeGlutCallbacks();

  // Must be done after glut is initialized!
  GLenum res = glewInit();
  if (res != GLEW_OK) {
   fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
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
  //gWVPLocation = shader("gWVP");
  shader.use();

  glutMainLoop();

  return 0;
}
