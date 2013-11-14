#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


#include <opencv2/opencv.hpp>

#include "pipeline.hpp"
#include "hand.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "glut_backend.hpp"

static const unsigned int WINDOW_WIDTH = 640;
static const unsigned int WINDOW_HEIGHT = 480;

class ModelViewer : public ICallbacks
{
  public:

    ModelViewer()
    {
      // Initialise the hand
      for (unsigned int i = 0; i < NUM_PARAMETERS; i++)
        handParams[i] = 0.0;

      handParams[0] = 45.0234;
      handParams[1] = 147.802;
      handParams[2] = 1080.53;
      handParams[3] = 0.269494;
      handParams[4] = -0.0946774;
      handParams[5] = 0.055982;
      handParams[6] = -0.9567;
      handParams[7] = -0.101737;
      handParams[8] = 0.2;
      handParams[9] = 0.2;
      handParams[10] = 0.1;
      handParams[11] = -0.0023682;
      handParams[12] = 0.2;
      handParams[13] = 0.1;
      handParams[14] = 0.456604;
      handParams[15] = 0.140723;
      handParams[16] = 0.2;
      handParams[17] = 0.1;
      handParams[18] = 0.2;
      handParams[19] = 0.292699;
      handParams[20] = 0.2;
      handParams[21] = 0.4;
      handParams[22] = 0.1;
      handParams[23] = 0.7;
      handParams[24] = -0.1;
      handParams[25] = 0.9;
      handParams[26] = -0.4;

    }

    ~ModelViewer()
    {
    }    

    bool init(string fileName)
    {
      float projVals[16] = {-1.6367,       0,       0,         0, 
                                  0, -2.1786,       0,         0, 
                             0.0005, -0.0170,  1.1111,    1.0000,
                                  0,       0,-277.778,         0};
      proj = glm::make_mat4(projVals);

      // Get meshes initialised
      mesh.init(0.5f, 50, 50, 0.5f, 1.0f, 20);

      // Load shaders from file
      handShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/hand.glslv");
      handShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/hand.glslf");
      // Compile and then link the shaders
      handShader.createAndLinkProgram();

      backgroundShader.loadFromFile(GL_VERTEX_SHADER, "./src/shaders/background.glslv");
      backgroundShader.loadFromFile(GL_FRAGMENT_SHADER, "./src/shaders/background.glslf");
      backgroundShader.createAndLinkProgram();
      backgroundTexLocation = backgroundShader.addUniform("textureSampler");

      glGenTextures(1, &backgroundTexture);

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
      for (unsigned int i = 0; i < depthImage.rows; i++)
        for (unsigned int j = 0; j < depthImage.cols; j++)
          if (depthImage.at<uint16_t>(i, j) > 0)
            skinImage.at<uint8_t>(i, j) = 255;

      matToTexture(backgroundTexture, skinImage);
      initBackground();

      return true;
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


    void run()
    {
        GLUTBackendRun(this);
    }

    void drawBackground()
    {
      // Turn off depth writes so background is always drawn behind
      glDepthMask(GL_FALSE);
      // Load background shader
      backgroundShader.use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, backgroundTexture);
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


    
    virtual void RenderSceneCB()
    {   
        // Clear all the GL stuff ready for rendering.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 sphereWVPs[NUM_SPHERES];
        glm::mat4 cylinderWVPs[NUM_CYLINDERS];
        glm::mat4 sphereWVs[NUM_SPHERES];
        glm::mat4 cylinderWVs[NUM_CYLINDERS];

        // Build a hand from parameters
        Hand h(handParams);
        // And add it to tile WVP arrays 
        h.addToWVPArrays(sphereWVPs, cylinderWVPs, 0, proj);
        h.addToWVArrays(sphereWVs, cylinderWVs, 0, proj);

        drawBackground();

        handShader.use();
        mesh.renderCylinders(
            NUM_CYLINDERS,
            cylinderWVPs, cylinderWVs);

        mesh.renderSpheres(
            NUM_SPHERES, 
            sphereWVPs, sphereWVs);

        handShader.unUse();

        glutSwapBuffers();
    }

    virtual void IdleCB()
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
      {
        if (!Hand::outOfBounds(key-65, handParams[key - 65] - 0.5))
        {
          handParams[key - 65] -= 0.5;
        }
      }
      // Orientation requires smaller adjustments
      else if (key >= 'D' && key <= 'G')
      {
        if (!Hand::outOfBounds(key-65, handParams[key - 65] - 0.1))
        {
          handParams[key - 65] -= 0.1;
        }
      }
      // Adjusting angles of fingers
      else if (key >= 'H' && key <= '[')
      {
        cout << key - 65 << endl;
        if (!Hand::outOfBounds(key-65, handParams[key - 65] - 0.1))
        {
          handParams[key - 65] -= 0.1;
        }
      }

      // Lower case letter pressed
      // Position
      else if (key >= 'a' && key <= 'c')
      {
        if (!Hand::outOfBounds(key-97, handParams[key - 97] + 0.5))
        {
          handParams[key - 97] += 0.5;
        }
      }
      // Orientation
      else if (key >= 'd' && key <= 'g')
      {
        if (!Hand::outOfBounds(key-97, handParams[key - 97] + 0.1))
        {
          handParams[key - 97] += 0.1;
        }
      }
      // Joint angles
      else if (key >= 'h' && key <= '{')
      {
        if (!Hand::outOfBounds(key-97, handParams[key - 97] + 0.1))
        {
          handParams[key - 97] += 0.1;
        }
      }
      // Now handle other key presses
      else
      {
        switch (key) {
          case 27:
            glutLeaveMainLoop();
            break;
          case '=':
            cout << "Params" << endl;
            for (int i = 0; i < NUM_PARAMETERS; i++)
              cout << "  position[" << i << "] = " << handParams[i] << ";" << endl;
            break;
          case '+':
            cout << "Params" << endl;
            for (int i = 0; i < NUM_PARAMETERS; i++)
              cout << "      handParams[" << i << "] = " << handParams[i] << ";" << endl;
            break;


        }
      }

      glm::quat quat(handParams[3], handParams[4], handParams[5], handParams[6]);
      quat = glm::normalize(quat);

      cout << quat.x << " " << quat.y << " " << quat.z << " " << quat.w << endl;

      handParams[3] = quat.w;
      handParams[4] = quat.x;
      handParams[5] = quat.y;
      handParams[6] = quat.z;

      Hand h(handParams);
      h.printPosition();
    }

    virtual void PassiveMouseCB(int x, int y)
    {
    }
    
    virtual void MouseCB(int Button, int State, int x, int y)
    {
    }

private:
    glm::mat4 proj;
    Mesh mesh;
    Shader handShader, backgroundShader;
    GLuint backgroundVAO, backgroundTexcoords, backgroundVertices, backgroundTexLocation, backgroundTexture;
    double handParams[NUM_PARAMETERS];
};

int main(int argc, char** argv)
{
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Model Viewer")) {
        return 1;
    }
    
    ModelViewer* app = new ModelViewer();

    if (!app->init(argv[1])) {
        return 1;
    }
    
    app->run();
    delete app;
    return 0;
}
