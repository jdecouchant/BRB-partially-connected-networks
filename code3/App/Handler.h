#ifndef HANDLER_H
#define HANDLER_H


#include <map>

#include "Message.h"
#include "Nodes.h"

// Salticidae related stuff
#include <memory>
#include <cstdio>
#include <functional>
#include "salticidae/msg.h"
#include "salticidae/event.h"
#include "salticidae/network.h"
#include "salticidae/stream.h"

using std::placeholders::_1;
using std::placeholders::_2;

using PeerNet   = salticidae::PeerNetwork<uint8_t>;
using Peer      = std::tuple<PID, salticidae::PeerId>;

struct mId {
  uint8_t msgHdr; // Msg code (e.g., ECHO)
  PID broadcasterId; 
  int broadcastId; 
  PID echoOrReadySender;
  
  bool operator<(const struct mId &m2) const {
    return (
        broadcasterId < m2.broadcasterId || 
        (broadcasterId == m2.broadcasterId && broadcastId < m2.broadcastId) ||
        (broadcasterId == m2.broadcasterId && broadcastId == m2.broadcastId && msgHdr < m2.msgHdr) ||
        (broadcasterId == m2.broadcasterId && broadcastId == m2.broadcastId && msgHdr == m2.msgHdr && echoOrReadySender < m2.echoOrReadySender)
    ); 
  }
};

struct bId {
  PID broadcasterId; 
  int broadcastId; 
  
  bool operator<(const struct bId &m2) const {
    return (
        broadcasterId < m2.broadcasterId || 
        (broadcasterId == m2.broadcasterId && broadcastId < m2.broadcastId) 
    ); 
  }
};

struct localInfo {
  PID broadcasterId; 
  int broadcastId; 
  
  unsigned int payloadSize; 
  const char * val;
};

// Needed for maps/sets where struct mId is used as key

class Handler {
    
private:
    bool MOD_1_BONOMI = true; // delivers if rcvd from source
    bool MOD_2_BONOMI = true; // upon delivers, send empty path
    bool MOD_3_BONOMI = true; // relays content only to neighbors that haven't delivered
    bool MOD_4_BONOMI = true; // if empty path from q, then ignore paths that contain q
    bool MOD_5_BONOMI = true; // stop forwarding/analyzing a content after delivery

protected:
    salticidae::EventContext pec; // peer ec
    Peer * peers;
    PeerNet pnet;
    
    PID myid;
    int myBroadcastId = 0; 
    
    unsigned int payloadSize = 10; // number of bytes per payload 
    
    unsigned int numNodes = 1; // total number of nodes
    unsigned int numFaulty = 1; // number of faulty nodes
    unsigned int byzQuorumSize;
    
    std::vector<PID> selfNeighbors; 

    // used to print debugging info
    std::string nfo();
    
public:
    Handler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename);
};


#endif
