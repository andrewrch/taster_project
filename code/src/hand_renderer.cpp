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

static const unsigned int NUM_TILES = 1;

class HandRenderer : public ICallbacks
{
  public:

    HandRenderer() :
      p(NUM_TILES)
    {
    }

    ~HandRenderer()
    {
    }    

    bool init()
    {
        // Some initial vectors for camera
        glm::vec3 pos(30.0f, 30.0f, 65.0f);
        glm::vec3 target(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0, 1.0f, 0.0f);

        p.setCamera(pos, target, up);
        p.setPerspectiveProj(45.6f, (float) WINDOW_HEIGHT/ WINDOW_WIDTH, 1.0f, 100.0f);   
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

        glm::mat4 sphereWVPs[NUM_SPHERES * NUM_TILES];
        glm::mat4 cylinderWVPs[NUM_CYLINDERS * NUM_TILES];

        float handParams[NUM_TILES][NUM_PARAMETERS];
        for (unsigned int i = 0; i < NUM_TILES; i++)
        {
          for (unsigned int j = 0; j < NUM_PARAMETERS; j++)
          {
            handParams[i][j] = (rand() - RAND_MAX / 2) % 10; 
            //handParams[i][j] = 0.0f;
//            handParams[i][THUMB_ROT_1X] = 45;
//            handParams[i][THUMB_ROT_1Z] = -45;
            // Some random params
          }
          Hand h(handParams[i]);
          h.addToTileArrays(sphereWVPs, cylinderWVPs, i, p);
          //h.initialiseHand(sphereWVPsTiled, sphereWVPs, cylinderWVPsTiled, cylinderWVPs, i, handParams[i]);
        }

        // Add uniforms
        GLuint thLocation = shader.addUniform("tileHeight");
        glUniform1f(thLocation, 1./sqrt(NUM_TILES));
        GLuint twLocation = shader.addUniform("tileWidth");
        glUniform1f(twLocation, 1./sqrt(NUM_TILES));

        GLuint ntxLocation = shader.addUniform("numTilesX");
        glUniform1ui(ntxLocation, (unsigned int) sqrt(NUM_TILES));
        GLuint ntyLocation = shader.addUniform("numTilesY");
        glUniform1ui(ntyLocation, (unsigned int) sqrt(NUM_TILES));
        GLuint npLocation = shader.addUniform("numPrimitives");
        glUniform1ui(npLocation, NUM_CYLINDERS);

        mesh.renderCylinders(
            NUM_CYLINDERS * NUM_TILES,
            cylinderWVPs);

        glUniform1ui(npLocation, NUM_SPHERES);

        mesh.renderSpheres(
            NUM_SPHERES * NUM_TILES, 
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
