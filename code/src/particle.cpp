#include <cmath>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "particle.hpp"

using namespace std;

Particle::Particle(unsigned int p, double C1, double C2) :
  position(p),
  velocity(p),
  bestScore(0.0),
  bestPosition(p),
  c1(C1),
  c2(C2)
{
  double phi = c1 + c2;
  k = 2./abs(2 - phi - sqrt(phi*phi - 4 * phi));

  // Now initialise the particle
  for (unsigned int i = 0; i < p; i++)
  {
    // Velocity always starts as 0
    velocity[i] = 0;
    position[i] = rand()/double(RAND_MAX) * 30; 
    printf("%f\n", rand()/double(RAND_MAX) * 30);
  }

  position[0] = rand()/double(RAND_MAX) * 10; 
  position[1] = rand()/double(RAND_MAX) * 10; 
  position[2] = rand()/double(RAND_MAX) * 10; 
  position[3] = rand()/double(RAND_MAX); 
  position[4] = rand()/double(RAND_MAX); 
  position[5] = rand()/double(RAND_MAX); 
  position[6] = rand()/double(RAND_MAX); 
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
  if (score > bestScore)
  {
    bestScore = score;
    bestPosition = position;
  }

  double r1 = rand() / double(RAND_MAX);
  double r2 = rand() / double(RAND_MAX);
  double c1r1 = c1 * r1;
  double c2r2 = c2 * r2;

  for (unsigned int i = 0; i < velocity.size(); i++)
  {
    velocity[i] = k * (velocity[i] + c1r1 * (bestPosition[i] - position[i]) + \
                                     c2r2 * (best.getArray()[i] - position[i]));

    position[i] = position[i] + velocity[i];
  }
}
