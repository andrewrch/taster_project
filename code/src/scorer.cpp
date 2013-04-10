#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <GL/glx.h>
#include "scorer.hpp"

using namespace std;

const char* oclErrorString(cl_int);

Scorer::Scorer(
    unsigned int scores, 
    double l, 
    double lk, 
    unsigned int _dm, 
    unsigned int _dM, 
    unsigned int width, 
    unsigned int height) : 
  clObjects(3),
  finalScores(scores),
  dm(_dm),
  dM(_dM),
  lambda(l),
  lambdak(lk),
  numScores(scores),
  imageWidth(width),
  imageHeight(height),
  readEvents(3)
{
  //setup devices and context
  std::vector<cl::Platform> platforms;
  err = cl::Platform::get(&platforms);
  deviceUsed = 0;
  err = platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
  int t = devices.front().getInfo<CL_DEVICE_TYPE>();
  maxWorkGroupSize = devices.front().getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
  printf("Device type: %d CL_DEVICE_TYPE_GPU: %d \n", t, CL_DEVICE_TYPE_GPU);
  printf("Max work group size on GPU: %u\n", maxWorkGroupSize);

  // Then calculate the dimensions of the 2D work group
  maxWorkGroupWidth = (unsigned int) floor(sqrt(maxWorkGroupSize));
  // Calculate local and global range of the kernel given max work group size
  globalRange = cl::NDRange(sqrt(numScores) * maxWorkGroupWidth, 
                            sqrt(numScores) * maxWorkGroupWidth);
  localRange = cl::NDRange(maxWorkGroupWidth, maxWorkGroupWidth);

  // Linux specific
  cl_context_properties props[] =
  {
      CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
      CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
      CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
      0
  };

  try{
      context = cl::Context(CL_DEVICE_TYPE_GPU, props);
  }
  catch (cl::Error er) {
      printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
  }

  //create the command queue we will use to execute OpenCL commands
  try{
      queue = cl::CommandQueue(context, devices[deviceUsed], 0, &err);
  }
  catch (cl::Error er) {
      printf("ERROR: %s(%d)\n", er.what(), er.err());
  }

  //create the OpenCL only arrays
  arraySize = numScores * sizeof(unsigned int);
  differenceBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, arraySize, NULL, &err);
  unionBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, arraySize, NULL, &err);
  intersectionBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, arraySize, NULL, &err);

}

Scorer::~Scorer() {}

void Scorer::loadProgram(const string& filename){
  ifstream fp;
  fp.open(filename.c_str(), ios_base::in);
  if(fp) 
  {    
    string buffer(
        std::istreambuf_iterator<char>(fp), 
        (std::istreambuf_iterator<char>()));
    //copy to source
    loadProgramFromString(buffer);   
  } 
  else 
    cerr<<"Error loading kernel: "<<filename<<endl;
}

void Scorer::loadProgramFromString(const string& kernel_source)
{
    // Program Setup
    int pl;
    printf("load the program\n");
    
    pl = kernel_source.size();
    printf("kernel size: %d\n", pl);
    try
    {
        cl::Program::Sources source(1,
            std::make_pair(kernel_source.c_str(), pl));
        program = cl::Program(context, source);
    }
    catch (cl::Error er) {
        printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
    }

    printf("build program\n");
    try
    {
        err = program.build(devices);
    }
    catch (cl::Error er) {
        printf("program.build: %s\n", oclErrorString(er.err()));
    }
    printf("done building program\n");
	  std::cout << "Build Status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
  	std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
  	std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;

    //initialize our kernel from the program
    try{
      kernel = cl::Kernel(program, "calculateImageCharacteristics", &err);
    }
    catch (cl::Error er) {
      printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
    }

}

void Scorer::loadData(GLuint rbo, GLuint skin, GLuint depth)
{
    //make sure OpenGL is finished before we proceed
    glFinish();
    // create OpenCL buffer from GL VBO
    try {
      clObjects[0] = cl::BufferRenderGL(context, CL_MEM_READ_ONLY, rbo, &err);
    } catch (cl::Error er) {
      printf("Renderbuffer 1 fail\n");
      cout << er.what() << " " << er.err() << endl;
    }
    try{
      clObjects[1] = cl::ImageGL(context, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, skin, &err);
      clObjects[2] = cl::ImageGL(context, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, depth, &err);
    }
    catch (cl::Error er) {
      cout << "Texture fail" << endl;
      cout << er.what() << " " << er.err() << endl;
      printf("v_vbo: %s\n", oclErrorString(err)); 
    }

    //set the arguments of our kernel
    try
    {
      err = kernel.setArg(0, clObjects[0]); // The renderbuffer
      err = kernel.setArg(1, clObjects[1]); // The skin texture
      err = kernel.setArg(2, clObjects[2]); // The depth texture
      err = kernel.setArg(3, differenceBuffer);
      err = kernel.setArg(4, unionBuffer);
      err = kernel.setArg(5, intersectionBuffer);
      err = kernel.setArg(6, dm);
      err = kernel.setArg(7, dM);
      err = kernel.setArg(8, (unsigned int) floor(sqrt(numScores)));
      err = kernel.setArg(9, imageWidth);
      err = kernel.setArg(10, imageHeight);
      err = kernel.setArg(11, maxWorkGroupSize * sizeof(cl_uint), NULL);
      err = kernel.setArg(12, maxWorkGroupSize * sizeof(cl_uint), NULL);
      err = kernel.setArg(13, maxWorkGroupSize * sizeof(cl_uint), NULL);
    }
    catch (cl::Error er) {
      printf("ERROR: %s(%s)\n", er.what(), oclErrorString(er.err()));
    }
}

void Scorer::setObservations(GLuint skin, GLuint depth)
{
  try{
    clObjects[1] = cl::ImageGL(context, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, skin, &err);
    clObjects[2] = cl::ImageGL(context, CL_MEM_READ_ONLY, GL_TEXTURE_2D, 0, depth, &err);
  }
  catch (cl::Error er) {
    cout << "Texture fail" << endl;
    cout << er.what() << " " << er.err() << endl;
    printf("v_vbo: %s\n", oclErrorString(err)); 
  }
  err = kernel.setArg(1, clObjects[1]); // The skin texture
  err = kernel.setArg(2, clObjects[2]); // The depth texture
}

// Update this function - should be somewhere else!
double Scorer::getCollisionPenalty(Particle& p)
{
  double* params = p.getArray();
  double  penalty = 0.0f;

  // For each of the 3 pairs of fingers...
  for (int i = 0; i < 3; i++)
  {
    double j = params[4*(i + 1)+3] - params[4 * (i + 2)+3];
    penalty -= min<double>(j, 0.0);
  }

  return penalty;
}

std::vector<double>& Scorer::calculateScores(std::vector<Particle>& particles)
{
  // This will calculate the scores for each particle in swarm
  unsigned int differenceSum[numScores];
  unsigned int unionSum[numScores];
  unsigned int intersectionSum[numScores];
  glFinish();

  try{
    err = queue.enqueueAcquireGLObjects(&clObjects, NULL, &event);
  } catch (cl::Error er) { cout << er.what() << " " << er.err() << endl; }

  try {
    err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalRange, localRange, NULL, &event);
  } catch (cl::Error er) { cout << er.what() << " " << er.err() << endl; }
  event.wait();

  queue.enqueueReadBuffer(differenceBuffer, CL_TRUE, 0, arraySize, &differenceSum, NULL, &readEvents[0]);
  queue.enqueueReadBuffer(unionBuffer, CL_TRUE, 0, arraySize, &unionSum, NULL, &readEvents[1]);
  queue.enqueueReadBuffer(intersectionBuffer, CL_TRUE, 0, arraySize, &intersectionSum, NULL, &readEvents[2]);
  cl::Event::waitForEvents(readEvents);

  // Calculate scores using formula from paper...
  for (unsigned int i = 0; i < numScores; i++)
  {
    double a = (double) differenceSum[i] / (unionSum[i] + 0.000001f);
    double b = (1 - ((double) (2 * intersectionSum[i]) / (intersectionSum[i] + unionSum[i])));
    finalScores[i] = (a + lambda * b)  + lambdak * getCollisionPenalty(particles[i]);
  }

//  for (int i = 0; i < numScores; i++)
//    cout << finalScores[i] << "  ";
//  cout << endl;
//
  //Release the VBOs so OpenGL can play with them
  err = queue.enqueueReleaseGLObjects(&clObjects, NULL, &event);
  event.wait();
  return finalScores;
}

// Function to display string for OpenCL error code
const char* oclErrorString(cl_int error)
{
    static const char* errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    return (index >= 0 && index < errorCount) ? errorString[index] : "";

};

