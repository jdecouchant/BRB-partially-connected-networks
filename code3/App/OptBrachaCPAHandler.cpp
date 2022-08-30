#include "OptBrachaCPAHandler.h"
#include <iostream>
#include <fstream>

inline bool contains(std::set<PID> s, PID p) {
    return (s.find(p) != s.end());
}

OptBrachaCPAHandler::OptBrachaCPAHandler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename, bool mod1, bool mod2, bool mod3, bool mod4, bool mod5) : Handler(id,pconf,nodes,topofilename) {

    MOD_1_BONOMI = mod1;
    MOD_2_BONOMI = mod2;
    MOD_3_BONOMI = mod3;
    MOD_4_BONOMI = mod4;
    MOD_5_BONOMI = mod5;
    
    // Binding message types to corresponding handling functions 
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaCPAHandler::handle_echo_cpa, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaCPAHandler::handle_ready_cpa, this, _1, _2));
    
    usleep(5000000); // in microseconds 
    
    // Send a Dolev msg to all other peers    
    if (myid==0) init_bracha_cpa_broadcast();

    auto pshutdown = [&](int) {pec.stop();};
    salticidae::SigEvent pev_sigterm(pec, pshutdown);
    pev_sigterm.add(SIGTERM);

    pec.dispatch();
}

void OptBrachaCPAHandler::init_bracha_cpa_broadcast() {
    
    VAL val = 0;
    
    struct mId msgId;
    msgId.msgHdr = HDR_ECHO_CPA;
    msgId.broadcasterId = myid;
    msgId.broadcastId = myBroadcastId;
    msgId.echoOrReadySender = myid;

    rcvEchos[msgId].insert(myid);
    
    EchoCPA m = EchoCPA(val, this->myid, this->myid, this->myBroadcastId, this->myid);
    
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID i = selfNeighbors[j];
        this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
    }
//     std::cout<<KCYN<<myid<<": sender deliver ["<<(int) HDR_ECHO<< ",("<<myid<<","<<this->myBroadcastId<<"),"<<myid<<"]" << std::endl;
//     myBroadcastId++;
}

void OptBrachaCPAHandler::send_echo_bracha_cpa(struct mId msgId, VAL val) {
    
    EchoCPA m = EchoCPA(val, this->myid, msgId.broadcasterId, msgId.broadcastId, this->myid);
    
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID i = selfNeighbors[j];
        this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
    }

    struct mId ownMsgId = {HDR_ECHO_CPA, msgId.broadcasterId, msgId.broadcastId, myid};
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    rcvEchos[ownMsgId].insert(myid);
    validatedEchos[broadId].insert(myid);
}

void OptBrachaCPAHandler::send_ready_bracha_cpa(struct mId msgId, VAL val) {
    
    ReadyCPA m = ReadyCPA(val, this->myid, msgId.broadcasterId, msgId.broadcastId, this->myid);
    
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID i = selfNeighbors[j];
        this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
    }

    struct mId ownMsgId = {HDR_READY_CPA, msgId.broadcasterId, msgId.broadcastId, myid};
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    rcvReadys[ownMsgId].insert(myid);
    validatedReadys[broadId].insert(myid);
    
    struct mId echoMsgId = {HDR_ECHO_CPA, msgId.broadcasterId, msgId.broadcastId, myid};
    rcvEchos[echoMsgId].insert(myid);
    validatedEchos[broadId].insert(myid);
}

void OptBrachaCPAHandler::handle_echo_cpa(EchoCPA msg, const PeerNet::conn_t &conn) {
    
    numEcho++;
    std::cout << KRED <<"\t"<<nfo() << " : " << numEcho << " Echo msgs (total="<<numEcho+numReady<<")"<<KNRM<<std::endl;
    
    struct mId msgId = {HDR_ECHO_CPA, msg.broadcasterId, msg.broadcastId, msg.echoSender};
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
        
    if (contains(validatedEchos[broadId], msg.echoSender)) return;
    
    VAL val = msg.val;

    rcvEchos[msgId].insert(msg.linkSenderId);
    
    struct mId sourceMsgId = {HDR_ECHO_CPA, msg.broadcasterId, msg.broadcastId, msg.broadcasterId};
    rcvEchos[sourceMsgId].insert(msg.linkSenderId);
    
    bool validatedEcho = (msg.echoSender==msg.linkSenderId or rcvEchos[msgId].size() >= numFaulty+1);
    
    if (validatedEcho) { 
//         std::cout << myid << " validates Echo" << " ["<<(int) HDR_ECHO_CPA<< ",("<<msg.broadcasterId<<","<<this->myBroadcastId<<"),"<<msg.echoSender<<"]"<<std::endl;
        
        if (!contains(validatedEchos[broadId], msg.echoSender) 
             and msg.broadcasterId != msg.linkSenderId) { // MOD_1_BONOMI // TODO
            validatedEchos[broadId].insert(msg.echoSender);
            
            // Forward to neighbors that have not sent it
            EchoCPA m = EchoCPA(val, this->myid, msgId.broadcasterId, msgId.broadcastId, msg.echoSender);
        
            for (int j = 0; j < selfNeighbors.size(); j++) { 
                PID i = selfNeighbors[j];
                if (contains(rcvEchos[msgId],i)) continue;
                this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
            }
        }
    }

    bool sourceValidatedEcho = (msg.broadcasterId==msg.linkSenderId or rcvEchos[sourceMsgId].size() >= numFaulty+1);
    if (sourceValidatedEcho) {
        validatedEchos[broadId].insert(msg.broadcasterId);
    }
    
    if (!contains(validatedReadys[broadId], myid) and validatedEchos[broadId].size() >= 2*numFaulty+1) {
        send_ready_bracha_cpa(msgId, val);
    } else if (sourceValidatedEcho and not contains(validatedEchos[broadId], myid)) {
            //or validatedEchos[broadId].size() >= numFaulty+1)) { // TODO: (Optim 2) optim already
        send_echo_bracha_cpa(msgId, val);
    }

}

void OptBrachaCPAHandler::handle_ready_cpa(ReadyCPA msg, const PeerNet::conn_t &conn) {
    numReady++;
    std::cout << KRED << "\t"<<nfo() << " : " << numReady << " Ready msgs (total="<<numReady+numEcho<<")"<< KNRM << std::endl;
    
    struct mId msgId = {HDR_READY_CPA, msg.broadcasterId, msg.broadcastId, msg.readySender};
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
        
    if (contains(validatedReadys[broadId], msg.readySender)) return;
    
//     std::cout << KRED <<"\t"<<nfo() << " <- " << msg.linkSenderId << " ["<<(int) HDR_READY_CPA<< ",("<<msg.broadcasterId<<","<<this->myBroadcastId<<"),"<<msg.readySender<<"]"<<std::endl;
    
    VAL val = msg.val;

    rcvReadys[msgId].insert(msg.linkSenderId);
    
    bool validatedReady = (msg.readySender==msg.linkSenderId or rcvReadys[msgId].size() >= numFaulty+1);
    
    if (validatedReady) { 
//         std::cout << myid << " validates Ready" << " ["<<(int) HDR_READY_CPA << ",("<<msg.broadcasterId<<","<<this->myBroadcastId<<"),"<<msg.readySender<<"]"<<std::endl;
        
        if (!contains(validatedReadys[broadId], msg.readySender)) { 
            validatedReadys[broadId].insert(msg.readySender);
            
            // Forward message to all neighbors 
            ReadyCPA m = ReadyCPA(val, this->myid, msgId.broadcasterId, msgId.broadcastId, msg.readySender);
        
            for (int j = 0; j < selfNeighbors.size(); j++) { 
                PID i = selfNeighbors[j];
                if (contains(rcvReadys[msgId],i)) continue;
                this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
            }
        }
        
        if (!contains(validatedReadys[broadId], myid) and validatedReadys[broadId].size() >= numFaulty+1) {
            send_ready_bracha_cpa(msgId, val);
        }
        
        if (deliveredBroad.find(broadId) == deliveredBroad.end() and validatedReadys[broadId].size() >= 2*numFaulty+1) { 
                deliveredBroad.insert(broadId);
                std::cout << KCYN << myid<<" DELIVERED ["<<msg.broadcasterId<<","<<msg.broadcastId<<"]" << std::endl;
        }
    }
    
}
