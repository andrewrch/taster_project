#include <cmath>
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <glm/gtx/quaternion.hpp>

#include "particleswarm.hpp"

using namespace std;

Particle::Particle(unsigned int p, double C1, double C2, ParticleSwarm& s) :
  swarm(s),
  position(p),
  velocity(p),
  bestScore(DBL_MAX),
  bestPosition(p),
  c1(C1),
  c2(C2),
  numParameters(p)
{
  double phi = c1 + c2;
  k = 2./fabs(2 - phi - sqrt(phi*phi - 4 * phi));

  // Now initialise the particle
  for (unsigned int i = 0; i < p; i++)
  {
    // Velocity always starts as 0
    velocity[i] = 0;
    //position[i] = 0;
    //bestPosition[i] = 0;
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = default_random_engine(seed);

  bestPosition[0] = position[0] = 45.0234;
  bestPosition[1] = position[1] = 147.802;
  bestPosition[2] = position[2] = 1080.53;
  bestPosition[3] = position[3] = 0.269494;
  bestPosition[4] = position[4] = -0.0946774;
  bestPosition[5] = position[5] = 0.055982;
  bestPosition[6] = position[6] = -0.9567;
  bestPosition[7] = position[7] = -0.101737;
  bestPosition[8] = position[8] = 0.2;
  bestPosition[9] = position[9] = 0.3;
  bestPosition[10] =position[10] = 0.1;
  bestPosition[11] =position[11] = -0.0023682;
  bestPosition[12] =position[12] = 0.3;
  bestPosition[13] =position[13] = 2.77556e-17;
  bestPosition[14] =position[14] = 0.456604;
  bestPosition[15] =position[15] = 0.140723;
  bestPosition[16] =position[16] = 0.2;
  bestPosition[17] =position[17] = 0.1;
  bestPosition[18] =position[18] = 0.2;
  bestPosition[19] =position[19] = 0.292699;
  bestPosition[20] =position[20] = 0.2;
  bestPosition[21] =position[21] = 0.4;
  bestPosition[22] =position[22] = 0.1;
  bestPosition[23] =position[23] = 0.7;
  bestPosition[24] =position[24] = -0.1;
  bestPosition[25] =position[25] = 0.9;
  bestPosition[26] =position[26] = -0.4;
}

Particle::Particle(unsigned int p, double C1, double C2, vector<double> pos, ParticleSwarm& s) :
  swarm(s),
  position(pos),
  velocity(p),
  bestScore(DBL_MAX),
  bestPosition(p),
  c1(C1),
  c2(C2),
  numParameters(p)
{
  double phi = c1 + c2;
  k = 2./fabs(2.0 - phi - sqrt(phi*phi - 4 * phi));

  bestPosition = position;

  // Now initialise the particle
  for (unsigned int i = 0; i < p; i++)
  {
    // Velocity always starts as 0
    velocity[i] = 0;
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  generator = default_random_engine(seed);
}


vector<double> Particle::getPerturbation()
{
  // The final perterbed result
  vector<double> pert(numParameters);
  // Vectors holding parameter indexes for perterbation
  vector<double> positionParameters;
  for (int i = 0; i < 3; i++)
    positionParameters.push_back(i);
  vector<double> orientationParameters;
  for (int i = 3; i < 7; i++)
    orientationParameters.push_back(i);
  vector<unsigned int> jointParameters;
  for (int i = 7; i < numParameters; i++)
    jointParameters.push_back(i);
  // Shuffle them
  random_shuffle ( positionParameters.begin(), positionParameters.end() );
  random_shuffle ( orientationParameters.begin(), orientationParameters.end() );
  random_shuffle ( jointParameters.begin(), jointParameters.end() );

  // How many of each parameter to shuffle
  const unsigned int positionsToShuffle = 1;
  const unsigned int orientationsToShuffle = 2;
  const unsigned int jointsToShuffle = 8;

  int positionsShuffled = 0;
  for (int i = 0; i < 3; i++)
  {
    if (positionsShuffled < positionsToShuffle)
    {  
      normal_distribution<double> distribution(position[positionParameters[i]], 50);
      pert[positionParameters[i]] = distribution(generator);
    }
    else
      pert[positionParameters[i]] = position[positionParameters[i]];
    positionsShuffled++;
  }

  int orientationsShuffled = 0;
  for (int i = 0; i < 4; i++)
  {
    if (orientationsShuffled < orientationsToShuffle)
    {  
      normal_distribution<double> distribution(position[orientationParameters[i]], 0.5);
      pert[orientationParameters[i]] = distribution(generator);
    }
    else
      pert[orientationParameters[i]] = position[orientationParameters[i]];
    orientationsShuffled++;
  }

  int jointsShuffled = 0;
  for (int i = 0; i < numParameters - 7; i++)
  {
    if (jointsShuffled < jointsToShuffle)
    {  
      normal_distribution<double> distribution(position[jointParameters[i]], 0.3);
      pert[jointParameters[i]] = distribution(generator);
    }
    else
      pert[jointParameters[i]] = position[jointParameters[i]];
    jointsShuffled++;
  }

  //for (int i = 0; i < 3; i++)
  //{
  //  normal_distribution<double> distribution(position[i], 30);
  //  pert.push_back(distribution(generator));
  //}

//  for (int i = 3; i < 7; i++)
//  {
//    normal_distribution<double> distribution(position[i], 100);
//    pert.push_back(distribution(generator));
//  }
//
//  for (unsigned int i = 7; i < numParameters; i++)
//  {
//    normal_distribution<double> distribution(position[i], M_PI/16);
//    pert.push_back(distribution(generator));
//  }
//
//
  normaliseQuarternion(pert);

  return pert;
}

void Particle::shuffle()
{
  unsigned int joint = (rand() % (NUM_PARAMETERS - 7)) + 7;
  if (joint == 7 || joint == 11 || joint == 15 || joint == 19)
    position[joint] = (rand()/(double(RAND_MAX)/2) - 1) * M_PI/8;
  else
    position[joint] = rand()/(double(RAND_MAX)) * M_PI/2;
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

void Particle::update(double score)
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

  bool outOfBounds = false;

  for (unsigned int i = 0; i < numParameters; i++)
  {
    velocity[i] = k * (velocity[i] + c1r1 * (bestPosition[i] - position[i]) + \
                                     c2r2 * (swarm.getBestParticle().getArray()[i] - position[i]));

    position[i] = position[i] + velocity[i];

    if (Hand::outOfBounds(i, position[i]))
    {
      outOfBounds = true;
      position[i] = swarm.getNearestParticleInDimension(i, position[i]); 
//      break;
    }
  }

  normaliseQuarternion(position);

  //cout << "turning: " << velocity[4] << endl;

  //if (outOfBounds)
  //{
  //  position = swarm.getNearestParticle(*this).position;

  //  for (unsigned int i = 0; i < numParameters; i++)
  //    position[i] = position[i] + velocity[i];
  //}
}

void Particle::resetScore() 
{ 
  bestScore = DBL_MAX; 
  for (int i = 0 ; i < numParameters; i++)
 // {
  //  bestPosition[i] = 0;
    velocity[i] = 0;
 // }
};

void Particle::normaliseQuarternion(vector<double>& pos)
{
  glm::quat quat(pos[3], pos[4], pos[5], pos[6]);
  quat = glm::normalize(quat);
  pos[3] = quat.w;
  pos[4] = quat.x;
  pos[5] = quat.y;
  pos[6] = quat.z;
}
