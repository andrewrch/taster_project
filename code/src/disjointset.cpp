#include <iostream>
#include <vector>
#include "disjointset.hpp"

using namespace std;

DisjointSet::DisjointSet() {
  // Set nodes size to large number
  // to reduce resizing.
  nodes.resize(256);
}

DisjointSet::~DisjointSet() {
  for (vector<Node*>::iterator it = nodes.begin(); it < nodes.end(); it++)
    delete *it;
}

// Join two sets together
void DisjointSet::setUnion(unsigned int a, unsigned int b)
{
  // Sets shouldn't be null
  if (nodes[a] == NULL || nodes[b] == NULL)
    return;

  // Get the root of each set
  Node* aRoot = nodes[find(a)];
  Node* bRoot = nodes[find(b)];

  // If they have the same root then
  // they are already joined
  if (&aRoot == &bRoot)
    return;

  // Join up the nodes.
  // We point to the other root node to
  // reduce traversal for future
  // unions
  if (aRoot->rank < bRoot->rank)
    aRoot->parent = bRoot;
  else if (aRoot->rank > bRoot->rank)
    bRoot->parent = aRoot;
  else
  {
    bRoot->parent = aRoot;
    // Increase rank so we know it is higher up the tree
    aRoot->rank++;;
  }
}

unsigned int DisjointSet::find(unsigned int num)
{
  // If a root node return that node
  if (nodes[num]->parent->value == nodes[num]->value)
    return num;
  else
  {
    nodes[num]->parent = nodes[find(nodes[num]->parent->value)];
    return nodes[num]->parent->value;
  }
}

void DisjointSet::makeSet(unsigned int num)
{
  // Resize if nodes is too small for
  // new node
  if (num >= nodes.size())
    nodes.resize(nodes.size()*2);

  if (nodes[num] != NULL)
    return;

  nodes[num] = new Node();
  nodes[num]->value = num;
  nodes[num]->parent = nodes[num];
  nodes[num]->rank = 0;
}
