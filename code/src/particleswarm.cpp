#include "particleswarm.hpp"
#include <stdio.h>

using namespace std;

ParticleSwarm::ParticleSwarm(
    unsigned int numParticles,
    unsigned int parameters,
    double c1,
    double c2) :
  bestScore(0.0),
  bestParticle(parameters, c1, c2),
  particles(0, Particle(parameters, c1, c2))
{
  particles.reserve(numParticles);
  for (int i = 0; i < numParticles; i++)
    particles.push_back(Particle(parameters, c1, c2));
}

void ParticleSwarm::updateSwarm(vector<double> scores)
{
  for (unsigned int i = 0; i < scores.size(); i++)
    if (scores[i] > bestScore)
    {
      bestScore = scores[i];
      bestParticle = particles[i];
    }

  for (unsigned int i = 0; i < particles.size(); i++)
    particles[i].update(scores[i], bestParticle);
}

void ParticleSwarm::resetScores()
{ 
  bestScore = 0.0;
  for (unsigned int i = 0; i < particles.size(); i++)
    particles[i].resetScore();
}

