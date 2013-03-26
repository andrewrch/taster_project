#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <opencv2/opencv.hpp>

#include "pipeline.hpp"
#include "hand.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "histogram.hpp"
#include "thresholder.hpp"
#include "classifier.hpp"
#include "particleswarm.hpp"
#include "scorer.hpp"

static const unsigned int WINDOW_WIDTH = 640;
static const unsigned int WINDOW_HEIGHT = 480;

static const double c1 = 2.8;
static const double c2 = 1.3;

using namespace std;

class HandRenderer
{
  public:
    HandRenderer(
        unsigned int particles, 
        unsigned int generations,
        unsigned int width,
        unsigned int height,
        string skinFilename, 
        string nonSkinFilename) :
      skinHist(skinFilename),
      nonSkinHist(nonSkinFilename),
      classifier(skinHist, nonSkinHist),
      thresholder(0.4, 0.5, 20),
      swarm(particles, NUM_PARAMETERS, c1, c2), 
      scorer(particles, 80, 100, width, height),
      imageWidth(width),
      imageHeight(height),
      windowWidth(WINDOW_WIDTH),
      windowHeight(WINDOW_HEIGHT),
      renderWidth(sqrt(particles) * width),
      renderHeight(sqrt(particles) * height),
      numTiles(particles),
      swarmGenerations(generations),
      windowPipeline(windowWidth, windowHeight, numTiles),
      renderPipeline(renderWidth, renderHeight, numTiles),
      frameCount(0),
      started(false)
    {
    }

    ~HandRenderer() {
      glFinish();
      glDeleteFramebuffers(1,&tileFBO);
      glDeleteRenderbuffers(1,&tileRB);
    	glDeleteTextures(1, &bgrTexture);
    	glDeleteTextures(1, &depthTexture);
    }   

    // Function turn a cv::Mat into a texture
    void matToTexture(GLuint textureID, cv::Mat& mat)
    {
      // Bind to our texture handle
      glBindTexture(GL_TEXTURE_2D, textureID);
      // Set texture interpolation methods for minification and magnification
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S , GL_REPEAT );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

      //use fast 4-byte alignment (default anyway) if possible
      glPixelStorei(GL_PACK_ALIGNMENT, (mat.step & 3) ? 1 : 4);
      //set length of one complete row in destination data (doesn't need to equal img.cols)
      glPixelStorei(GL_PACK_ROW_LENGTH, mat.step/mat.elemSize());
 
      // Set incoming texture format
      GLenum inputColourFormat = GL_BGR;
      GLenum inputType = GL_UNSIGNED_BYTE;
      GLenum internalFormat = GL_RGBA;
      if (mat.channels() == 1)
      {
        inputColourFormat = GL_RED_INTEGER;
        inputType = GL_UNSIGNED_SHORT;
        internalFormat = GL_RGBA16UI;
      }

      // Create the texture
      glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                   0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                   internalFormat,    // Internal colour format to convert to
                   mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                   mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                   0,                 // Border width in pixels (can either be 1 or 0)
                   inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                   inputType,         // Image data type
                   mat.data);         // The actual image data itself

      glBindTexture(GL_TEXTURE_2D, 0);
    }

    //bool initCameras()
    //bool initShaders()
    //bool initPrimitives()

    bool init()
    {
      // OpenGL initialisation
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glFrontFace(GL_CW);
      glCullFace(GL_BACK);
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);

      // Some initial vectors for camera
      glm::vec3 pos(0.0f, 0.0f, 600.0f);
      glm::vec3 target(0.0f, 0.0f, 0.0f);
      glm::vec3 up(0.0, 1.0f, 0.0f);

      float aspect = (float) windowWidth / windowHeight;
      windowPipeline.setCamera(pos, target, up);
      windowPipeline.setPerspectiveProj(45.6f, aspect, 40.0f, 10000.0f);   
      renderPipeline.setCamera(pos, target, up);
      renderPipeline.setPerspectiveProj(45.6f, aspect, 40.0f, 10000.0f);   

      // Get meshes initialised
      //
      // Seperate this out as a sphere and cylinder class, and have
      // a single render function for each
      mesh.init(0.5f, 50, 50, 0.5f, 1.0f, 20);

      // Load shaders for tiled rendering
      tileShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/tile.glslv");
      tileShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/tile.glslf");
      tileShader.createAndLinkProgram();
      tsLocation = tileShader.addUniform("tileSize");
      tprLocation = tileShader.addUniform("tilesPerRow");
      npLocation = tileShader.addUniform("numPrimitives");

      // Load shaders for hand
      handShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/hand.glslv");
      handShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/hand.glslf");
      handShader.createAndLinkProgram();

      // Load shader to draw background image
      backgroundShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/background.glslv");
      backgroundShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/background.glslf");
      backgroundShader.createAndLinkProgram();
      backgroundTexLocation = backgroundShader.addUniform("textureSampler");

      glGenFramebuffers(1,&tileFBO);
      glBindFramebuffer(GL_FRAMEBUFFER, tileFBO);
      glGenRenderbuffers(1,&tileRB);
      glBindRenderbuffer(GL_RENDERBUFFER, tileRB);
      cout << "Render width: " << renderWidth << " height: " << renderHeight << endl;
      glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, renderWidth, renderHeight);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, tileRB);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      GLenum status;
      status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if(status != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "FrameBuffer is not complete.\n");

      glGenTextures(1, &depthTexture);
      glGenTextures(1, &bgrTexture);
      // Initialise texture to blank image (Required for CL interop)
      cv::Mat blank = cv::Mat::zeros(cv::Size(imageWidth, imageHeight), CV_16UC1);
      matToTexture(depthTexture, blank);
      blank = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
      matToTexture(bgrTexture, blank);

      initBackground();

      // OpenCL interop stuff here
      scorer.loadProgram("./src/kernels/distancekernel.cl");
      scorer.loadData(tileRB, depthTexture);

      // Open the kinect up
      capture = cv::VideoCapture(CV_CAP_OPENNI);

      // Structure element for morphology whatsit
      se = cv::Mat::zeros(cv::Size(5, 5), CV_8UC1);
      cv::circle(se, cv::Point(3, 3), 2, cv::Scalar(255), -1);
      previousFrame = cv::Mat(cv::Size(imageWidth, imageHeight), CV_16UC1);

      return true;
    }

    void makeTiledRendering()
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
      glBindFramebuffer(GL_FRAMEBUFFER, tileFBO);
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

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        Uncomment this to draw to window.
//        *********************************
//
//        glBindFramebuffer(GL_READ_FRAMEBUFFER,tileFBO);
//       
//        //Set what color attachment we are reading
//        glReadBuffer(GL_COLOR_ATTACHMENT0);
//       
//        //Set to what attachment we will write
//        glDrawBuffer(GL_BACK);
//       
//        //This function does the magic of copying your FBO to the default one. 
//        //You could use it to copy only a region of the FBO to another region of the screen
//        //The parameter GL_LINEAR is the function to use in case the FBOs have different sizes.
//        glBlitFramebuffer(0, 0, renderWidth, renderHeight,
//           0, 0, windowWidth, windowHeight,
//           GL_COLOR_BUFFER_BIT, GL_LINEAR);
//       
//        //Resets the Draw Buffer to the default one
//        glDrawBuffer(GL_BACK);
//
//        glfwSwapBuffers();
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
      //cv::GaussianBlur(bgrImage, bgrBlurred, cv::Size(25, 25), 1.5);

      // Do processing with them
      cv::Mat yuvImage;
      cv::cvtColor(bgrBlurred, yuvImage, CV_BGR2YCrCb);
      cv::Mat prob = classifier.classifyImage(yuvImage);
      cv::Mat skin = thresholder.thresholdImage(prob, depthMap, validPixels, previousFrame); 

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

      for (int i = 0; i < depthMap.rows; i++)
        for (int j = 0; j < depthMap.cols; j++)
          if (!skin.at<uchar>(i, j))
            depthMap.at<uint16_t>(i, j) = 0;

      // Finally resize the image if required
      cv::resize(depthMap, depthMap, cv::Size(imageWidth, imageHeight));
    }

    void initBackground()
    {
      static const GLfloat squareVertices[] = {
      -1.0f, -1.0f,
      1.0f,  1.0f,
      1.0f, -1.0f,
      -1.0f, -1.0f,
      -1.0f,  1.0f,
      1.0f,  1.0f};

      static const GLfloat texCoords[] = {
        1.0, 1.0,
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0, 
        1.0, 0.0,
        0.0, 0.0};

      glGenVertexArrays(1, &backgroundVAO);
      glBindVertexArray(backgroundVAO);

      glGenBuffers(1, &backgroundVertices);
      glBindBuffer(GL_ARRAY_BUFFER, backgroundVertices);
      glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

      glGenBuffers(1, &backgroundTexcoords);
      glBindBuffer(GL_ARRAY_BUFFER, backgroundTexcoords);
      glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
      glBindVertexArray(0);
    }

    void drawBackground()
    {
      // Turn off depth writes so background is always drawn behind
      glDepthMask(GL_FALSE);
      // Load background shader
      backgroundShader.use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, bgrTexture);
      glUniform1i(backgroundTexLocation, 0);

      glBindVertexArray(backgroundVAO);
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, backgroundVertices);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
      glEnableVertexAttribArray(1);
      glBindBuffer(GL_ARRAY_BUFFER, backgroundTexcoords);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glBindVertexArray(0);
      backgroundShader.unUse();
      glDepthMask(GL_TRUE);
    }

    void drawHand()
    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, windowWidth, windowHeight);

      // First draw the RGB image
      drawBackground();

      // Transformation matrices for the hand
      glm::mat4 sphereWVPs[NUM_SPHERES];
      glm::mat4 cylinderWVPs[NUM_CYLINDERS];
      glm::mat4 sphereWVs[NUM_SPHERES];
      glm::mat4 cylinderWVs[NUM_CYLINDERS];

      // Get the best particle from the swarm
      Particle p = swarm.getBestParticle();
      // And produce a hand model from it
      Hand h(p.getArray());
      // Add to arrays
      h.addToWVPArrays(sphereWVPs, cylinderWVPs, 0, windowPipeline);
      h.addToWVArrays(sphereWVs, cylinderWVs, 0, windowPipeline);

      // And then render the model using the mesh functions
      handShader.use();
      mesh.renderCylinders(NUM_CYLINDERS, cylinderWVPs, cylinderWVs);
      mesh.renderSpheres(NUM_SPHERES, sphereWVPs, sphereWVs);
      handShader.unUse();
    }

    // This function is the main work loop.
    // Returns true if we should run again, or false if
    // we should exit
    bool run()
    {
      if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
        return false;

      cv::Mat depthImage;
      cv::Mat bgrImage;

      processImages(bgrImage, depthImage);

      // Generate a number for our textureID's unique handle
      matToTexture(bgrTexture, bgrImage);
      matToTexture(depthTexture, depthImage);

      vector<double> scores;

      // If the users hand hasn't moved in to an acceptable 
      // position, keep waiting
      if (!started)
      {
        makeTiledRendering();
        scorer.setTexture(depthTexture);
        scores = scorer.calculateScores(swarm.getParticles());
        for (unsigned int i = 0; i < scores.size(); i++)
          if (scores[i] < 21.0f)
            started = true;
        // Draw hand as a hint of where user should be putting theirs
        drawHand();
      }
      else
      {
        for (unsigned int i = 0; i < swarmGenerations; i++)
        {
          // Disturb half of the particles
          if (!(i % 3))
          {
            // For half of the particles
            for (unsigned int j = 0; j < numTiles / 2; j++)
            {
              // Choose random particle
              unsigned int particle = rand() % numTiles;
              // Choose random joint (Add 7 so ignore global pos/orientation)
              unsigned int joint = (rand() % (NUM_PARAMETERS - 7)) + 7;

//              cout << "Particle: " << particle << " joint: " << joint << endl;
              swarm.getParticles()[particle].getArray()[joint] += (rand()/(double(RAND_MAX)/2) - 1) * M_PI/10;
            }
          }
          makeTiledRendering();
          // For some reason texture needs setting every
          // frame TODO Find out why!
          scorer.setTexture(depthTexture);
          scores = scorer.calculateScores(swarm.getParticles());
          swarm.updateSwarm(scores);
        }

        drawHand();
        swarm.resetScores(scores);
        previousFrame = depthImage.clone();
      }

      glfwSwapBuffers();
      // Update frame count
      frameCount++;
      return true;
    }

  private:
    Mesh mesh;
    Shader tileShader, handShader, backgroundShader;
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

    Pipeline windowPipeline, renderPipeline;

    GLuint bgrTexture, depthTexture;
    GLuint tileFBO, tileRB; 
    GLuint tsLocation, tprLocation, npLocation;

    GLuint backgroundVAO, backgroundTexcoords, backgroundVertices, backgroundTexLocation;
    cv::Mat se, previousFrame;

    unsigned int frameCount;

    // This is toggled when the hand moves into an acceptable
    // starting position
    bool started;
};

void shutDown(int returnCode)
{
  glfwTerminate();
  exit(returnCode);
}

int main(int argc, char** argv)
{
  // First job: Seed random numbers
  srand(time(NULL));

  int windowWidth = 640;
  int windowHeight = 480;

  // Initialise GLFW
  if (glfwInit() == GL_FALSE)
    shutDown(1);

  // select opengl version 
  glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 4);
  glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
  glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Get a glfw window
  if (glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 0, 24, 0, GLFW_WINDOW) == GL_FALSE)
    shutDown(1);
  glfwSetWindowTitle("Hand Renderer");

  int major, minor, rev;
  glfwGetGLVersion(&major, &minor, &rev);
  fprintf(stderr, "OpenGL version received: %d.%d.%d\n", major, minor, rev);

  if (glewInit() != GLEW_OK)
    shutDown(1);

  // Tidy up these command line args
  HandRenderer *app = new HandRenderer(atoi(argv[1]), atoi(argv[2]), 640, 480, argv[3], argv[4]);

  if (!app->init()) 
    shutDown(1);

  glfwEnable(GLFW_STICKY_KEYS);

  // Run the main loop until it returns false
  do {} while (app->run());

  delete app;
  // Shut down GLFW and exit
  shutDown(0);
}
