#include <cmath>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include "particle.hpp"

using namespace std;

Particle::Particle(unsigned int p, double C1, double C2) :
  position(p),
  velocity(p),
  bestScore(DBL_MAX),
  bestPosition(p),
  c1(C1),
  c2(C2),
  numParameters(p)
{
  double phi = c1 + c2;
  k = 2./abs(2 - phi - sqrt(phi*phi - 4 * phi));

  // Now initialise the particle
  for (unsigned int i = 0; i < p; i++)
  {
    // Velocity always starts as 0
    velocity[i] = 0;
    position[i] = 0;
  }
}

Particle Particle::getPerturbation()
{
  Particle pert(position.size(), c1, c2);
  for (int i = 0; i < 3; i++)
    pert.position[i] = position[i] + (rand()/(double(RAND_MAX)/2) - 1) * 10;

  for (int i = 3; i < 7; i++)
    pert.position[i] = position[i] + (rand()/(double(RAND_MAX)/2) - 1) * M_PI/10;

  for (unsigned int i = 7; i < numParameters; i++)
    pert.position[i] = position[i] + (rand()/(double(RAND_MAX)/2) - 1) * M_PI/10;
  return pert;
}

Particle& Particle::operator=(const Particle& p)
{
  position = p.position;
  velocity = p.velocity;
  bestScore = p.bestScore;
  bestPosition = p.bestPosition;
  c1 = p.c1;
  c2 = p.c2;
  k = p.k;

  return *this;
}

void Particle::update(double score, Particle best)
{
  if (score < bestScore)
  {
    bestScore = score;
    bestPosition = position;
  }

  double r1, r2, c1r1, c2r2;

    r1 = rand() / double(RAND_MAX);
    r2 = rand() / double(RAND_MAX);
    c1r1 = c1 * r1;
    c2r2 = c2 * r2;
  for (unsigned int i = 0; i < numParameters; i++)
  {
    velocity[i] = k * (velocity[i] + c1r1 * (bestPosition[i] - position[i]) + \
                                     c2r2 * (best.getArray()[i] - position[i]));

    position[i] = position[i] + velocity[i];
  }
}
