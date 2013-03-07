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
#include "opencv2/opencv.hpp"

#include "histogram.hpp"
#include "thresholder.hpp"
#include "classifier.hpp"

#include "particleswarm.hpp"

static const unsigned int WINDOW_WIDTH = 1024;
static const unsigned int WINDOW_HEIGHT = 768;
static const unsigned int RENDER_WIDTH = 4096;
static const unsigned int RENDER_HEIGHT = 3072;

static const double c1 = 2.8;
static const double c2 = 1.3;

using namespace std;

class HandRenderer : public ICallbacks
{
  public:
    HandRenderer(
        unsigned int particles, 
        unsigned int generations,
        string skinFilename, 
        string nonSkinFilename) :
      windowPipeline(WINDOW_WIDTH, WINDOW_HEIGHT, particles),
      renderPipeline(RENDER_WIDTH, RENDER_HEIGHT, particles),
      skinHist(skinFilename),
      nonSkinHist(nonSkinFilename),
      classifier(skinHist, nonSkinHist),
      swarm(particles, NUM_PARAMETERS, c1, c2), 
      windowWidth(WINDOW_WIDTH),
      windowHeight(WINDOW_HEIGHT),
      renderWidth(RENDER_WIDTH),
      renderHeight(RENDER_HEIGHT),
      numTiles(particles),
      swarmGenerations(generations)
    {
    }

    ~HandRenderer() {}   
    //At deinit:
    //glDeleteFramebuffers(1,&fbo);
    //glDeleteRenderbuffers(1,&renderBuffer);


    // Function turn a cv::Mat into a texture, and return the texture ID as a GLuint for use
    GLuint matToTexture(cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter)
    {
      // Generate a number for our textureID's unique handle
      GLuint textureID;
      glGenTextures(1, &textureID);
     
      // Bind to our texture handle
      glBindTexture(GL_TEXTURE_2D, textureID);
     
      // Catch silly-mistake texture interpolation method for magnification
      if (magFilter == GL_LINEAR_MIPMAP_LINEAR  ||
          magFilter == GL_LINEAR_MIPMAP_NEAREST ||
          magFilter == GL_NEAREST_MIPMAP_LINEAR ||
          magFilter == GL_NEAREST_MIPMAP_NEAREST)
      {
        cout << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << endl;
        magFilter = GL_LINEAR;
      }
     
      // Set texture interpolation methods for minification and magnification
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
     
      // Set texture clamping method
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);
     
      // Set incoming texture format to:
      // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
      // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
      // Work out other mappings as required ( there's a list in comments in main() )
      GLenum inputColourFormat = GL_BGR;
      if (mat.channels() == 1)
      {
        inputColourFormat = GL_LUMINANCE;
      }
     
      // Create the texture
      glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                   0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                   GL_RGB,            // Internal colour format to convert to
                   mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                   mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                   0,                 // Border width in pixels (can either be 1 or 0)
                   inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                   GL_UNSIGNED_BYTE,  // Image data type
                   mat.ptr());        // The actual image data itself
     
      // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
      if (minFilter == GL_LINEAR_MIPMAP_LINEAR  ||
          minFilter == GL_LINEAR_MIPMAP_NEAREST ||
          minFilter == GL_NEAREST_MIPMAP_LINEAR ||
          minFilter == GL_NEAREST_MIPMAP_NEAREST)
      {
        glGenerateMipmap(GL_TEXTURE_2D);
      }
     
      return textureID;
    }

    bool init()
    {
        // Some initial vectors for camera
        glm::vec3 pos(0.0f, 30.0f, 65.0f);
        glm::vec3 target(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0, 1.0f, 0.0f);

        ////// 
        //////
        /// Do I really need two of these?
        float aspect = (float) windowWidth / windowHeight;
        windowPipeline.setCamera(pos, target, up);
        windowPipeline.setPerspectiveProj(45.6f, aspect, 40.0f, 1000.0f);   
        renderPipeline.setCamera(pos, target, up);
        renderPipeline.setPerspectiveProj(45.6f, aspect, 40.0f, 1000.0f);   

        // Get meshes initialised
        mesh.init(0.5f, 50, 50, 0.5f, 1.0f, 20);

        // Load shaders for tiled rendering
        tileShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/tile_vs.glslv");
        tileShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/tile_fs.glslf");
        tileShader.createAndLinkProgram();
        tsLocation = tileShader.addUniform("tileSize");
        tprLocation = tileShader.addUniform("tilesPerRow");
        npLocation = tileShader.addUniform("numPrimitives");

        // Load shaders for display render
        renderShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/hand_vs.glslv");
        renderShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/hand_fs.glslf");
        // Compile and then link the shaders
        renderShader.createAndLinkProgram();

        glGenFramebuffers(1,&fbo);
        glGenRenderbuffers(1,&renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, renderWidth, renderHeight);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
        // Open the kinect up
        capture = cv::VideoCapture(CV_CAP_OPENNI);

        return true;
    }

    void run()
    {
        GLUTBackendRun(this);
    }

    void drawBackground()
    {
        tileShader.unUse();
        renderShader.unUse();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bgrTexture);
        glDepthMask(GL_FALSE);
        glBegin (GL_TRIANGLES);
          glTexCoord2f (1.0, 1.0);
          glVertex2f (-1.0, -1.0);

          glTexCoord2f (0.0, 0.0);
          glVertex2f (1.0, 1.0);

          glTexCoord2f (0.0, 1.0);
          glVertex2f (1.0, -1.0);

          glTexCoord2f (1.0, 1.0);
          glVertex2f (-1.0, -1.0);

          glTexCoord2f (1.0, 0.0);
          glVertex2f (-1.0, 1.0);

          glTexCoord2f (0.0, 0.0);
          glVertex2f (1.0, 1.0);
        glEnd ();
        glDepthMask(GL_TRUE);
        glDisable(GL_TEXTURE_2D);
    }

    bool makeTiledRendering()
    {
        tileShader.use();
        glm::mat4 sphereWVPs[NUM_SPHERES * numTiles];
        glm::mat4 cylinderWVPs[NUM_CYLINDERS * numTiles];

        vector<Particle>& particles = swarm.getParticles();
        for (unsigned int i = 0; i < particles.size(); i++)
        {
          // For each particle in swarm build a hand
          Hand h(particles[i].getArray());
          // And add it to tile WVP arrays 
          h.addToTileArrays(sphereWVPs, cylinderWVPs, i, windowPipeline);
        }

        //Before drawing
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);
        glViewport(0, 0, renderWidth, renderHeight);

        // Clear all the GL stuff ready for rendering.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Add uniforms
        int x = sqrt(numTiles);
        glUniform1f(tsLocation, 1./(x));
        glUniform1ui(tprLocation, (unsigned int) numTiles / x);
        glUniform1ui(npLocation, NUM_CYLINDERS);
        
        // Render to the renderbuffer
        mesh.renderCylinders(
            NUM_CYLINDERS * numTiles,
            cylinderWVPs);

        glUniform1ui(npLocation, NUM_SPHERES);

        mesh.renderSpheres(
            NUM_SPHERES * numTiles, 
            sphereWVPs);

        tileShader.unUse();

        glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
       
        //Set what color attachment we are reading
        glReadBuffer(GL_COLOR_ATTACHMENT0);
       
        //Set to what attachment we will write
        glDrawBuffer(GL_BACK);
       
        //This function does the magic of copying your FBO to the default one. 
        //You could use it to copy only a region of the FBO to another region of the screen
        //The parameter GL_LINEAR is the function to use in case the FBOs have different sizes.
        glBlitFramebuffer(0, 0, renderWidth, renderHeight,
           0, 0, windowWidth, windowHeight,
           GL_COLOR_BUFFER_BIT, GL_LINEAR);
       
        //Resets the Draw Buffer to the default one
        glDrawBuffer(GL_BACK);

        glutSwapBuffers();

        return GLCheckError();
    }

    cv::Mat getSegmentedHand(cv::Mat& depthImage)
    {
      cv::Mat cat;
      return cat;
    }

    void drawHand()
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // First draw the background
      drawBackground();
      // Want to render with correct shaders
      renderShader.use();

      glm::mat4 sphereWVPs[NUM_SPHERES];
      glm::mat4 cylinderWVPs[NUM_CYLINDERS];

      Particle p = swarm.getBestParticle();
      Hand h(p.getArray());
      h.addToArrays(sphereWVPs, cylinderWVPs, windowPipeline);

      mesh.renderCylinders(NUM_CYLINDERS, cylinderWVPs);
      mesh.renderSpheres(NUM_SPHERES, sphereWVPs);

      renderShader.unUse();

      glutSwapBuffers();
    }

    virtual void RenderSceneCB()
    {   
      for (unsigned int i = 0; i < swarmGenerations; i++)
      {
        vector<double> scores(numTiles);
        for (unsigned int j = 0; j < numTiles; j++)
          scores[j] = rand() / double(RAND_MAX);

        scores[0] = 1.0;
        swarm.updateSwarm(scores);
        makeTiledRendering();

      }
      // First ree/der the tiles
      // Now draw the best hand
      //drawHand();
      swarm.resetScores();
    }

    virtual void IdleCB()
    {
      cv::Mat depthImage;
      cv::Mat bgrImage;
      capture.grab();
      capture.retrieve( depthImage, CV_CAP_OPENNI_DEPTH_MAP );
      capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );

      bgrTexture = matToTexture(bgrImage, GL_NEAREST, GL_NEAREST, GL_CLAMP);
      depthTexture = matToTexture(depthImage, GL_NEAREST, GL_NEAREST, GL_CLAMP);
      RenderSceneCB();
    	glDeleteTextures(1, &bgrTexture);
    	glDeleteTextures(1, &depthTexture);
    }

    virtual void TimerCB(int val)
    {
        RenderSceneCB();
    }

    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
            case 'q':
                glutLeaveMainLoop();
                break;
        }
    }

    virtual void PassiveMouseCB(int x, int y) {}
    virtual void MouseCB(int Button, int State, int x, int y) {}
    virtual void SpecialKeyboardCB(int Key, int x, int y) {}

private:
    Mesh mesh;
    Shader tileShader, renderShader;
    Pipeline windowPipeline, renderPipeline;
    cv::VideoCapture capture;
    Histogram skinHist, nonSkinHist;
    Classifier classifier;

    ParticleSwarm swarm;

    unsigned int windowWidth, windowHeight, \
                 renderWidth, renderHeight, \
                 numTiles, swarmGenerations;
    GLuint bgrTexture, depthTexture;
    GLuint fbo, renderBuffer;
    GLuint tsLocation, tprLocation, npLocation;
};

int main(int argc, char** argv)
{
    // Seed random numbers
    srand(time(NULL));

    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Hand Renderer")) {
        return 1;
    }
    
    HandRenderer* app = new HandRenderer(atoi(argv[1]), atoi(argv[2]), argv[3], argv[4]);

    if (!app->init()) {
        return 1;
    }

    
    app->run();
    delete app;
    return 0;
}
