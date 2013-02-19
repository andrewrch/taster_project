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
static const unsigned int WINDOW_HEIGHT = 1024;

static const unsigned int NUM_TILES = 36;

class HandRenderer : public ICallbacks
{
  public:

    HandRenderer() :
      p(NUM_TILES),
      h(p)
    {
    }

    ~HandRenderer()
    {
    }    

    bool init()
    {
        // Some initial vectors for camera
        glm::vec3 pos(0.0f, 0.0f, 35.0f);
        glm::vec3 target(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0, 1.0f, 0.0f);

        p.setCamera(pos, target, up);
        p.setPerspectiveProj(60.0f, (float) WINDOW_HEIGHT/ WINDOW_WIDTH, 1.0f, 100.0f);   
        //p.setRotate(0.0f, 90.0f, 0.0f);

        // Get meshes initialised
        mesh.init(0.5f, 50, 50, 0.5f, 1.0f, 20);

        // Record time for FPS count.
        //time = glutGet(GLUT_ELAPSED_TIME);
        // Load shaders from file
        shader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/vs.glslv");
        shader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/fs.glslf");
        // Compile and then link the shaders
        shader.createAndLinkProgram();
        shader.use();
        return true;
    }

    void run()
    {
        GLUTBackendRun(this);
    }
    
    virtual void RenderSceneCB()
    {   
        //calcFPS();
        
        // Clear all the GL stuff ready for rendering.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//        int numSpheres = 13 * NUM_TILES;
        glm::mat4 sphereWVPs[HAND_SPHERES * NUM_TILES];
//        int numCylinders = 8 * NUM_TILES;
        glm::mat4 cylinderWVPs[HAND_CYLINDERS * NUM_TILES];

        float handParams[NUM_TILES][NUM_PARAMETERS];

        for (unsigned int i = 0; i < NUM_TILES; i++)
        {
          for (unsigned int j = 0; j < NUM_PARAMETERS; j++)
          {
            // Some random params
            handParams[i][j] = (rand() - RAND_MAX / 2) % 30; 
          }
          h.initialiseHand(sphereWVPs, cylinderWVPs, i, handParams[i]);
        }

        mesh.renderCylinders(
            HAND_CYLINDERS * NUM_TILES,
            cylinderWVPs,
            cylinderWVPs);

        mesh.renderSpheres(
            HAND_SPHERES * NUM_TILES, 
            sphereWVPs, 
            sphereWVPs);

        glutSwapBuffers();
    }

    virtual void IdleCB()
    {
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
    }

    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
            case 'q':
                glutLeaveMainLoop();
                break;
        }
    }

    virtual void PassiveMouseCB(int x, int y)
    {
    }
    
    virtual void MouseCB(int Button, int State, int x, int y)
    {
    }

private:
    
    void calcFPS()
    {
        frameCount++;
        
//        int time = glutGet( GLUT_ELAPSED_TIME );

//        if (time - this->time > 1000) {
//            this->fps = (float) this->frameCount * 1000.0f / (time - this->time);
//            this->time = time;
//            this->frameCount = 0;
//        }
    }
    
    Mesh mesh;
    Shader shader;
    Pipeline p;
    Hand h;
//    int time;
    int frameCount;
    float fps;
};

int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Hand Renderer")) {
        return 1;
    }
    
    HandRenderer* app = new HandRenderer();

    if (!app->init()) {
        return 1;
    }
    srand(time(NULL));
    
    app->run();
    delete app;
    return 0;
}
