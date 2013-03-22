#ifndef PARTICLE_SWARM_H
#define PARTICLE_SWARM_H

#include <vector>
#include "particle.hpp"

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
