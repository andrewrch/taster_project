#include <vector>

#include "mesh.hpp"

class Sphere : public Mesh
{
  public: 
    Sphere(unsigned int, unsigned int, unsigned int);
    ~Sphere();

  private:
    std::vector<glm::vec3> vertices, normals;
    std::vector<unsigned int> indices;

    void makeSphere();
};
