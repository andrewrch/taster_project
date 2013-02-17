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

class HandRenderer : public ICallbacks
{
  public:

    HandRenderer()
    {
    }

    ~HandRenderer()
    {
    }    

    bool init()
    {
        // Some initial vectors for camera
        glm::vec3 pos(-10.0f, 10.0f, 10.0f);
        glm::vec3 target(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0, 1.0f, 0.0f);

        p.setCamera(pos, target, up);
        p.setPerspectiveProj(45.0f, (float) WINDOW_HEIGHT/ WINDOW_WIDTH, 1.0f, 100.0f);   
        p.setRotate(0.0f, 90.0f, 0.0f);

        // Get meshes initialised
        mesh.init(0.5f, 50, 50, 0.5f, 0.5f, 20);

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

        int numSpheres = 5;
        glm::mat4 sphereWVPs[numSpheres];
        int numCylinders = 5;
        glm::mat4 cylinderWVPs[numCylinders];

        for (int i = 0; i < numSpheres; i++)
        {
          p.setWorldPos(0, 0, i*2);
          sphereWVPs[i] = p.getWVPTrans();
        }

        for (int i = 0; i < numCylinders; i++)
        {
          p.setWorldPos(i * 2, 0, 0);
          cylinderWVPs[i] = p.getVPTrans();
        }

        mesh.renderCylinders(
            numCylinders,
            cylinderWVPs,
            cylinderWVPs);

        //mesh.renderSpheres(
        //    numSpheres, 
        //    sphereWVPs, 
        //    sphereWVPs);

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
