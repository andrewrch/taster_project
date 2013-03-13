#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opencv2/opencv.hpp>

#include "pipeline.hpp"
#include "hand.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "glut_backend.hpp"

#include "histogram.hpp"
#include "thresholder.hpp"
#include "classifier.hpp"

#include "particleswarm.hpp"

#include "scorer.hpp"

static const unsigned int WINDOW_WIDTH = 640;
static const unsigned int WINDOW_HEIGHT = 480;
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
        unsigned int width,
        unsigned int height,
        string skinFilename, 
        string nonSkinFilename) :
      windowPipeline(WINDOW_WIDTH, WINDOW_HEIGHT, particles),
      renderPipeline(RENDER_WIDTH, RENDER_HEIGHT, particles),
      skinHist(skinFilename),
      nonSkinHist(nonSkinFilename),
      classifier(skinHist, nonSkinHist),
      thresholder(0.4, 0.5, 20),
      swarm(particles, NUM_PARAMETERS, c1, c2), 
      scorer(particles, 20, width, height),
      imageWidth(width),
      imageHeight(width),
      windowWidth(WINDOW_WIDTH),
      windowHeight(WINDOW_HEIGHT),
      renderWidth(sqrt(particles) * width),
      renderHeight(sqrt(particles) * height),
      numTiles(particles),
      swarmGenerations(generations)
    {
    }

    ~HandRenderer() {
      glDeleteFramebuffers(1,&fbo);
      glDeleteRenderbuffers(1,&renderBuffer);
    	glDeleteTextures(1, &bgrTexture);
    	glDeleteTextures(1, &depthTexture);
      glFinish();
    }   
    //At deinit:


    // Function turn a cv::Mat into a texture
    void matToTexture(GLuint textureID, cv::Mat &mat)
    {
      // Bind to our texture handle
      glBindTexture(GL_TEXTURE_2D, textureID);
      // Set texture interpolation methods for minification and magnification
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
 
      // Set incoming texture format to:
      // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
      // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
      // Work out other mappings as required ( there's a list in comments in main() )
      GLenum inputColourFormat = GL_BGR;
      GLenum inputType = GL_UNSIGNED_BYTE;
      GLenum internalFormat = GL_RGBA;
      if (mat.channels() == 1)
      {
        inputColourFormat = GL_LUMINANCE;
        inputType = GL_UNSIGNED_SHORT;
        internalFormat = GL_RGBA32F;
      }

//      mat = cv::Mat::ones(cv::Size(imageWidth, imageHeight), CV_16UC1) * 1000;
//      mat = mat * 255;
     
      // Create the texture
      glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                   0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                   internalFormat,    // Internal colour format to convert to
                   mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                   mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                   0,                 // Border width in pixels (can either be 1 or 0)
                   inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                   inputType,         // Image data type
                   mat.ptr());        // The actual image data itself
    }

    bool init()
    {
        // Some initial vectors for camera
        glm::vec3 pos(0.0f, 300.0f, 650.0f);
        glm::vec3 target(0.0f, 0.0f, 0.0f);
        glm::vec3 up(0.0, 1.0f, 0.0f);

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

        printf("making buffers...\n");
        glGenFramebuffers(1,&fbo);
        glGenRenderbuffers(1,&renderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16UI, renderWidth, renderHeight);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

        glGenTextures(1, &depthTexture);
        glGenTextures(1, &bgrTexture);
        // Initialise texture to blank image (Required for CL interop)
        cv::Mat blank = cv::Mat::zeros(cv::Size(640, 480), CV_16UC1);
        matToTexture(depthTexture, blank);
        blank = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
        matToTexture(bgrTexture, blank);

        // OpenCL interop stuff here
        scorer.loadProgram("./src/kernels/distancekernel.cl");
        scorer.loadData(renderBuffer, depthTexture);

        // Open the kinect up
        capture = cv::VideoCapture(CV_CAP_OPENNI);

        // Structure element for morphology whatsit
        se = cv::Mat::zeros(cv::Size(5, 5), CV_8UC1);
        cv::circle(se, cv::Point(3, 3), 2, cv::Scalar(255), -1);

        return true;
    }

    void run()
    {
        GLUTBackendRun(this);
    }

    void drawBackground()
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
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
        glm::mat4 sphereWVPs[NUM_SPHERES * numTiles];
        glm::mat4 cylinderWVPs[NUM_CYLINDERS * numTiles];

        glm::mat4 sphereWVs[NUM_SPHERES * numTiles];
        glm::mat4 cylinderWVs[NUM_CYLINDERS * numTiles];

        vector<Particle>& particles = swarm.getParticles();
        for (unsigned int i = 0; i < particles.size(); i++)
        {
          // For each particle in swarm build a hand
          Hand h(particles[i].getArray());
          // And add it to tile WVP arrays 
          h.addToTileArrays(sphereWVPs, cylinderWVPs, i, windowPipeline);
          h.addToWVArrays(sphereWVs, cylinderWVs, i, windowPipeline);
        }

        //Before drawing
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);
        glViewport(0, 0, renderWidth, renderHeight);
        // Clear all the GL stuff ready for rendering.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tileShader.use();

        int x = sqrt(numTiles);
        glUniform1f(tsLocation, 1./(x));
        glUniform1ui(tprLocation, (unsigned int) numTiles / x);
        glUniform1ui(npLocation, NUM_CYLINDERS);
        mesh.renderCylinders(NUM_CYLINDERS * numTiles, cylinderWVPs, cylinderWVs);
        glUniform1ui(npLocation, NUM_SPHERES);
        mesh.renderSpheres(NUM_SPHERES * numTiles, sphereWVPs, sphereWVs);
        tileShader.unUse();

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
//        Uncomment this to draw to window.
//        *********************************
//
        glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
       
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

    void processImages(cv::Mat& bgrImage, cv::Mat& depthMap)
    {
      capture.grab();
      capture.retrieve( depthMap, CV_CAP_OPENNI_DEPTH_MAP );
      capture.retrieve( bgrImage, CV_CAP_OPENNI_BGR_IMAGE );

      // Require these images from the kinect as well...
      cv::Mat validPixels;
      cv::Mat dispMap;
	  	capture.retrieve( validPixels, CV_CAP_OPENNI_VALID_DEPTH_MASK );
		  capture.retrieve( dispMap, CV_CAP_OPENNI_DISPARITY_MAP );

      cv::Mat bgrBlurred = bgrImage.clone();
      //cv::medianBlur(yuvImage, yuvImage, 3);
      cv::GaussianBlur(bgrImage, bgrBlurred, cv::Size(25, 25), 1.5);

      // Do processing with them
      cv::Mat yuvImage;
      cv::cvtColor(bgrBlurred, yuvImage, CV_BGR2YCrCb);
      cv::Mat prob = classifier.classifyImage(yuvImage);
      cv::Mat skin = thresholder.thresholdImage(prob, depthMap, validPixels); 

      cv::morphologyEx(skin, skin, cv::MORPH_CLOSE, se);
      cv::morphologyEx(skin, skin, cv::MORPH_DILATE, se);

      // Find contours in skin image
      vector<vector<cv::Point> > contours;
      vector<cv::Vec4i> hierarchy;
      cv::Mat c = skin.clone();
      cv::findContours(
          c, 
          contours, 
          hierarchy, 
          CV_RETR_LIST, 
          CV_CHAIN_APPROX_NONE, 
          cv::Point(0, 0));

      // Find largest contour
      unsigned int largestContour = 0, size = 0;
      for (unsigned int i = 0; i < contours.size(); i++)
        if (contours[i].size() > size)
        {
          largestContour = i;
          size = contours[i].size();
        }

      // Draw largest contour, filled
      skin = cv::Mat::zeros( bgrImage.size(), CV_8UC1 );
      cv::drawContours(
          skin, 
          contours, 
          largestContour, 
          cv::Scalar(255), 
          CV_FILLED, 
          8, 
          hierarchy, 
          0, 
          cv::Point() );

//      depthMap = cv::Mat::zeros(bgrImage.size(), CV_8UC1);
      for (int i = 0; i < depthMap.rows; i++)
        for (int j = 0; j < depthMap.cols; j++)
          if (!skin.at<uchar>(i, j))
            depthMap.at<uint16_t>(i, j) = 0; //dispMap.at<uchar>(i, j);
//          else
//            depthMap.at<uint16_t>(i, j) = 10000; //dispMap.at<uchar>(i, j);

      cv::resize(depthMap, depthMap, cv::Size(imageWidth, imageHeight));
      //imshow("d", depthMap);
    }

    void drawHand()
    {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, windowWidth, windowHeight);
      // First draw the background
      drawBackground();

      glm::mat4 sphereWVPs[NUM_SPHERES];
      glm::mat4 cylinderWVPs[NUM_CYLINDERS];
      glm::mat4 sphereWVs[NUM_SPHERES];
      glm::mat4 cylinderWVs[NUM_CYLINDERS];

      Particle p = swarm.getBestParticle();
      Hand h(p.getArray());
      h.addToWVPArrays(sphereWVPs, cylinderWVPs, 0, windowPipeline);
      h.addToWVArrays(sphereWVs, cylinderWVs, 0, windowPipeline);

      renderShader.use();
      mesh.renderCylinders(NUM_CYLINDERS, cylinderWVPs, cylinderWVs);
      mesh.renderSpheres(NUM_SPHERES, sphereWVPs, sphereWVs);
      renderShader.unUse();

      glutSwapBuffers();
    }

    virtual void RenderSceneCB()
    {   
      for (unsigned int i = 0; i < swarmGenerations; i++)
      {
        makeTiledRendering();
        scorer.calculateScores();
        // Get scores from rendering
        vector<double> scores(numTiles);
        for (unsigned int j = 0; j < numTiles; j++)
          scores[j] = rand() / double(RAND_MAX);

        scores[0] = 1.0;
        swarm.updateSwarm(scores);
      }
      drawHand();
      swarm.resetScores();
    }

    virtual void IdleCB()
    {
      cv::Mat depthImage;
      cv::Mat bgrImage;

      processImages(bgrImage, depthImage);
      // Generate a number for our textureID's unique handle
      matToTexture(bgrTexture, bgrImage);
      matToTexture(depthTexture, depthImage);
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

    virtual void TimerCB(int val)
    {
//        RenderSceneCB();
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
    Thresholder thresholder;
    ParticleSwarm swarm;
    Scorer scorer;

    unsigned int imageWidth, imageHeight, \
                 windowWidth, windowHeight, \
                 renderWidth, renderHeight, \
                 numTiles, swarmGenerations;
    GLuint bgrTexture, depthTexture;
    GLuint fbo, renderBuffer;
    GLuint tsLocation, tprLocation, npLocation;
    cv::Mat se;

};

int main(int argc, char** argv)
{
    // Seed random numbers
    srand(time(NULL));

    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Hand Renderer")) {
        return 1;
    }
    
    HandRenderer* app = new HandRenderer(atoi(argv[1]), atoi(argv[2]), 320, 240, argv[3], argv[4]);

    if (!app->init()) {
        return 1;
    }

    
    app->run();
    delete app;
    return 0;
}
