#include <iostream>
#include <cstring>
#include <fstream>

#include "config.h"
#include "Nodes.h"


#define MAXLINE 256


void Nodes::addNode(PID id, HOST host, PORT port) {
//   if (DEBUG) { std::cout << KMAG << "adding " << id << " to list of nodes" << KNRM << std::endl; }
  NodeInfo hp(id,host,port);
  this->nodes[id]=hp;
}


NodeInfo * Nodes::find(PID id) {
  std::map<PID,NodeInfo>::iterator it = nodes.find(id);

  if (it != nodes.end()) {
//     if (DEBUG) { std::cout << KMAG << "found a corresponding NodeInfo entry" << KNRM << std::endl; }
    return &(it->second);
  } else { return NULL; }
}


std::set<PID> Nodes::getIds() {
  std::map<PID,NodeInfo>::iterator it = nodes.begin();
  std::set<PID> l = {};
  while (it != nodes.end()) { l.insert(it->first); it++; }
  return l;
}


void Nodes::printNodes() {
  std::map<PID,NodeInfo>::iterator it = nodes.begin();

  while (it != nodes.end()) {
    PID id = it->first;
    NodeInfo nfo = it->second;

//     if (DEBUG) std::cout << KMAG << "id: " << id << KNRM << std::endl;
    it++;
  }
}

int Nodes::numNodes() {
  std::map<PID,NodeInfo>::iterator it = nodes.begin();

  int count = 0;
  while (it != nodes.end()) { count++; it++; }
  return count;
}


void printIds(std::list<PID> l) {
  std::list<PID>::iterator it = l.begin();

  while (it != l.end()) {
//       if (DEBUG) std::cout << KMAG << "id: " << *it << KNRM << std::endl;
    it++;
  }
}


Nodes::Nodes() {}


Nodes::Nodes(std::string filename) {
  std::ifstream inFile(filename);
  char oneline[MAXLINE];
  char delim[] = " ";
  char *token;

  while (inFile) {
    inFile.getline(oneline,MAXLINE);
    token = strtok(oneline,delim);

    if (token) {
      // id
      int id = atoi(token+3);

      // host
      token=strtok(NULL,delim);
      HOST host = token+5;

      // replica port
      token=strtok(NULL,delim);
      PORT port = atoi(token+5);

//       std::cout << KMAG << "id: " << id << "; host: " << host << "; port: " << port << KNRM << std::endl;

      addNode(id,host,port);
    }
  }

//   if (DEBUG) std::cout << KMAG << "closing configuration file" << KNRM << std::endl;
  inFile.close();
//   if (DEBUG) std::cout << KMAG << "done parsing the configuration file" << KNRM << std::endl;
}
