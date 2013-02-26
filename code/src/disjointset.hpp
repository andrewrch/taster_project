#ifndef DISJOINTSET_H
#define DISJOINTSET_H

#include <iostream>
#include <vector>

class DisjointSet {
  public:
    DisjointSet();
    ~DisjointSet();
    void setUnion(unsigned int, unsigned int);
    unsigned int find(unsigned int);
    void makeSet(unsigned int);

  private:
    struct Node
    {
      Node* parent;
      unsigned int rank, value;
    };

    std::vector<Node*> nodes;
};

#endif
