#ifndef NODES_H
#define NODES_H


#include <map>
#include <set>
#include <list>

#include "NodeInfo.h"


class Nodes {
 private:
  std::map<PID,NodeInfo> nodes;

 public:
  Nodes();
  Nodes(std::string filename);

  void addNode(PID id, HOST host, PORT port);

  NodeInfo * find(PID id);
  std::set<PID> getIds();

  void printNodes();
  int numNodes();
};


#endif
