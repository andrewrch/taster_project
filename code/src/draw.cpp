#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <vector>

void init(void)
{
  glClearColor(0,0,0,0);
  glShadeModel(GL_FLAT);
}

void DrawCube(void)
{
  glBegin(GL_POLYGON);
  //face in xy plane
  glColor3f(0.82, 0.41, 0.12);//this the color with which complete cube is drawn. 
  glVertex3f(0,0 ,0 );
  glVertex3f(5, 0, 0);
  glVertex3f(5, 5, 0);
  glVertex3f(0, 5, 0);

  //face in yz plane
  glColor3f(1, 0, 0);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 5);
  glVertex3f(0, 5, 0);
  glVertex3f(0, 5, 5);

  //face in zx plance
  glColor3f(0, 1, 0);
  glVertex3f(0, 0, 0  );
  glVertex3f(0, 0, 5);
  glVertex3f(5, 0, 5);
  glVertex3f(5, 0, 0);

  //|| to xy plane.
  glColor3f(0, 0, 1);
  glVertex3f(0, 0, 5);
  glVertex3f(5, 0, 5);
  glVertex3f(5, 5, 5);
  glVertex3f(0, 5, 5);

  //|| to yz plane
  glColor3f(0.73, 0.58, 0.58);
  glVertex3f(0,0 ,5 );
  glVertex3f(5, 0, 5);
  glVertex3f(5, 5, 5);
  glVertex3f(0, 5, 5);

  //|| to zx plane
  glVertex3f(0.58, 0, 0.82);
  glVertex3f(0, 5, 0  );
  glVertex3f(0, 5, 5);
  glVertex3f(5, 5, 5);
  glVertex3f(5, 5, 0);
}


void reshape(int w, int h){
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1, 1, -1, 1, 1.5, 20);
  glMatrixMode(GL_MODELVIEW);
}

void drawBall()
{
  std::vector<GLfloat> ballVerts;

  for(int i = 0; i <= 40; i++)
  {
      double lat0 = M_PI * (-0.5 + (double) (i - 1) / 40);
      double z0  = sin(lat0);
      double zr0 =  cos(lat0);

      double lat1 = M_PI * (-0.5 + (double) i / 40);
      double z1 = sin(lat1);
      double zr1 = cos(lat1);

      for(int j = 0; j <= 40; j++)
      {
          double lng = 2 * M_PI * (double) (j - 1) / 40;
          double x = cos(lng);
          double y = sin(lng);

          ballVerts.push_back(x * zr0); //X
          ballVerts.push_back(y * zr0); //Y
          ballVerts.push_back(z0);      //Z

          ballVerts.push_back(0.0f); 
          ballVerts.push_back(1.0f); 
          ballVerts.push_back(0.0f); 
          ballVerts.push_back(1.0f); //R,G,B,A

          ballVerts.push_back(x * zr1); //X
          ballVerts.push_back(y * zr1); //Y
          ballVerts.push_back(z1);      //Z

          ballVerts.push_back(0.0f); 
          ballVerts.push_back(1.0f); 
          ballVerts.push_back(0.0f); 
          ballVerts.push_back(1.0f); //R,G,B,A
      }
  }

  glGenBuffers(1, &ballVbo);
  glBindBuffer(GL_VERTEX_ARRAY, ballVbo);

  GLuint sphereSize = 3200*7*4; //3200 vertixes * 7 floats
  glBufferData(GL_VERTEX_ARRAY,sphereSize, &ballVerts, GL_STATIC_DRAW);
  /*
      Draw a ball
  */
  glBindBuffer(GL_VERTEX_ARRAY, ballVbo);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 7*4, 0);
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(4, GL_FLOAT, 7*4, (void*)(3*4));

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3200);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void display(void)
{
  glLoadIdentity();
  gluLookAt(10, 10, 10, 0, 0, 0, 0, 1, 0);
  drawBall();
  glEnd();
  glFlush();
}

int main(int argc, char** argv){
  glutInit(&argc, argv);//we initizlilze the glut. functions
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}
