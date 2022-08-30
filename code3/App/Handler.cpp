#include "config.h"
#include "Handler.h"

#include <fstream>
#include <iostream>
#include <unistd.h>


std::string Handler::nfo() { return "[" + std::to_string(this->myid) + "]"; }

const uint8_t Dolev::opcode; // Dolev struct def in config.h

const uint8_t SendInit::opcode;
const uint8_t Send::opcode;

const uint8_t Echo::opcode;
const uint8_t EchoInit::opcode;
const uint8_t EchoLocalId::opcode;

const uint8_t Ready::opcode;
const uint8_t ReadyInit::opcode;
const uint8_t ReadyLocalId::opcode;

const uint8_t EchoSingleHop::opcode;
const uint8_t EchoInitSingleHop::opcode;
const uint8_t EchoLocalIdSingleHop::opcode;

const uint8_t ReadySingleHop::opcode;
const uint8_t ReadyInitSingleHop::opcode;
const uint8_t ReadyLocalIdSingleHop::opcode;

const uint8_t EchoEcho::opcode;
const uint8_t EchoEchoInit::opcode;
const uint8_t EchoEchoLocalId::opcode;

const uint8_t ReadyEcho::opcode;
const uint8_t ReadyEchoInit::opcode;
const uint8_t ReadyEchoLocalId::opcode;

const uint8_t EchoCPA::opcode;
const uint8_t ReadyCPA::opcode;

Handler::Handler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename) : pnet(pec,pconf) {

    this->myid = id;

    // Read topology from file
    std::ifstream intopofile(topofilename);

    intopofile >> numNodes >> numFaulty;

    byzQuorumSize = numNodes + numFaulty + 1;
    if (byzQuorumSize % 2 == 1) {
        byzQuorumSize++;
    }
    byzQuorumSize /= 2;

    bool **connected = new bool*[numNodes];
    for (int i = 0; i < numNodes; i++) {
        connected[i] = new bool[numNodes];
        for (int j = 0; j < numNodes; j++) {
            connected[i][j] = false;
        }
    }

    int a, b;
    while (intopofile >> a >> b) {
        if (a!=b) {
            connected[a][b] = true;
            connected[b][a] = true;
        }
    }

    // Allocate peers array
    peers = new Peer[numNodes];

    // Initialize neighbors
    for (int i = 0; i < this->numNodes; i++) {
        if (connected[myid][i]) {
            selfNeighbors.push_back(i);
        }
    }

    for (int i = 0; i < numNodes; i++) {
        delete[] connected[i];
    }
    delete[] connected;

    HOST host = "127.0.0.1";       // default value, which is updated below from 'nodes'
    PORT port = 8760 + this->myid; // default value, which is updated below from 'nodes'

    NodeInfo* ownnode = nodes.find(this->myid);
    if (ownnode != NULL) {
      host = ownnode->getHost();
      port = ownnode->getPort();
    } else {
      std::cout << KLRED << nfo() << "couldn't find own information among nodes" << KNRM << std::endl;
    }

    salticidae::NetAddr paddr = salticidae::NetAddr(host + ":" + std::to_string(port));
    this->pnet.start();
    this->pnet.listen(paddr);


    usleep(5000000); // in microseconds

    // Connecting to neighbors
    std::cout << KBLU << nfo() << " connecting..." << KNRM << std::endl;
    for (int i = 0; i < selfNeighbors.size(); i++) {

      PID j = selfNeighbors[i];

      NodeInfo* othernode = nodes.find(j);
      if (othernode != NULL) {

        HOST jhost = othernode->getHost();
        PORT jport = othernode->getPort();

        salticidae::NetAddr peer_addr(jhost + ":" +std::to_string(jport));
        salticidae::PeerId other{peer_addr};
        this->pnet.add_peer(other);
        this->pnet.set_peer_addr(other, peer_addr);
        this->pnet.conn_peer(other);

        std::cout << KLGRN << nfo() << "connecting to " << othernode->toString() << KNRM << std::endl;

        this->peers[j] = std::make_pair(j,other);
      } else {
        std::cout << KLRED << nfo() << "couldn't find " << j << "'s information among nodes" << KNRM << std::endl;
      }
    }

}
