#ifndef PARTICLE_SWARM_H
#define PARTICLE_SWARM_H

#include <vector>
#include <float.h>
#include <random>
#include "hand.hpp"

class ParticleSwarm;

class Particle
{
  friend class ParticleSwarm;
  public:
    Particle(unsigned int, double, double, ParticleSwarm&);
    Particle(unsigned int, double, double, std::vector<double>, ParticleSwarm&);
    ~Particle() {};

    // So we can easily update particles
    Particle& operator=(const Particle&);

    // Gets the particle as an array of doubles
    inline double* getArray() { return &position[0]; };
    unsigned int getNumParams() { return numParameters; };

    // Updates the particle given its score, and the
    // best particle in the swarm
    void update(double score);
    void shuffle();

    // So swarm can be reused each frame, remember we're
    // trying to minimise score so reset to large number
    void resetScore() ;

    // Returns a perturbation of this particle.
    std::vector<double> getPerturbation();

  private:
    void normaliseQuarternion(std::vector<double>&);
    ParticleSwarm& swarm;
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

    // Random number generator
    std::default_random_engine generator;
};

class ParticleSwarm
{
  public:
    ParticleSwarm(
        unsigned int,     // Number of particles
        unsigned int,     // Parameters per particle
        double,           // c1
        double);          // c2
    ~ParticleSwarm() {};

    // Update with vector of scores
    void updateSwarm(std::vector<double>);
    Particle& getBestParticle() { return bestParticle; };
    std::vector<Particle>& getParticles() { return particles; };
    Particle& getNearestParticle(Particle p);
    double getNearestParticleInDimension(unsigned int, double);
    void shuffle();

    // So we can reuse particle swarm from previous
    // generation
    void resetScores(std::vector<double>);

  private:
    double bestScore;
    // Keep a record of best particle
    Particle bestParticle;
    std::vector<Particle> particles;
};

#endif
