#ifndef BRACHA_DOLEV_HANDLER_H
#define BRACHA_DOLEV_HANDLER_H

#include "Handler.h"

class BrachaDolevHandler: public Handler {
    
private:
    
    unsigned int numEcho = 0;
    unsigned int numReady = 0;
    
    bool MOD_1_BONOMI = true; // delivers if rcvd from source
    bool MOD_2_BONOMI = true; // upon delivers, send empty path
    bool MOD_3_BONOMI = true; // relays content only to neighbors that haven't delivered
    bool MOD_4_BONOMI = true; // if empty path from q, then ignore paths that contain q
    bool MOD_5_BONOMI = true; // stop forwarding/analyzing a content after delivery
    
    std::set<struct bId> deliveredBroad; // Broadcasts that are over
    
    // For Bracha-Dolev: store the ids of the nodes that sent Echos or Readys
    std::map<struct bId, std::set<PID>> rcvEchos; 
    std::map<struct bId, std::set<PID>> rcvReadys; 
    
    // For each (senderId, broadcastId) store the paths that are received per node (neighbour)
    std::map<struct mId, std::vector<std::string> *> pathsBitmasks; // an array of vector of paths (one per possible node in the network) per broadcast id
    std::set<struct mId> deliveredMsgs; // Echo/Ready messages that have been Dolev-delivered
    
    std::map<struct mId, std::set<int>> neighborsThatDelivered;
    
    // Bracha-Dolev with or without Bonomi's modifications
    void init_bracha_dolev_broadcast();
    void send_echo_bracha_dolev(struct mId msgId, char * val);
    void send_ready_bracha_dolev(struct mId msgId, char * val);
    void handle_echo(Echo msg, const PeerNet::conn_t &conn);
    void handle_ready(Ready msg, const PeerNet::conn_t &conn);
    
    bool msgCanBeDelivered(struct mId msgId, PID senderId);

public:    
    BrachaDolevHandler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename, int payloadSize, bool mod1, bool mod2, bool mod3, bool mod4, bool mod5);

};

#endif
