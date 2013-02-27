#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>

class Particle
{
  public:
    Particle(unsigned int, double, double);
    ~Particle() {};
    Particle& operator=(const Particle&);

    //
    double* getArray() { return &position[0]; };
    void update(double score, Particle);

    // So swarm can be reused each frame
    void resetScore() { bestScore = 0.0; };

  private:
    // Holds all information about current particle
    std::vector<double> position;
    std::vector<double> velocity;

    // Keep track of best score for updating best position
    double bestScore;
    std::vector<double> bestPosition;
    // Used for calculating new velocity
    double k, c1, c2;
};

#endif
