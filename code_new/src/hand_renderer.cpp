#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <opencv2/opencv.hpp>

#include "hand.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "particleswarm.hpp"
#include "scorer.hpp"

static const unsigned int WINDOW_WIDTH = 640;
static const unsigned int WINDOW_HEIGHT = 480;

static const double c1 = 2.8;
static const double c2 = 1.3;

using namespace std;
namespace fs = boost::filesystem;

class HandRenderer
{
  public:
    HandRenderer(
        unsigned int particles, 
        unsigned int generations,
        unsigned int width,
        unsigned int height) :
      swarm(particles, NUM_PARAMETERS, c1, c2), 
      scorer(particles, 10.0, 10.0, 10, 40, width, height),
      imageWidth(width),
      imageHeight(height),
      windowWidth(WINDOW_WIDTH),
      windowHeight(WINDOW_HEIGHT),
      renderWidth(sqrt(particles) * width),
      renderHeight(sqrt(particles) * height),
      numTiles(particles),
      swarmGenerations(generations),
      started(false)
    {
    }

    ~HandRenderer() {
      glFinish();
      glDeleteFramebuffers(1,&tileFBO);
      glDeleteRenderbuffers(1,&tileRB);
    	glDeleteTextures(1, &skinTexture);
    	glDeleteTextures(1, &depthTexture);
    }   

    // Function turn a cv::Mat into a texture
    void matToTexture(GLuint textureID, cv::Mat& mat)
    {
      // Bind to our texture handle
      glBindTexture(GL_TEXTURE_2D, textureID);
      // Set texture interpolation methods for minification and magnification
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
      glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
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
        if (mat.depth() == CV_8U)
        {
          inputColourFormat = GL_RED_INTEGER;
          inputType = GL_UNSIGNED_BYTE;
          internalFormat = GL_RGBA8UI;
        }
        else
        {
          inputColourFormat = GL_RED_INTEGER;
          inputType = GL_UNSIGNED_SHORT;
          internalFormat = GL_RGBA16UI;
        }
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

    bool init()
    {
      // Hard code projection using values from Oik
      //float projVals[16] = {-1.6367,       0,  0.0005,         0, 
      //                            0, -2.1786, -0.0170,         0, 
      //                            0,       0,  1.1111, -277.7778,
      //                            0,       0,  1.0000,         0};

      // This is the perspective projection matrix given by Iasonas
      float projVals[16] = {-1.6367,       0,       0,         0, 
                                  0, -2.1786,       0,         0, 
                             0.0005, -0.0170,  1.1111,    1.0000,
                                  0,       0,-277.778,         0};
      displayProj = glm::make_mat4(projVals);

      // Copy this in to tileProj (This will eventually be modified to 
      // scale/zoom for segmented hand area
      tileProj = displayProj;

      //glm::mat4 translateTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.2, -0, 0));
      //glm::mat4 scaleTrans = glm::scale(glm::mat4(1.0f), glm::vec3(3, 3, 1.0f));
      //proj = scaleTrans * translateTrans * proj;
      
      // OpenGL initialisation
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//      glFrontFace(GL_CW);
//      glCullFace(GL_BACK);
//      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);

      // Get meshes initialised
      //
      // Seperate this out as a sphere and cylinder class, and have
      // a single render function for each
      mesh.init(0.5f, 7, 7, 0.5f, 1.0f, 8);

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
        cerr << "FrameBuffer is not complete" << endl;

      // Generate textures
      glGenTextures(1, &depthTexture);
      glGenTextures(1, &skinTexture);

      // Initialise skin and depth textures to blank images (Required for CL interop)
      cv::Mat blank = cv::Mat::zeros(cv::Size(imageWidth, imageHeight), CV_8UC1);
      matToTexture(skinTexture, blank);
      blank = cv::Mat::zeros(cv::Size(imageWidth, imageHeight), CV_16UC1);
      matToTexture(depthTexture, blank);

      initBackground();

      // OpenCL interop stuff here
      scorer.loadProgram("./src/kernels/distancekernel.cl");
      scorer.loadData(tileRB, skinTexture, depthTexture);

      prevSkin = cv::Mat(cv::Size(imageWidth, imageHeight), CV_8UC1);
      prevDepth = cv::Mat(cv::Size(imageWidth, imageHeight), CV_16UC1);
      return true;
    }

    void makeTiledRendering()
    {
      glm::mat4 sphereWVPs[NUM_SPHERES * numTiles];
      glm::mat4 cylinderWVPs[NUM_CYLINDERS * numTiles];

      glm::mat4 sphereWVs[NUM_SPHERES * numTiles];
      glm::mat4 cylinderWVs[NUM_CYLINDERS * numTiles];

      vector<Particle>& particles = swarm.getParticles();

      glm::mat4 tileTransformations[numTiles];

      glm::mat4 scaleTrans, translateTrans;
      int x = sqrt(numTiles);
      double scale = 1.0 / x;
      scaleTrans = glm::scale(scaleTrans, glm::vec3(scale, scale, 1.0f));
      double xTrans, yTrans, zTrans;
      for (unsigned int i = 0; i < numTiles; i++)
      {
        xTrans = -1.0f + (2.0 * scale * (i % x)) + scale;
        yTrans = -1.0f + (2.0 * scale * (i / x)) + scale;
        zTrans = 0.0f;
        translateTrans = glm::translate(glm::mat4(1.0f), glm::vec3(xTrans, yTrans, zTrans));
        tileTransformations[i] = translateTrans * scaleTrans * tileProj;
      }

      for (unsigned int i = 0; i < particles.size(); i++)
      {
        // For each particle in swarm build a hand
        Hand h(particles[i].getArray());
        // And add it to tile WVP arrays 
        h.addToTileArrays(sphereWVPs, cylinderWVPs, i, tileTransformations[i] );
        h.addToWVArrays(sphereWVs, cylinderWVs, i, tileTransformations[i] );
      }

      //Before drawing
      glBindFramebuffer(GL_FRAMEBUFFER, tileFBO);
      glViewport(0, 0, renderWidth, renderHeight);
      // Clear all the GL stuff ready for rendering.
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      tileShader.use();

//      int x = sqrt(numTiles);
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
//
//        cv::waitKey(2000);
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
      glBindTexture(GL_TEXTURE_2D, skinTexture);
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

      //for (int i = 0; i < NUM_PARAMETERS; i++)
      //{
      //  cout << "  position[" << i << "] = " << p.getArray()[i] << ";" << endl;
      //}

      // Add to arrays
      h.addToWVPArrays(sphereWVPs, cylinderWVPs, 0, displayProj);
      h.addToWVArrays(sphereWVs, cylinderWVs, 0, displayProj);
      //h.printPosition();

      // And then render the model using the mesh functions
      handShader.use();
      mesh.renderCylinders(NUM_CYLINDERS, cylinderWVPs, cylinderWVs);
      mesh.renderSpheres(NUM_SPHERES, sphereWVPs, sphereWVs);
      handShader.unUse();
    }

    // This function is the main work loop.
    // Returns true if we should run again, or false if
    // we should exit
    bool run(string fileName)
    {
      if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
        return false;

      cv::Mat depthImage, skinImage;
      // Now read image from directory and create skin image
      depthImage = cv::imread(fileName, -1);

      if (!depthImage.data)
      {
        cout << "Could not open image file: " << fileName << endl;
        return false;
      }

      // set skin image to 1 if depth image is non-zero at some pixel
      skinImage = cv::Mat::zeros(depthImage.size(), CV_8UC1);
      //cout << "rows: " << depthImage.rows << " cols: " << depthImage.cols << endl;
      //cout << "Depth: " << depthImage.depth() << endl;
      //cout << "Channels: " << depthImage.channels() << endl;
      for (int i = 0; i < depthImage.rows; i++)
        for (int j = 0; j < depthImage.cols; j++)
          if (depthImage.at<uint16_t>(i, j) > 0)
            skinImage.at<uint8_t>(i, j) = 255;

      // Now find the bounds of the hand in the skin image
      int maxX = 0, maxY = 0, minX = INT_MAX, minY = INT_MAX;
      for (int i = 0; i < skinImage.rows; i++)
        for (int j = 0; j < skinImage.cols; j++)
          if (skinImage.at<uint8_t>(i, j) == 255)
          {
            if (i < minY) minY = i;
            if (i > maxY) maxY = i;
            if (j < minX) minX = j;
            if (j > maxX) maxX = j;
          }
      cout << minY << " " << maxY << " " << minX << " " << maxX << endl;

      cv::Mat hand = skinImage(cv::Rect(minX, minY, maxX - minX, maxY - minY));
      //skinImage = cv::Mat(cv::Size(128, 128), CV_8U);
      cv::resize(hand, skinImage, cv::Size(128, 128));
      imshow("skin", skinImage);

      skinImage.copyTo(prevSkin);
      depthImage.copyTo(prevDepth);

      // Generate a number for our textureID's unique handle
      matToTexture(skinTexture, skinImage);
      matToTexture(depthTexture, depthImage);

      vector<double> scores;
      for (unsigned int i = 0; i < swarmGenerations; i++)
      {
        // Disturb half of the particles
        if (!((i+1) % 3))
        {
          swarm.shuffle();
        }
        makeTiledRendering();
        // For some reason texture needs setting every
        // frame TODO Find out why!
        scorer.setObservations(skinTexture, depthTexture);
        scores = scorer.calculateScores(swarm.getParticles());
        swarm.updateSwarm(scores);
      }

      drawHand();
      swarm.resetScores(scores);

      glfwSwapBuffers();
      return true;
    }

  private:
    Mesh mesh;
    Shader tileShader, handShader, backgroundShader;
    ParticleSwarm swarm;
    Scorer scorer;

    unsigned int imageWidth, imageHeight, \
                 windowWidth, windowHeight, \
                 renderWidth, renderHeight, \
                 numTiles, swarmGenerations;

    GLuint depthTexture, skinTexture;
    GLuint tileFBO, tileRB; 
    GLuint tsLocation, tprLocation, npLocation;

    GLuint backgroundVAO, backgroundTexcoords, backgroundVertices, backgroundTexLocation;
    cv::Mat se, prevDepth, prevSkin;

    glm::mat4 displayProj, tileProj;

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
  printf("OpenGL version received: %d.%d.%d\n", major, minor, rev);

  if (glewInit() != GLEW_OK)
    shutDown(1);

  HandRenderer *app = new HandRenderer(
      atoi(argv[1]), 
      atoi(argv[2]), 
      atoi(argv[3]), 
      atoi(argv[4]));

  if (!app->init()) 
    shutDown(1);

  glfwEnable(GLFW_STICKY_KEYS);

  // Open image directory
  fs::path full_path( fs::initial_path<fs::path>() );
  full_path = fs::system_complete( fs::path( argv[5] ) );

  typedef vector<fs::path> vec;             // store paths,
  vec v;                                // so we can sort them later

  copy(fs::directory_iterator(full_path), fs::directory_iterator(), back_inserter(v));
  sort(v.begin(), v.end());             // sort, since directory iteration
                                        // is not ordered on some file systems
  
  bool running = true;
  for (vec::const_iterator it (v.begin()); it != v.end() && running; ++it)
  {
        string filename((*it).string());
        if (!filename.compare(filename.length() - 3, 3, "png"))
        {
          cout << "Filename OK" << endl;
          running = app->run((*it).string());
        }
    }

  delete app;
  // Shut down GLFW and exit
  shutDown(0);
}
