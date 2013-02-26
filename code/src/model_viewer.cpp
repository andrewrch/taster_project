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

class ModelViewer : public ICallbacks
{
  public:

    ModelViewer() :
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

        // Load shaders from file
        shader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/model_view_vs.glslv");
        shader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/model_view_fs.glslf");
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
        // Clear all the GL stuff ready for rendering.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mesh.renderCylinders(
            NUM_CYLINDERS
            cylinderWVPs);

        mesh.renderSpheres(
            NUM_SPHERES, 
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
        switch (key) {
            case 'a':
            case 'b':
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
    Mesh mesh;
    Shader shader;
    Pipeline p;
    glm::mat4 sphereWVPs, cylinderWVPs;
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
    srand(time(NULL));
    
    app->run();
    delete app;
    return 0;
}
