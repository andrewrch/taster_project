#include <float.h>
#include <stdio.h>
#include <algorithm>

#include "particleswarm.hpp"

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
  for (unsigned int i = 0; i < numParticles; i++)
    particles.push_back(Particle(parameters, c1, c2));
}

void ParticleSwarm::updateSwarm(vector<double> scores)
{
  for (unsigned int i = 0; i < scores.size(); i++)
    if (scores[i] < bestScore)
    {
      bestScore = scores[i];
      bestParticle = particles[i];
    }

  for (unsigned int i = 0; i < particles.size(); i++)
    particles[i].update(scores[i], bestParticle);
}

void ParticleSwarm::resetScores(vector<double> scores)
{ 
  unsigned int bestP = 0;
  double bestS = DBL_MAX;
  for (unsigned int i = 0; i < scores.size(); i++)
  {
    if (scores[i] < bestS)
    {
      bestS = scores[i];
      bestP = i;
    }
  }

  bestScore = DBL_MAX;
  for (unsigned int i = 0; i < particles.size(); i++)
    if (i != bestP)
      particles[i] = particles[bestP].getPerturbation();
    else
      particles[i].resetScore();
}

