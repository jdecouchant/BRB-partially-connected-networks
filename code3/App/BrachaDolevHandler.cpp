#include "BrachaDolevHandler.h"
#include <iostream>
#include <fstream>

inline bool contains(std::set<PID> s, PID p) {
    return (s.find(p) != s.end());
}

BrachaDolevHandler::BrachaDolevHandler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename, int payloadSize, bool mod1, bool mod2, bool mod3, bool mod4, bool mod5) : Handler(id,pconf,nodes,topofilename) {

    this->payloadSize = payloadSize; 
    
    MOD_1_BONOMI = mod1;
    MOD_2_BONOMI = mod2;
    MOD_3_BONOMI = mod3;
    MOD_4_BONOMI = mod4;
    MOD_5_BONOMI = mod5;
    
    // Binding message types to corresponding handling functions 
    this->pnet.reg_handler(salticidae::generic_bind(&BrachaDolevHandler::handle_echo, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&BrachaDolevHandler::handle_ready, this, _1, _2));
    
    usleep(5000000); // in microseconds 
    
    // Send a Dolev msg to all other peers    
    if (myid==0) init_bracha_dolev_broadcast();

    auto pshutdown = [&](int) {pec.stop();};
    salticidae::SigEvent pev_sigterm(pec, pshutdown);
    pev_sigterm.add(SIGTERM);

    pec.dispatch();
}

void BrachaDolevHandler::init_bracha_dolev_broadcast() {
    
    char * val = new char[payloadSize];
    for (int i = 0; i < payloadSize; i++) {
        val[i] = 'a'+rand()%26;
    }
    
    struct mId msgId;
    msgId.msgHdr = HDR_DOLEV;
    msgId.broadcasterId = myid;
    msgId.broadcastId = myBroadcastId;
    msgId.echoOrReadySender = myid; 
    
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    rcvEchos[broadId].insert(myid);
    deliveredMsgs.insert(msgId); 
    
    std::vector<PID> emptyPath;
    Echo m = Echo(payloadSize, val, this->myid, this->myid, this->myBroadcastId, this->myid, emptyPath);
    
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID i = selfNeighbors[j];
        this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
    }
    
    if (DEBUG) std::cout<<KCYN<<myid<<": sender deliver ["<<(int) HDR_ECHO<< ",("<<myid<<","<<this->myBroadcastId<<"),"<<myid<<"]" << std::endl;
    myBroadcastId++;
    
    delete[] val;
}

void BrachaDolevHandler::send_echo_bracha_dolev(struct mId msgId, char *val) {
    
    std::vector<PID> emptyPath;
    Echo m = Echo(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, this->myid, emptyPath);
    
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID i = selfNeighbors[j];
        this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
    }

    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    rcvEchos[broadId].insert(myid);
    deliveredMsgs.insert(msgId); 
}

void BrachaDolevHandler::send_ready_bracha_dolev(struct mId msgId, char * val) {
    
    std::vector<PID> emptyPath;
    Ready m = Ready(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, this->myid, emptyPath);
    
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID i = selfNeighbors[j];
        this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
    }
    
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    rcvReadys[broadId].insert(myid);
    deliveredMsgs.insert(msgId);
}

bool BrachaDolevHandler::msgCanBeDelivered(struct mId msgId, PID linkSenderId) {
        
        // Check if (f+1) disjoint paths can be identified
        int threshold = numFaulty + 1;
        std::string neighborsBitmask(threshold, 1); // used to generate all 2f+1 combinations
        neighborsBitmask.resize(numNodes, 0);
        
        do {
            int selectedIds[threshold]; // stores the ids of the 2f+1 nodes whose paths will be examined 
            bool invalidNodes = false;
            int cur = 0;
            int senderPosInSelected = 0;
            for (int i = 0; i < numNodes; ++i) { // [0..N-1] integers {
                if (neighborsBitmask[i]) {
                    if (pathsBitmasks[msgId][i].size() == 0) { // need at least one path per neighbor
                        invalidNodes = true;
                        break;
                    }
                    selectedIds[cur] = i;
                    if (i == linkSenderId) {
                        senderPosInSelected = cur;
                    }
                    cur++;
                }
            }
            // TODO: would be (much) better to generate combinations that only involves the newly received message
            if (!invalidNodes) {
                int neighborIndex[threshold];
                for (int i = 0; i < threshold; i++) {
                    neighborIndex[i] = 0;
                }
                neighborIndex[senderPosInSelected] = pathsBitmasks[msgId][linkSenderId].size()-1;

                do { // generate all combinations of paths rcvd from these 2f+1 nodes 

                    if (DEBUG and myid==1) {
                        std::cout << KBLU << "\t"<<myid<<" selected counters: ";
                        for (int i = 0; i < threshold; i++) {
                            std::cout << neighborIndex[i]<<"/"<<pathsBitmasks[msgId][selectedIds[i]].size()-1<<" ";   
                        }
                        std::cout << KNRM << std::endl;
                    }
                    
                    // Check if all bitmasks never intersect
                    bool disjoint = true;
                    std::string unionBitmask(numNodes,0);
                    for (int i = 0; i < threshold; i++) {
                        std::string curBitmask = pathsBitmasks[msgId][selectedIds[i]][neighborIndex[i]];

                        for (int j = 0; j < numNodes; j++) {
                            if (j != msgId.broadcasterId and curBitmask[j]) {
                                if (unionBitmask[j]) {
                                    disjoint = false;
                                } else {
                                    unionBitmask[j] = 1;
                                }
                            }
                        }
                    }
                    
                    if (DEBUG and myid==1) std::cout << "\t"<<((disjoint)? "disjoint": "non disjoint") << std::endl;
                    if (disjoint) {
                        return true;
                    }
                    
                    int pos = 0;
                    while (pos < threshold and neighborIndex[pos] == pathsBitmasks[msgId][selectedIds[pos]].size()-1) {
                        if (selectedIds[pos]!=linkSenderId) {
                            neighborIndex[pos] = 0;
                        }
                        pos++;
                    }
                    if (pos == threshold) {
                        break;   
                    } else {
                        neighborIndex[pos]++;
                    }

                } while (true);
            }
            
        } while (std::prev_permutation(neighborsBitmask.begin(), neighborsBitmask.end()));
        
        return false;
}


void BrachaDolevHandler::handle_echo(Echo msg, const PeerNet::conn_t &conn) {
    
    numEcho++;
    std::cout << KRED <<"\t"<<nfo() << " : " << numEcho << " Echo msgs (total="<<numEcho+numReady<<")"<<KNRM<<std::endl;
    
    struct mId msgId;
    msgId.msgHdr = HDR_ECHO;
    msgId.broadcasterId = msg.broadcasterId;
    msgId.broadcastId = msg.broadcastId;
    msgId.echoOrReadySender = msg.echoSender;
    
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    bool wasDelivered = (deliveredMsgs.find(msgId) != deliveredMsgs.end()); 
    
    if (MOD_5_BONOMI && wasDelivered) {return;} // ignore if already delivered
    
    if (MOD_4_BONOMI) { // if empty path from q, store it, and will ignore paths that contain q in the future
        for (int i = 0; i < msg.pathLen; i++) { 
            if (neighborsThatDelivered[msgId].find(msg.path[i]) != neighborsThatDelivered[msgId].end()) {
                return;
            }
        }
        if (msg.pathLen == 0) {
            neighborsThatDelivered[msgId].insert(msg.linkSenderId);
        }
    }
    
    // Assemble path: rcvd path + link sender Id
    std::vector<PID> path = msg.path;
    if (msg.echoSender != msg.linkSenderId) {
        path.push_back(msg.linkSenderId);
    }
    
    // Path string 
    std::string pathBitmask = std::string(numNodes, 0);
    for (int i = 0; i < path.size(); i++) { 
        pathBitmask[path[i]] = 1;
    }
    
    // Insert path into data structure
    if (pathsBitmasks.find(msgId) == pathsBitmasks.end()) {
        pathsBitmasks[msgId] = new std::vector<std::string>[numNodes];
    }
    pathsBitmasks[msgId][msg.linkSenderId].push_back(pathBitmask);
    
    // Direct deliver? 
    bool directDeliver = MOD_1_BONOMI && msg.linkSenderId == msg.echoSender;
    
    // Indirect Dolev-deliver ? 
    bool indirectDeliver = (!wasDelivered and !directDeliver and msgCanBeDelivered(msgId, msg.linkSenderId));
    
    if (!wasDelivered and (directDeliver or indirectDeliver)) {
        deliveredMsgs.insert(msgId); // save delivered broadcast id 
        rcvEchos[broadId].insert(msgId.echoOrReadySender);
        
        // Generate own Echo?
        if (!contains(rcvEchos[broadId], myid) and 
           (contains(rcvEchos[broadId], msg.broadcasterId) or rcvEchos[broadId].size() >= numFaulty + 1)) {
            send_echo_bracha_dolev(msgId, msg.val);
        }
        
        // Generate own Ready?
        if (!contains(rcvReadys[broadId], myid) and 
            (rcvEchos[broadId].size() >= byzQuorumSize or     rcvReadys[broadId].size() >= numFaulty+1)) {
            send_ready_bracha_dolev(msgId, msg.val);
        }
    }
    
    // Relay message with modified path to neighbors
    for (int i = 0; i < selfNeighbors.size(); i++) {
        PID curNeighbor = selfNeighbors[i];
        if (curNeighbor == msg.echoSender) continue;
        
        if (MOD_2_BONOMI and (directDeliver or indirectDeliver)) {
            std::vector<PID> emptyPath;
            Echo d = Echo(payloadSize, msg.val, this->myid, msg.broadcasterId, msg.broadcastId, msg.echoSender, emptyPath);
            this->pnet.send_msg(d, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
    
        } else {
            bool isInPath = false;
            for (int j = 0; j < path.size(); j++) {
                if (curNeighbor == path[j]) {
                    isInPath = true;
                    break;
                }
            }
            
            if (!isInPath) {
                Echo d = Echo(payloadSize, msg.val, this->myid, msg.broadcasterId, msg.broadcastId, msg.echoSender, path);
        
                this->pnet.send_msg(d, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
            }
        }
    }
}

void BrachaDolevHandler::handle_ready(Ready msg, const PeerNet::conn_t &conn) {
    numReady++;
    std::cout << KRED << "\t"<<nfo() << " : " << numReady << " Ready msgs (total="<<numReady+numEcho<<")"<< KNRM << std::endl;
    
    struct mId msgId;
    msgId.msgHdr = HDR_READY;
    msgId.broadcasterId = msg.broadcasterId;
    msgId.broadcastId = msg.broadcastId;
    msgId.echoOrReadySender = msg.readySender;
    
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    bool wasDelivered = (deliveredMsgs.find(msgId) != deliveredMsgs.end()); 
    
    if (MOD_5_BONOMI && wasDelivered) {return;} // ignore if already delivered
    
    if (MOD_4_BONOMI) { // if empty path from q, store it, and will ignore paths that contain q in the future
        for (int i = 0; i < msg.pathLen; i++) { 
            if (neighborsThatDelivered[msgId].find(msg.path[i]) != neighborsThatDelivered[msgId].end()) {
                return;
            }
        }
        if (msg.pathLen == 0) {
            neighborsThatDelivered[msgId].insert(msg.linkSenderId);
        }
    }
	
    // Assemble path: rcvd path + link sender Id
    std::vector<PID> path = msg.path;
    if (msg.readySender != msg.linkSenderId) {
        path.push_back(msg.linkSenderId);
    }
    
    // Path string 
    std::string pathBitmask = std::string(numNodes, 0);
    for (int i = 0; i < path.size(); i++) { 
        pathBitmask[path[i]] = 1;
    }
    
    // Insert path into data structure
    if (pathsBitmasks.find(msgId) == pathsBitmasks.end()) {
        pathsBitmasks[msgId] = new std::vector<std::string>[numNodes];
    }
    pathsBitmasks[msgId][msg.linkSenderId].push_back(pathBitmask);
    
    // Direct deliver? 
    bool directDeliver = MOD_1_BONOMI && msg.linkSenderId == msg.readySender;
    
    // Indirect Dolev-deliver ? 
    bool indirectDeliver = (!wasDelivered and !directDeliver and msgCanBeDelivered(msgId, msg.linkSenderId));
    
    if (!wasDelivered and (directDeliver or indirectDeliver)) {
        rcvReadys[broadId].insert(msgId.echoOrReadySender);
        deliveredMsgs.insert(msgId); // save delivered broadcast id 
                        
        // Generate own Ready?
        if (!contains(rcvReadys[broadId], myid) and (contains(rcvReadys[broadId], msg.broadcasterId) or rcvReadys[broadId].size() >= numFaulty + 1)) {
            send_ready_bracha_dolev(msgId, msg.val);
        }
        
        // Deliver? 
        if (rcvReadys[broadId].size() >= 2*numFaulty+1 and deliveredBroad.find(broadId) == deliveredBroad.end()) {
            std::cout << KCYN << myid<<" DELIVERED ["<<msg.broadcasterId<<","<<msg.broadcastId<<"]" << std::endl;
            deliveredBroad.insert(broadId);
        }
    }
    
    // Relay message with modified path to neighbors
    for (int i = 0; i < selfNeighbors.size(); i++) {
        PID curNeighbor = selfNeighbors[i];
        if (curNeighbor == msg.readySender) continue;
        
        if (MOD_2_BONOMI and (directDeliver or indirectDeliver)) {
            std::vector<PID> emptyPath;
            Ready d = Ready(payloadSize, msg.val, this->myid, msg.broadcasterId, msg.broadcastId, msg.readySender, emptyPath);
            this->pnet.send_msg(d, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
    
        } else {
            bool isInPath = false;
            for (int j = 0; j < path.size(); j++) {
                if (curNeighbor == path[j]) {
                    isInPath = true;
                    break;
                }
            }
            
            if (!isInPath) {
                Ready d = Ready(payloadSize, msg.val, this->myid, msg.broadcasterId, msg.broadcastId, msg.readySender, path);
        
                this->pnet.send_msg(d, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
            }
        }
    }
    
    
}
