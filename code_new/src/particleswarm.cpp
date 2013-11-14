#include <float.h>
#include <math.h>
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
  bestParticle(parameters, c1, c2, *this),
  particles(0, Particle(parameters, c1, c2, *this))
{
  particles.reserve(numParticles);

  // First particle is initialised at starting position
  particles.push_back(Particle(parameters, c1, c2, *this));
  // Initialise the rest of the particles to perturbations
  // around that position
  for (unsigned int i = 1; i < numParticles; i++)
    particles.push_back(Particle(parameters, c1, c2, particles[0].getPerturbation(), *this));
}

Particle& ParticleSwarm::getNearestParticle(Particle p)
{
  double bestDistance = DBL_MAX, tempDistance;
  unsigned int bestParticle = 0;
  for (unsigned int i = 0; i < particles.size(); i++)
  {
    tempDistance = 0;
    bool outOfBounds = false;
    for (unsigned int param = 0; param < p.getNumParams(); param++)
    {
      tempDistance += pow(fabs(p.getArray()[param] - particles[i].getArray()[param]),2);
      if (Hand::outOfBounds(param, particles[i].getArray()[param]))
      {
        outOfBounds = true;
      }
    }

    tempDistance = sqrt(tempDistance);

    if ((tempDistance < bestDistance) && !outOfBounds)
    {
      bestDistance = tempDistance;
      bestParticle = i;
    }
  }
    
  return particles[bestParticle];
}

double ParticleSwarm::getNearestParticleInDimension(unsigned int dimension, double value)
{
  double bestDistance = DBL_MAX, bestValue;
  unsigned int bestParticle = 0;
  for (unsigned int i = 0; i < particles.size(); i++)
  {
    bool outOfBounds = Hand::outOfBounds(dimension, particles[i].getArray()[dimension]);
    double distance = fabs(value - particles[i].getArray()[dimension]);

    if ((distance < bestDistance) && !outOfBounds)
    {
      bestDistance = distance;
      bestParticle = i;
      bestValue = particles[i].getArray()[dimension];
    }
  }

  return bestValue;
}

void ParticleSwarm::shuffle()
{
  vector<unsigned int> particleIndexes;
  for (int i = 0; i < particles.size(); i++)
    particleIndexes.push_back(i);

  random_shuffle ( particleIndexes.begin(), particleIndexes.end() );

  // For half of the particles
  for (unsigned int i = 0; i < particles.size() / 2; i++)
  {
    particles[particleIndexes[i]].shuffle();
  }
}

void ParticleSwarm::updateSwarm(vector<double> scores)
{
  for (unsigned int i = 0; i < scores.size(); i++)
  {
    if (scores[i] < bestScore)
    {
      bestScore = scores[i];
      bestParticle = particles[i];
    }
  }

  for (unsigned int i = 0; i < particles.size(); i++)
    particles[i].update(scores[i]);

  
  //for (unsigned int i = 0; i < particles.size(); i++)
  //  cout << particles[i].getArray()[4] << endl;
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

  bestParticle = particles[bestP];
  bestScore = DBL_MAX;
  for (unsigned int i = 0; i < particles.size(); i++)
    if (i != bestP)
    {
      particles[i].position = bestParticle.getPerturbation();
      particles[i].bestPosition = particles[i].position;
      particles[i].resetScore();
    }
    else
      particles[i].resetScore();
}

