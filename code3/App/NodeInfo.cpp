#include <cstring>
#include <iostream>
#include <assert.h>
#include <unistd.h>

#include "NodeInfo.h"


NodeInfo::NodeInfo() {
  this->pid  = 0;
  this->host = "";
  this->port = 0;
}


NodeInfo::NodeInfo(PID pid, HOST host, PORT port) {
  this->pid  = pid;
  this->host = host;
  this->port = port;
}


PID  NodeInfo::getId()   { return this->pid;  }
HOST NodeInfo::getHost() { return this->host; }
PORT NodeInfo::getPort() { return this->port; }


std::string NodeInfo::toString() {
  return ("NFO[id="  + std::to_string(this->pid)
          + ";host=" + this->host
          + ";port=" + std::to_string(this->port)
          + "]");
}


bool NodeInfo::operator<(const NodeInfo& hp) const {
  if (pid < hp.pid) { return true; }
  return false;
}
