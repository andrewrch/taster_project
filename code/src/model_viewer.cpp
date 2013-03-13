#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "pipeline.hpp"
#include "hand.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "glut_backend.hpp"

static const unsigned int WINDOW_WIDTH = 1024;
static const unsigned int WINDOW_HEIGHT = 768;

class ModelViewer : public ICallbacks
{
  public:

    ModelViewer() :
      pipeline(WINDOW_WIDTH, WINDOW_HEIGHT, 1),
      useHandShader(true),
      numTiles(1)
    {
      // Initialise the hand
      for (unsigned int i = 0; i < NUM_PARAMETERS; i++)
        handParams[i] = 0.0;
    }

    ~ModelViewer()
    {
    }    

    bool init()
    {
        // Some initial vectors for camera
        glm::vec3 pos(10.0f, 10.0f, 30.0f);
        glm::vec3 target(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0, 1.0f, 0.0f);

        pipeline.setCamera(pos, target, up);
        float aspect = (float) WINDOW_WIDTH / WINDOW_HEIGHT;
        pipeline.setPerspectiveProj(45.6f, aspect, 10.0f, 1000.0f);   

        // Get meshes initialised
        mesh.init(0.5f, 50, 50, 0.5f, 1.0f, 20);

        // Load shaders from file
        handShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/hand_vs.glslv");
        handShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/hand_fs.glslf");
        // Compile and then link the shaders
        handShader.createAndLinkProgram();

        // Load shaders from file
        tileShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/tile_vs.glslv");
        tileShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/tile_fs.glslf");
        // Compile and then link the shaders
        tileShader.createAndLinkProgram();

        tsLocation = tileShader.addUniform("tileSize");
        tprLocation = tileShader.addUniform("tilesPerRow");
        npLocation = tileShader.addUniform("numPrimitives");

        return true;
    }

    void run()
    {
        GLUTBackendRun(this);
    }
    
    virtual void RenderSceneCB()
    {   
        // Clear all the GL stuff ready for rendering.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (useHandShader)
        {
          tileShader.unUse();
          handShader.use();
        }
        else
        {
          handShader.unUse();
          tileShader.use();

          int x = sqrt(numTiles);
          glUniform1f(tsLocation, 1./(x));
          glUniform1ui(tprLocation, (unsigned int) numTiles / x);
          glUniform1ui(npLocation, NUM_CYLINDERS);
        }

        glm::mat4 sphereWVPs[NUM_SPHERES*numTiles];
        glm::mat4 cylinderWVPs[NUM_CYLINDERS*numTiles];

        // Build a hand from parameters
        Hand h(handParams);
        // And add it to tile WVP arrays 
        if (numTiles != 1)
          h.addToTileArrays(sphereWVPs, cylinderWVPs, 0, pipeline);
        else
          h.addToArrays(sphereWVPs, cylinderWVPs, pipeline);

        // Here handle when we add more tiles (Generate random hands...)
        for (unsigned int i = 1; i < numTiles; i++)
        {
          double p[NUM_PARAMETERS];
          for(unsigned int j = 0; j < NUM_PARAMETERS; j++)
            p[j] = 0.0;

          // Generate random hands
          p[7] = rand() % 40 - 20; 
          p[8] = rand() % 180 - 90;
          p[9] = rand() % 180 - 90;
          p[10] = rand() % 180 - 90;

          p[11] = rand() % 40 - 20; 
          p[12] = rand() % 180 - 90;
          p[13] = rand() % 180 - 90;
          p[14] = rand() % 180 - 90;

          p[15] = rand() % 40 - 20; 
          p[16] = rand() % 180 - 90;
          p[17] = rand() % 180 - 90;
          p[18] = rand() % 180 - 90;

          p[19] = rand() % 40 - 20; 
          p[20] = rand() % 180 - 90;
          p[21] = rand() % 180 - 90;
          p[22] = rand() % 180 - 90;

          p[23] = rand() % 30 - 15; 
          p[24] = rand() % 30 - 15;
          p[25] = rand() % 100 - 50;
          p[26] = rand() % 180 - 90;

          Hand h(p);
          h.addToTileArrays(sphereWVPs, cylinderWVPs, i, pipeline);
        }

        mesh.renderCylinders(
            NUM_CYLINDERS*numTiles,
            cylinderWVPs);

        if (!useHandShader)
          glUniform1ui(npLocation, NUM_SPHERES);

        mesh.renderSpheres(
            NUM_SPHERES*numTiles, 
            sphereWVPs);

        glutSwapBuffers();
    }

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void TimerCB(int val)
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
    }

    virtual void KeyboardCB(unsigned char key, int x, int y)
    {
      // Capital letter pressed
      // Position
      if (key >= 'A' && key <= 'C')
        handParams[key - 65] -= 0.5;
      // Orientation requires smaller adjustments
      else if (key >= 'D' && key <= 'G')
        handParams[key - 65] -= 0.1;
      // Adjusting angles of fingers
      else if (key >= 'H' && key <= '[')
        handParams[key - 65] -= 1.0;

      // Lower case letter pressed
      // Position
      else if (key >= 'a' && key <= 'c')
        handParams[key - 97] += 0.5;
      // Orientation
      else if (key >= 'd' && key <= 'g')
        handParams[key - 97] += 0.1;
      // Joint angles
      else if (key >= 'h' && key <= '{')
        handParams[key - 97] += 1.0;

      // Now handle other key presses
      else
      {
        switch (key) {
          case 27:
            glutLeaveMainLoop();
            break;
          case '1':
            numTiles = 1;
            break;
          case '2':
            numTiles = 4;
            break;
          case'3':
            numTiles = 9;
            break;
          case'4':
            numTiles = 16;
            break;
          case '5':
            numTiles = 25;
            break;
          case '6':
            numTiles = 64;
            break;
          case '7':
            numTiles = 144;
            break;
          case '8':
            numTiles = 400;
            break;
          case '#':
            useHandShader = !useHandShader;
            break;
        }

        pipeline.setTiles(numTiles);
      }
    }

    virtual void PassiveMouseCB(int x, int y)
    {
    }
    
    virtual void MouseCB(int Button, int State, int x, int y)
    {
    }

private:
    Mesh mesh;
    Shader handShader, tileShader;
    Pipeline pipeline;

    bool useHandShader;

    double handParams[NUM_PARAMETERS];
    unsigned int numTiles;
    GLuint tsLocation, tprLocation, npLocation;

};

int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Model Viewer")) {
        return 1;
    }
    
    ModelViewer* app = new ModelViewer();

    if (!app->init()) {
        return 1;
    }
    srand((unsigned)time(NULL));
    
    app->run();
    delete app;
    return 0;
}
