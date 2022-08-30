#ifndef OPT_BRACHA_CPA_HANDLER_H
#define OPT_BRACHA_CPA_HANDLER_H

#include "Handler.h"

class OptBrachaCPAHandler: public Handler {
    
private:
    
    unsigned int numEcho = 0;
    unsigned int numReady = 0;
    
    // Optims: 
    
    // MOD_1_BONOMI is necessary for CPA
    // 1 - after validating, do not send to neighbors who already sent the message
    
    // TODO: check which options make sense
    bool MOD_1_BONOMI = true; // delivers if rcvd from source
    bool MOD_2_BONOMI = true; // upon delivers, send empty path
    bool MOD_3_BONOMI = true; // relays content only to neighbors that haven't delivered
    bool MOD_4_BONOMI = true; // if empty path from q, then ignore paths that contain q
    bool MOD_5_BONOMI = true; // stop forwarding/analyzing a content after delivery
    
    std::set<struct bId> deliveredBroad; // Broadcasts that are over
    
    // For Bracha-Dolev: store the ids of the nodes that sent Echos or Readys
    std::map<struct mId, std::set<PID>> rcvEchos; // TODO: msg type could be removed from key
    std::map<struct mId, std::set<PID>> rcvReadys; // TODO: same
    
    std::map<struct bId, std::set<PID>> validatedEchos;
    std::map<struct bId, std::set<PID>> validatedReadys;

    // Bracha-Dolev with or without Bonomi's modifications
    void init_bracha_cpa_broadcast();
    
    void send_echo_bracha_cpa(struct mId msgId, VAL val);
    void send_ready_bracha_cpa(struct mId msgId, VAL val);
    
    void handle_echo_cpa(EchoCPA msg, const PeerNet::conn_t &conn);
    void handle_ready_cpa(ReadyCPA msg, const PeerNet::conn_t &conn);

public:    
    OptBrachaCPAHandler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename, bool mod1, bool mod2, bool mod3, bool mod4, bool mod5);

};

#endif
