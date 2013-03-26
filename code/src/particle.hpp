#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <float.h>

class Particle
{
  public:
    Particle(unsigned int, double, double);
    ~Particle() {};

    // So we can easily update particles
    Particle& operator=(const Particle&);

    // Gets the particle as an array of doubles
    inline double* getArray() { return &position[0]; };

    // Updates the particle given its score, and the
    // best particle in the swarm
    void update(double score, Particle);

    // So swarm can be reused each frame, remember we're
    // trying to minimise score so reset to large number
    void resetScore() { bestScore = DBL_MAX; };

    // Returns a perturbation of this particle.
    Particle getPerturbation();

  private:
    // Holds all information about current particle
    std::vector<double> position;
    std::vector<double> velocity;

    // Keep track of best score for updating best position
    double bestScore;
    std::vector<double> bestPosition;

    // Used for calculating new velocity
    double k, c1, c2;

    // Number of parameters the particle has
    unsigned int numParameters;
};

#endif
