#ifndef DOLEV_HANDLER_H
#define DOLEV_HANDLER_H

#include "Handler.h"

class DolevHandler: public Handler {
    
private:
    
    bool MOD_1_BONOMI = true; // delivers if rcvd from source
    bool MOD_2_BONOMI = true; // upon delivers, send empty path
    bool MOD_3_BONOMI = true; // relays content only to neighbors that haven't delivered
    bool MOD_4_BONOMI = true; // if empty path from q, then ignore paths that contain q
    bool MOD_5_BONOMI = true; // stop forwarding/analyzing a content after delivery
    
    unsigned int numDolev = 0;

    // For each (senderId, broadcastId) store the paths that are received per node (neighbour)
    std::map<struct mId, std::vector<std::string> *> pathsBitmasks; // an array of vector of paths (one per possible node in the network) per broadcast id
    std::set<struct mId> deliveredMsgs; // Echo/Ready messages that have been Dolev-delivered
    
    std::map<struct mId, std::set<int>> neighborsThatDelivered;
    
    void init_dolev_broadcast();
    void handle_dolev(Dolev msg, const PeerNet::conn_t &conn);
    
    bool msgCanBeDelivered(struct mId msgId, PID senderId);

public:    
    DolevHandler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename, bool mod1, bool mod2, bool mod3, bool mod4, bool mod5);

};

#endif
