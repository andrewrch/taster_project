#include "sphere.hpp"

using namespace std;

Sphere::Sphere(unsigned int r, unsigned int st, unsigned int sl) :
  vertices(0),
  normals(0),
  indices(0)
{
//  Mesh();
  makeSphere();
  setupArrays(vertices, normals, indices);
}

Sphere::~Sphere()
{
}

void Sphere::makeSphere()
{
  vertices.push_back(glm::vec3(-1.0f, -1.0f, 0.5773f));
  vertices.push_back(glm::vec3(0.0f, -1.0f, -1.15475));
  vertices.push_back(glm::vec3(1.0f, -1.0f, 0.5773f));
  vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

  normals.push_back(glm::vec3(-1.0f, -1.0f, 0.5773f));
  normals.push_back(glm::vec3(0.0f, -1.0f, -1.15475));
  normals.push_back(glm::vec3(1.0f, -1.0f, 0.5773f));
  normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

  indices.push_back(0);
  indices.push_back(3);
  indices.push_back(1);
  indices.push_back(1);
  indices.push_back(3);
  indices.push_back(2);
  indices.push_back(2);
  indices.push_back(3);
  indices.push_back(0);
  indices.push_back(0);
  indices.push_back(2);
  indices.push_back(1);
}
