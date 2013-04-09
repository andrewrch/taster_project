#ifndef SCORER_H
#define SCORER_H

#define __CL_ENABLE_EXCEPTIONS

#include <vector>
#include <string>
#include <CL/cl.hpp>

#include "particle.hpp"

class Scorer {
  public:
    //default constructor initializes OpenCL context 
    Scorer(unsigned int, double, double, unsigned int, unsigned int, unsigned int, unsigned int);
    //default destructor releases OpenCL objects and frees device memory
    ~Scorer();
    //load an OpenCL program from a given filename
    void loadProgram(const std::string&);
    // Setup the data for the kernel (I.e. the renderbuffer
    // and the depth texture)
    void loadData(GLuint, GLuint, GLuint);
    // This does the real work - i.e. runs the kernel and accumulates
    // scores for all renderings
    std::vector<double>& calculateScores(std::vector<Particle>&);

    // Updates the texture when OpenGL has updated it
    void setObservations(GLuint, GLuint);

  private:
    void loadProgramFromString(const std::string&);
    double getCollisionPenalty(Particle&);

    // Buffers and GL objects for OpenCL
    std::vector<cl::Memory> clObjects;  // 0: renderbuffer, 1: skin texture, 2: depth texture
    cl::Buffer differenceBuffer; 
    cl::Buffer unionBuffer; 
    cl::Buffer intersectionBuffer;

    // Holds the actual scores
    std::vector<double> finalScores;

    // Clamping values to penalise badly segmented depth image
    unsigned int dm, dM;
    double lambda, lambdak;
    unsigned int numScores;    //the number of tiles to score

    // Size of the depth image
    unsigned int imageWidth, imageHeight;

    // Max work items the GPU can handle per group
    unsigned int maxWorkGroupSize, maxWorkGroupWidth; 

    // Member vars for OpenCL gubbins
    size_t arraySize;
    unsigned int deviceUsed;
    std::vector<cl::Device> devices;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    cl::Kernel kernel;

    // Range for the kernel
    cl::NDRange globalRange, localRange;

    //debugging variables
    cl_int err;
    /// Event to wait on when executing certain tasks
    cl::Event event;
    // Events for read
    std::vector<cl::Event> readEvents;
};

#endif
