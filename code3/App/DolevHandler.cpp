
#include "DolevHandler.h"
#include <iostream>
#include <fstream>

DolevHandler::DolevHandler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename, bool mod1, bool mod2, bool mod3, bool mod4, bool mod5) : Handler(id,pconf,nodes,topofilename) {

    MOD_1_BONOMI = mod1;
    MOD_2_BONOMI = mod2;
    MOD_3_BONOMI = mod3;
    MOD_4_BONOMI = mod4;
    MOD_5_BONOMI = mod5;
    
    // Binding message types to corresponding handling functions 
    this->pnet.reg_handler(salticidae::generic_bind(&DolevHandler::handle_dolev, this, _1, _2));
//     this->pnet.reg_handler(salticidae::generic_bind(&Handler::handle_echo, this, _1, _2));
//     this->pnet.reg_handler(salticidae::generic_bind(&Handler::handle_ready, this, _1, _2));
    
    usleep(5000000); // in microseconds 
    
    // Send a Dolev msg to all other peers    
    if (myid==0) init_dolev_broadcast();

    auto pshutdown = [&](int) {pec.stop();};
    salticidae::SigEvent pev_sigterm(pec, pshutdown);
    pev_sigterm.add(SIGTERM);

    pec.dispatch();
}

bool DolevHandler::msgCanBeDelivered(struct mId msgId, PID linkSenderId) {

    // Check if (f+1) disjoint paths can be identified
    int threshold = numFaulty+1;
    std::string neighborsBitmask(threshold,1); // used to generate all 2f+1 combinations
    neighborsBitmask.resize(numNodes, 0);
    
    do {
        int selectedIds[threshold]; // stores the ids of the f+1 nodes whose paths will be examined 
        
        int cur = 0;
        int senderPosInSelected = 0;
        bool invalidNodes = false;
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
       
            do { // generate all combinations of paths rcvd from these f+1 nodes 
                
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

void DolevHandler::init_dolev_broadcast() {

	char * val = new char[payloadSize];
    for (int i = 0; i < payloadSize; i++) {
        val[i] = 'a' + rand()%26;
    }

	struct mId msgId;
	msgId.msgHdr = HDR_DOLEV;
	msgId.broadcasterId = myid;
	msgId.broadcastId = myBroadcastId;

	deliveredMsgs.insert(msgId);

	std::vector<PID> emptyPath;
	Dolev m = Dolev(payloadSize, val, this->myid, this->myid, this->myBroadcastId, emptyPath);

	for (int j = 0; j < selfNeighbors.size(); j++) { 
		PID i = selfNeighbors[j];
		this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[i]));
	}

	std::cout<<KCYN <<myid<<": sender deliver ["<<(int) HDR_ECHO<<","<<myid<<","<<this->myBroadcastId<<"]" << std::endl;
	myBroadcastId++;
}

void DolevHandler::handle_dolev(Dolev msg, const PeerNet::conn_t &conn) {
	numDolev++;
	std::cout << KRED << nfo() << " : " << numDolev << " Dolev msgs" << KNRM << std::endl;

	struct mId msgId;
	msgId.msgHdr = HDR_DOLEV;
	msgId.broadcasterId = msg.broadcasterId;
	msgId.broadcastId = msg.broadcastId;
    msgId.echoOrReadySender = msg.broadcasterId; // Not used, but required for equality tests

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

	// TODO: copy from message
    char * val = new char[payloadSize];
    for (int i = 0; i < payloadSize; i++) {
        val[i] = msg.val[i]; 
    }

	// Assemble path: rcvd path + link sender Id
	std::vector<PID> path = msg.path;
	if (msg.broadcasterId != msg.linkSenderId) {
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

	// Check if message can be directly delivered  
    bool directDeliver = (MOD_1_BONOMI and msg.linkSenderId == msg.broadcasterId);
        
    if (directDeliver) {
		deliveredMsgs.insert(msgId); // save delivered broadcast id 
		std::cout << KCYN << myid<<": direct deliver ["<<msg.broadcasterId<<","<<msg.broadcastId<<"]" << std::endl;
	}

	// Can the message be Dolev-delivered?
	bool indirectDeliver = (!wasDelivered and !directDeliver and 
 msgCanBeDelivered(msgId, msg.linkSenderId));
	
	if (indirectDeliver) {
        deliveredMsgs.insert(msgId);
        std::cout << KCYN << myid<<": indirect deliver ["<<msg.broadcasterId<<","<<msg.broadcastId<<"]" << KNRM << std::endl;   
    }

	// Relay path to neighbors
	for (int i = 0; i < selfNeighbors.size(); i++) {
		PID curNeighbor = selfNeighbors[i];
		if (curNeighbor == msg.broadcasterId) continue;

		if (MOD_2_BONOMI and (directDeliver or indirectDeliver)) {
			std::vector<PID> emptyPath;
			Dolev d = Dolev(payloadSize, val, this->myid, msg.broadcasterId, msg.broadcastId, emptyPath);
			this->pnet.send_msg(d, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));

		} else {
			bool isInPath = false;
			for (int j = 0; j < path.size(); j++) {
				if (curNeighbor == path[j]) {
					isInPath = true;
					break;
				}
			}

			if (!isInPath) { // TODO: add MOD_3_BONOMI here!
				Dolev d = Dolev(payloadSize, val, this->myid, msg.broadcasterId, msg.broadcastId, path);

				this->pnet.send_msg(d, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
			}
		}
	}
	
	delete[] val;

}
