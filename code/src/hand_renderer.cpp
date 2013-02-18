#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "pipeline.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "glut_backend.hpp"

static const int WINDOW_WIDTH = 1024;
static const int WINDOW_HEIGHT = 1024;

static const int NUM_TILES = 25;

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
        glm::vec3 pos(-10.0f, 0.0f, -10.0f);
        glm::vec3 target(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0, 1.0f, 0.0f);

        p.setCamera(pos, target, up);
        p.setPerspectiveProj(90.0f, (float) WINDOW_HEIGHT/ WINDOW_WIDTH, 1.0f, 100.0f);   
        p.setRotate(0.0f, 90.0f, 0.0f);

        // Get meshes initialised
        mesh.init(0.5f, 50, 50, 0.5f, 1.0f, 20);

        // Record time for FPS count.
        time = glutGet(GLUT_ELAPSED_TIME);
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

        int numSpheres = 12 * NUM_TILES;
        glm::mat4 sphereWVPs[numSpheres];
        int numCylinders = 8 * NUM_TILES;
        glm::mat4 cylinderWVPs[numCylinders];

        for (int i = 0; i < NUM_TILES; i++)
        {
          p.setScale(1, 1, 1);
          p.setWorldPos(0, 0, 0);
          sphereWVPs[i*12+0] = p.getTileTrans(i);

          p.setWorldPos(2, 0, 0);
          sphereWVPs[i*12+1] = p.getTileTrans(i);
         
          p.setWorldPos(4, 0, 0);
          sphereWVPs[i*12+2] = p.getTileTrans(i);

          p.setWorldPos(6, 0, 0);
          sphereWVPs[i*12+3] = p.getTileTrans(i);

          p.setWorldPos(0, 3, 0);
          sphereWVPs[i*12+4] = p.getTileTrans(i);

          p.setWorldPos(2, 3, 0);
          sphereWVPs[i*12+5] = p.getTileTrans(i);
         
          p.setWorldPos(4, 3, 0);
          sphereWVPs[i*12+6] = p.getTileTrans(i);

          p.setWorldPos(6, 3, 0);
          sphereWVPs[i*12+7] = p.getTileTrans(i);

          p.setWorldPos(0, 6, 0);
          sphereWVPs[i*12+8] = p.getTileTrans(i);

          p.setWorldPos(2, 6, 0);
          sphereWVPs[i*12+9] = p.getTileTrans(i);
         
          p.setWorldPos(4, 6, 0);
          sphereWVPs[i*12+10] = p.getTileTrans(i);

          p.setWorldPos(6, 6, 0);
          sphereWVPs[i*12+11] = p.getTileTrans(i);


          p.setScale(1, 3, 1);
          p.setWorldPos(0, 0, 0);
          cylinderWVPs[i*8+0] = p.getTileTrans(i);

          p.setWorldPos(0, 3, 0);
          cylinderWVPs[i*8+1] = p.getTileTrans(i);

          p.setWorldPos(2, 0, 0);
          cylinderWVPs[i*8+2] = p.getTileTrans(i);

          p.setWorldPos(2, 3, 0);
          cylinderWVPs[i*8+3] = p.getTileTrans(i);

          p.setWorldPos(4, 0, 0);
          cylinderWVPs[i*8+4] = p.getTileTrans(i);

          p.setWorldPos(4, 3, 0);
          cylinderWVPs[i*8+5] = p.getTileTrans(i);

          p.setWorldPos(6, 0, 0);
          cylinderWVPs[i*8+6] = p.getTileTrans(i);

          p.setWorldPos(6, 3, 0);
          cylinderWVPs[i*8+7] = p.getTileTrans(i);
        }


        mesh.renderCylinders(
            numCylinders,
            cylinderWVPs,
            cylinderWVPs);

        mesh.renderSpheres(
            numSpheres, 
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
        
        int time = glutGet( GLUT_ELAPSED_TIME );

        if (time - this->time > 1000) {
            this->fps = (float) this->frameCount * 1000.0f / (time - this->time);
            this->time = time;
            this->frameCount = 0;
        }
    }
    
    Mesh mesh;
    Shader shader;
    Pipeline p;
    int time;
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
    
    app->run();
    delete app;
    return 0;
}
