#ifndef HOSTPORT_H
#define HOSTPORT_H


#include <string>


typedef std::string HOST;
typedef unsigned int PID; // process ids
typedef unsigned int PORT;


class NodeInfo {
 private:
  PID pid;
  HOST host;
  PORT port;

 public:
  NodeInfo();
  NodeInfo(PID pid, HOST host, PORT port);

  PID getId();
  HOST getHost();
  PORT getPort();

  std::string toString();

  bool operator<(const NodeInfo& hp) const;
};


#endif
