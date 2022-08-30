#include "OptBrachaDolevHandler.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include <chrono>
#include <ctime>  
#include <time.h>


inline bool contains(std::set<PID> s, PID p) {
    return (s.find(p) != s.end());
}

inline void OptBrachaDolevHandler::checkPath(Echo msg) { 
    std::vector<PID> path = msg.path; 
    for (int i = 0; i < path.size(); i++) {
        if (path[i] == this->myid) {
            std::cout << msg.linkSenderId<<" -> " << myid<<" RECEIVED ECHO " << msg.echoSender << " WITH MYID [" << std::endl;
            for (int j = 0; j < path.size(); j++) {
                std::cout << path[j] << ",";
            }
            std::cout << "]" << std::endl;
            break;
        }
    }
    
    if (msg.echoSender == this->myid) { 
        std::cout << "RECEIVED MY OWN ECHO !!!" << std::endl;
    }
}

inline void OptBrachaDolevHandler::checkPath(Ready msg) { 
    std::vector<PID> path = msg.path; 
    for (int i = 0; i < path.size(); i++) {
        if (path[i] == this->myid) {
            std::cout << "RECEIVED READY PATH WITH MYID !!!" << std::endl;
        }
    }
    
    if (msg.readySender == this->myid) { 
        std::cout << myid << " RECEIVED MY OWN READY !!!" << std::endl;
    }
}

void OptBrachaDolevHandler::printStats() {
    
    std::ofstream myfile;
    myfile.open(statsfilename+"msgCounts_"+std::to_string(myid));
    myfile << "# nSends nSendInits nEchos nEchoInits nEchoLocalIds nReadys nReadyInits nReadyLocalIds nEchoEchos nEchoEchoInits nEchoEchoLocalIds nReadyEchos nReadyEchoInits nReadyEchoLocalIds Total\n";
    for (auto it = deliveredBroad.begin(); it != deliveredBroad.end(); it++) {
        
        bId b = {it->first.broadcasterId, it->first.broadcastId};
        
        if (minBidMeasure > it->first.broadcastId or it->first.broadcastId >= maxBidMeasure) {continue;}
        
        myfile << "("<<it->first.broadcasterId << "," << it->first.broadcastId << ")\t";
        
        int sum = 0;
        
        if (numSends.find(b) == numSends.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numSends[b] << "\t";
            sum += numSends[b];
        }
        if (numSendInits.find(b) == numSendInits.end()) {
            myfile << 0 << "\t\t";
        } else {
            myfile << numSendInits[b] << "\t\t";
            sum += numSendInits[b];
        }
    
        if (numEchos.find(b) == numEchos.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numEchos[b] << "\t";
            sum += numEchos[b];
        }
        if (numEchoInits.find(b) == numEchoInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numEchoInits[b] << "\t";
            sum += numEchoInits[b];
        }
        if (numEchoLocalIds.find(b) == numEchoLocalIds.end()) {
            myfile << 0 << "\t\t";
        } else {
            myfile << numEchoLocalIds[b] << "\t\t";
            sum += numEchoLocalIds[b];
        }
        
        if (numReadys.find(b) == numReadys.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numReadys[b] << "\t";
            sum += numReadys[b];
        }
        if (numReadyInits.find(b) == numReadyInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numReadyInits[b] << "\t";
            sum += numReadyInits[b];
        }
        if (numReadyLocalIds.find(b) == numReadyLocalIds.end()) {
            myfile << 0 << "\t\t";
        } else {
            myfile << numReadyLocalIds[b] << "\t\t";
            sum += numReadyLocalIds[b];
        }
        
        if (numEchoEchos.find(b) == numEchoEchos.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numEchoEchos[b] << "\t";
            sum += numEchoEchos[b];
        }
        if (numEchoEchoInits.find(b) == numEchoEchoInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numEchoEchoInits[b] << "\t";
            sum += numEchoEchoInits[b];
        }
        if (numEchoEchoLocalIds.find(b) == numEchoEchoLocalIds.end()) {
            myfile << 0 << "\t\t";
        } else {
            myfile << numEchoEchoLocalIds[b] << "\t\t";
            sum += numEchoEchoLocalIds[b];
        }
        
        if (numReadyEchos.find(b) == numReadyEchos.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numReadyEchos[b] << "\t";
            sum += numReadyEchos[b];
        }
        if (numReadyEchoInits.find(b) == numReadyEchoInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numReadyEchoInits[b] << "\t";
            sum += numReadyEchoInits[b];
        }
        if (numReadyEchoLocalIds.find(b) == numReadyEchoLocalIds.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << numReadyEchoLocalIds[b] << "\t";
            sum += numReadyEchoLocalIds[b];
        }
        
        myfile << sum << "\n";
    }
    myfile.close();
    
    
    myfile.open(statsfilename+"bytesCounts_"+std::to_string(myid));
    myfile << "# bSends bSendInits bEchos bEchoInits  bEchoLocalIds bReadys bReadyInits bReadyLocalIds bEchoEchos bEchoEchoInits bEchoEchoLocalIds bReadyEchos bReadyEchoInits bReadyEchoLocalIds total\n";
    myfile << "# Payload was: " << payloadSize << "B\n";
    
    for (auto it = deliveredBroad.begin(); it != deliveredBroad.end(); it++) {

        bId b = {it->first.broadcasterId, it->first.broadcastId};
        
        int sum = 0;
        
        if (minBidMeasure > it->first.broadcastId or it->first.broadcastId >= maxBidMeasure) {continue;}
        
        myfile << "("<<it->first.broadcasterId << "," << it->first.broadcastId << ")\t";
        
        if (bytesSends.find(b) == bytesSends.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesSends[b] << "\t";
            sum += bytesSends[b];
        }
        if (bytesSendInits.find(b) == bytesSendInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesSendInits[b] << "\t";
            sum += bytesSendInits[b];
        }
        
        if (bytesEchos.find(b) == bytesEchos.end()) {
            myfile << 0 << "\t\t";
        } else {
            myfile << bytesEchos[b] << "\t\t";
            sum += bytesEchos[b];
        }
        if (bytesEchoInits.find(b) == bytesEchoInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesEchoInits[b] << "\t";
            sum += bytesEchoInits[b];
        }
        if (bytesEchoLocalIds.find(b) == bytesEchoLocalIds.end()) {
            myfile << 0 << "\t\t";
        } else {
            myfile << bytesEchoLocalIds[b] << "\t\t";
            sum += bytesEchoLocalIds[b];
        }
        
        if (bytesReadys.find(b) == bytesReadys.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesReadys[b] << "\t";
            sum += bytesReadys[b];
        }
        if (bytesReadyInits.find(b) == bytesReadyInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesReadyInits[b] << "\t";
            sum += bytesReadyInits[b];
        }
        if (bytesReadyLocalIds.find(b) == bytesReadyLocalIds.end()) {
            myfile << 0 << "\t\t";
        } else {
            myfile << bytesReadyLocalIds[b] << "\t\t";
            sum += bytesReadyLocalIds[b];
        }
        
        if (bytesEchoEchos.find(b) == bytesEchoEchos.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesEchoEchos[b] << "\t";
            sum += bytesEchoEchos[b];
        }
        if (bytesEchoEchoInits.find(b) == bytesEchoEchoInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesEchoEchoInits[b] << "\t";
            sum += bytesEchoEchoInits[b];
        }
        if (bytesEchoEchoLocalIds.find(b) == bytesEchoEchoLocalIds.end()) {
            myfile << 0 << "\t\t";
        } else {
            myfile << bytesEchoEchoLocalIds[b] << "\t\t";
            sum += bytesEchoEchoLocalIds[b];
        }
        
        if (bytesReadyEchos.find(b) == bytesReadyEchos.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesReadyEchos[b] << "\t";
            sum += bytesReadyEchos[b];
        }
        if (bytesReadyEchoInits.find(b) == bytesReadyEchoInits.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesReadyEchoInits[b] << "\t";
            sum += bytesReadyEchoInits[b];
        }
        if (bytesReadyEchoLocalIds.find(b) == bytesReadyEchoLocalIds.end()) {
            myfile << 0 << "\t";
        } else {
            myfile << bytesReadyEchoLocalIds[b] << "\t";
            sum += bytesReadyEchoLocalIds[b];
        }

        myfile << sum << "\n";
    }
    myfile.close();
}

std::chrono::milliseconds::rep OptBrachaDolevHandler::getCurrentTime() {
    const auto current_time_point {std::chrono::high_resolution_clock::now()};
    const auto current_time_since_epoch {current_time_point.time_since_epoch()};
    const auto current_microseconds {std::chrono::duration_cast<std::chrono::microseconds> (current_time_since_epoch).count()};
    return current_microseconds;
}

void OptBrachaDolevHandler::printBroadcastTime(PID broadcasterId, int broadcastId, bool firstWriteBroadcast) {
    std::ofstream myfile;
    if (firstWriteBroadcast) {
        myfile.open(statsfilename+"broadcastTimes_"+std::to_string(myid));
    } else {
        myfile.open(statsfilename+"broadcastTimes_"+std::to_string(myid), std::ios_base::app);
    }
    myfile << broadcasterId << "\t" << broadcastId << "\t" <<  getCurrentTime() << "\n";
    myfile.close();
}

void OptBrachaDolevHandler::printDeliverTime(PID broadcasterId, int broadcastId) {
    countDelivers++;
    
    if (countDelivers % writingIntervals == 0) {
        std::ofstream myfile;
        if (firstWrite) {
            myfile.open(statsfilename+"deliver_"+std::to_string(myid));
            firstWrite = false;
        } else {
            myfile.open(statsfilename+"deliver_"+std::to_string(myid), std::ios_base::app);
        }
        
        for (auto it = deliveredBroad.begin(); it != deliveredBroad.end(); it++) {
            if (minBidMeasure <= it->first.broadcastId and it->first.broadcastId < maxBidMeasure and it->second.second) {
                myfile << it->first.broadcasterId << "\t" << it->first.broadcastId << "\t" << it->second.first << "\n";
                it->second.second = false;
            }
        }
        myfile.close();
        
        printStats();
    }

//     if (countDelivers == (minBidMeasure + (maxBidMeasure-minBidMeasure)/2)*numNodes) {
//         printStats();
//     }
}

bool OptBrachaDolevHandler::isSelectedToSendEchos(PID broadcasterId) {
    if (!MBD_11) {
        return true;
    }
    
    bool result = false;
    int numSendingNodes = byzQuorumSize+numFaulty;
    if (broadcasterId+numSendingNodes <= numNodes) { 
        result = (broadcasterId <= myid and myid < broadcasterId+numSendingNodes);
    } else {
        result = (broadcasterId <= myid) or 
        (myid < broadcasterId+numSendingNodes-numNodes);
    }
    
//     bool res2 = false;
// // Equivalent to: 
//     for (int i = broadcasterId; i < broadcasterId+byzQuorumSize+numFaulty; i++) {
//         if (myid == (i % numNodes)) {
// //             std::cout << myid << " sends Echo" << std::endl;
//             res2 = true;
//             break;
//         }
//     }
// //     res2 = false;
//     if (result != res2) { 
//         std::cout << "DIFFERENT RESULT: " << myid << " "<<result<<" "<<res2<<std::endl;
//     }
    return result;
}

bool OptBrachaDolevHandler::isSelectedToSendReadys(PID broadcasterId) {
    if (!MBD_11) {
//         std::cout << myid << " sends Ready (MBD_11=0)" << std::endl;
        return true; 
    }
    int numSendingNodes = 2*numFaulty+1+numFaulty;
    if (broadcasterId+numSendingNodes <= numNodes) { 
        return (broadcasterId <= myid and myid < broadcasterId+numSendingNodes);
    } else {
        return (broadcasterId <= myid) or 
        (myid < broadcasterId+numSendingNodes-numNodes);
    }
// Equivalent to: 
//     for (int i = broadcasterId; i < broadcasterId+2*numFaulty+1+numFaulty; i++) {
//         if (myid == (i % numNodes)) {
// //             std::cout << myid << " sends Ready" << std::endl;
//             return true;
//         }
//     }
    return false;
}

void OptBrachaDolevHandler::consumeAllPending(const PID linkSenderId, const int localId, const PeerNet::conn_t &conn) {
    
    if (pendingReadyLocalId[linkSenderId].find(localId) != pendingReadyLocalId[linkSenderId].end()) {
        std::queue<ReadyLocalId> q = pendingReadyLocalId[linkSenderId][localId];
        while (!q.empty()) {
            ReadyLocalId m = q.front();
            q.pop();
            handle_readyLocalId(m, conn);
        }
    }
    
    if (pendingReadyLocalIdSingleHop[linkSenderId].find(localId) != pendingReadyLocalIdSingleHop[linkSenderId].end()) {
        std::queue<ReadyLocalIdSingleHop> q = pendingReadyLocalIdSingleHop[linkSenderId][localId];
        while (!q.empty()) {
            ReadyLocalIdSingleHop m = q.front();
            q.pop();
            handle_readyLocalIdSingleHop(m, conn);
        }
    }

    if (pendingReadyEchoLocalId[linkSenderId].find(localId) != pendingReadyEchoLocalId[linkSenderId].end()) {
        std::queue<ReadyEchoLocalId> q = pendingReadyEchoLocalId[linkSenderId][localId];
        while (!q.empty()) {
            ReadyEchoLocalId m = q.front();
            q.pop();
            handle_readyEchoLocalId(m, conn);
        }
    }
    
    if (pendingEchoLocalId[linkSenderId].find(localId) != pendingEchoLocalId[linkSenderId].end()) {
        std::queue<EchoLocalId> q = pendingEchoLocalId[linkSenderId][localId];
        while (!q.empty()) {
            EchoLocalId m = q.front();
            q.pop();
            handle_echoLocalId(m, conn);
        }
    }
    
    if (pendingEchoLocalIdSingleHop[linkSenderId].find(localId) != pendingEchoLocalIdSingleHop[linkSenderId].end()) {
        std::queue<EchoLocalIdSingleHop> q = pendingEchoLocalIdSingleHop[linkSenderId][localId];
        while (!q.empty()) {
            EchoLocalIdSingleHop m = q.front();
            q.pop();
            handle_echoLocalIdSingleHop(m, conn);
        }
    }
    
    if (pendingEchoEchoLocalId[linkSenderId].find(localId) != pendingEchoEchoLocalId[linkSenderId].end()) {
        std::queue<EchoEchoLocalId> q = pendingEchoEchoLocalId[linkSenderId][localId];
        while (!q.empty()) {
            EchoEchoLocalId m = q.front();
            q.pop();
            handle_echoEchoLocalId(m, conn);
        }
    }
}


void OptBrachaDolevHandler::send_multiple_broadcasts() {
  // Send a Dolev msg to all other peers
//   if (myid!=0) return;
  bool firstBroadcastWrite = true;
  usleep(this->sleepTime);
  for (int i = 0; i < this->numBcasts; i++) {
    if (i % numNodes == myid) {
        printBroadcastTime(myid, myBroadcastId, firstBroadcastWrite);
        init_broadcast();
    }
    firstBroadcastWrite = false;
    usleep(this->sleepTime);
  }
}

OptBrachaDolevHandler::OptBrachaDolevHandler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename, int payloadSize, bool mod1, bool mod2, bool mod3, bool mod4, bool mod5, unsigned int numBcasts, unsigned int sleepTime, int minBidMeasure, int maxBidMeasure, bool MBD_1, bool MBD_2, bool MBD_3, bool MBD_4, bool MBD_5, bool MBD_6, bool MBD_7, bool MBD_8, bool MBD_9, bool MBD_10, bool MBD_11, bool MBD_12, int writingIntervals) : Handler(id,pconf,nodes,topofilename) {
    
    this->payloadSize = payloadSize; 
    
    MOD_1_BONOMI = mod1;
    MOD_2_BONOMI = mod2;
    MOD_3_BONOMI = mod3;
    MOD_4_BONOMI = mod4;
    MOD_5_BONOMI = mod5;
    
    this->MBD_1 = MBD_1;
    this->MBD_2 = MBD_2;
    this->MBD_3 = MBD_3;
    this->MBD_4 = MBD_4;
    this->MBD_5 = MBD_5;
    this->MBD_6 = MBD_6;
    this->MBD_7 = MBD_7;
    this->MBD_8 = MBD_8;
    this->MBD_9 = MBD_9;
    this->MBD_10 = MBD_10;
    this->MBD_11 = MBD_11;
    this->MBD_12 = MBD_12;
    
    this->numBcasts = numBcasts;
    this->sleepTime = sleepTime;
    this->minBidMeasure = minBidMeasure;
    this->maxBidMeasure = maxBidMeasure;
    
    this->writingIntervals = writingIntervals;
    
    // Binding message types to corresponding handling functions 
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echo, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echoInit, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echoLocalId, this, _1, _2));
          
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echoSingleHop, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echoInitSingleHop, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echoLocalIdSingleHop, this, _1, _2));
    
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_ready, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_readyInit, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_readyLocalId, this, _1, _2));
    
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_readySingleHop, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_readyInitSingleHop, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_readyLocalIdSingleHop, this, _1, _2));
    
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echoEcho, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echoEchoInit, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_echoEchoLocalId, this, _1, _2));
    
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_readyEcho, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_readyEchoInit, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_readyEchoLocalId, this, _1, _2));
    

    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_sendInit, this, _1, _2));
    this->pnet.reg_handler(salticidae::generic_bind(&OptBrachaDolevHandler::handle_send, this, _1, _2));
    
    usleep(5000000); // in microseconds 

    send_thread = std::thread([this]() { send_multiple_broadcasts(); });

    auto pshutdown = [&](int) {pec.stop();};
    salticidae::SigEvent pev_sigterm(pec, pshutdown);
    pev_sigterm.add(SIGTERM);

    pec.dispatch();
}

void OptBrachaDolevHandler::init_broadcast() {

    std::cout << KCYN <<"\n"<< myid << " broadcasts ("<<myid<<","<<myBroadcastId<<")"<< KNRM << std::endl;
    mId echoMsgId = {HDR_ECHO, myid, myBroadcastId, myid}; 
    bId broadId = {myid, myBroadcastId};
    
    rcvEchos[broadId].insert(myid);
    deliveredMsgs.insert(echoMsgId);
    
    char * val = new char[payloadSize];
    for (int i = 0; i < payloadSize; i++) {
        val[i] = 'a' + rand() % 26;
    }
    
    if (MBD_1) { 
        // Choose a new localId for this broadcast
        struct localInfo li = {myid, myBroadcastId, payloadSize, val};
        int localId = myBroadcastId;
        pidToLocalInfo[myid][localId] = li;
        bidToLocalId[broadId] = localId;
        
        pidsThatKnowLocalId[localId].reset();
  
        if (MBD_5) { 
            
            SendInit m(payloadSize, val, this->myid, this->myBroadcastId, localId);
            
            int count = 0;
            for (int j = 0; j < selfNeighbors.size(); j++) { 
                PID cur = selfNeighbors[j];
                if (MBD_12 and count >= 2*numFaulty+1) {break;}
                this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[cur]));

                pidsThatKnowLocalId[localId].set(cur);
                count++;
            }
            
        } else {
        
            EchoInitSingleHop m(payloadSize, val, this->myid, this->myid, this->myBroadcastId, localId);
            
            int count = 0;
            int numToSendTo = selfNeighbors.size();
            if (MBD_12 and 2*numFaulty+1 < selfNeighbors.size()) {
                numToSendTo = 2*numFaulty+1;
            }
            for (int j = 0; j < numToSendTo; j++) { 
                PID cur = selfNeighbors[j];
                this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[cur]));
                pidsThatKnowLocalId[localId].set(cur);
                count++;
            }
        }
        
    } else { // !MBD_1
        
        if (MBD_5) {
            Send m(payloadSize, val, this->myid, this->myBroadcastId);
            
            int count = 0;
            for (int j = 0; j < selfNeighbors.size(); j++) { 
                PID cur = selfNeighbors[j];
                if (MBD_12 and count >= 2*numFaulty+1) {break;}
                this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[cur]));
                count++;
            }
        } else {
            EchoSingleHop m(payloadSize, val, this->myid, this->myid, this->myBroadcastId);
            
            int count = 0;
            for (int j = 0; j < selfNeighbors.size(); j++) { 
                PID cur = selfNeighbors[j];
                if (MBD_12) {
                    if (count >= 2*numFaulty+1) {break;}
                    count++;
                }
                this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[cur]));
            }
        }
    }

    myBroadcastId++;
}

bool OptBrachaDolevHandler::send_echo(struct mId msgId, const char * val) {
    
    if (!isSelectedToSendEchos(msgId.broadcasterId)) {return false;}
    
    if (DEBUG) 
        std::cout << KGRN << "\t" << myid << " sends own ECHO" << KNRM << std::endl;
    
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    struct mId echoMsgId = {HDR_ECHO, msgId.broadcasterId, msgId.broadcastId, myid};
    rcvEchos[broadId].insert(myid);
    deliveredMsgs.insert(echoMsgId); 
    
    int localId = 0;
    if (MBD_1) {
        localId = bidToLocalId[broadId];
    } 

    EchoInitSingleHop d1(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, localId);
    EchoLocalIdSingleHop d2(this->myid, localId);
    EchoSingleHop m(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId);
    
    int count = 0;
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID cur = selfNeighbors[j];
        
        if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(cur) != neighborsToAvoid[broadId].end()) {continue;}
        
        if (MBD_8 and contains(rcvReadys[broadId], cur)) {continue;}
        
        if (MBD_1) {
            if (not pidsThatKnowLocalId[localId][cur]) {
                this->pnet.send_msg(d1, (salticidae::PeerId) std::get<1>(peers[cur]));
                pidsThatKnowLocalId[localId].set(cur);
            } else { // send Echo_localId
                this->pnet.send_msg(d2, (salticidae::PeerId) std::get<1>(peers[cur]));
            }
        } else {
            this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[cur]));
        }
        count++;
        if (MBD_12 and count >= 2*numFaulty+1) {
            break;
        }
    }
    
    return true;
}

void OptBrachaDolevHandler::send_echoEcho(struct mId msgId, const char * val, std::vector<PID> path) {

    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    // Deliver own Echo
    rcvEchos[broadId].insert(myid);
    struct mId echoMsgId = {HDR_ECHO, msgId.broadcasterId, msgId.broadcastId, myid};
    deliveredMsgs.insert(echoMsgId); 
    
    EchoEcho mee(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, msgId.echoOrReadySender, this->myid, emptyPath);
    
    EchoSingleHop me(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId); 
    
    int localId = 0;
    if (MBD_1) {
        localId = bidToLocalId[broadId];
    } 
    
    EchoInitSingleHop mei(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, localId);
    EchoLocalIdSingleHop melid(this->myid, localId);

    EchoEchoInit meei(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, localId, msgId.echoOrReadySender, this->myid, emptyPath);
    
    EchoEchoLocalId meelid(this->myid, localId, msgId.echoOrReadySender, this->myid, emptyPath);
    
    // Nodes in path should only receive the new Echo
    int count = 0;
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID cur = selfNeighbors[j];
//         std::cout << myid << " considers " << cur << " in sendEchoEcho" << std::endl;
        
        if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(cur) != neighborsToAvoid[broadId].end()) {continue;}
        
        if (MBD_8 and contains(rcvReadys[broadId], cur)) {continue;}

        if (cur == msgId.echoOrReadySender) { // only new echo
//             std::cout << "send only new echo to " << cur << std::endl;
            if (MBD_1) {
                if (not pidsThatKnowLocalId[localId][cur]) { 
                    this->pnet.send_msg(mei, (salticidae::PeerId) std::get<1>(peers[cur]));
                    pidsThatKnowLocalId[localId].set(cur);
                } else { // send Echo_localId
                    this->pnet.send_msg(melid, (salticidae::PeerId) std::get<1>(peers[cur]));
                }
            } else {
                this->pnet.send_msg(me, (salticidae::PeerId) std::get<1>(peers[cur]));
            }
            count++;
        } else { // both echos can be relayed
//             std::cout << "send echoEcho to " << cur << std::endl;
            if (MBD_1) {
                if (not pidsThatKnowLocalId[localId][cur]) {
                    this->pnet.send_msg(meei, (salticidae::PeerId) std::get<1>(peers[cur]));
                    pidsThatKnowLocalId[localId].set(cur);
                } else { // send Echo_localId
                    this->pnet.send_msg(meelid, (salticidae::PeerId) std::get<1>(peers[cur]));
                }
            } else {
                this->pnet.send_msg(mee, (salticidae::PeerId) std::get<1>(peers[cur]));
            }
            count++;
        }
        if (MBD_12 and count >= 2*numFaulty + 1) { 
            break;
        }
    }

}

void OptBrachaDolevHandler::send_readyEcho(struct mId msgId, const char * val, std::vector<PID> path) {
    
    if (DEBUG) std::cout << KGRN << "\t" << myid << " sends own READY_ECHO" << KNRM << std::endl;
    
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    struct mId readyMsgId = {HDR_READY, msgId.broadcasterId, msgId.broadcastId, myid};
    rcvReadys[broadId].insert(myid);
    deliveredMsgs.insert(readyMsgId);

    struct mId echoMsgId = {HDR_ECHO, msgId.broadcasterId, msgId.broadcastId, myid};
    rcvEchos[broadId].insert(myid);
    deliveredMsgs.insert(echoMsgId);
    
    int localId = 0;
    if (MBD_1) {
        localId = bidToLocalId[broadId];
    } 
    
    // Some nodes only need the Ready
    ReadyInitSingleHop r1(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, localId);
    ReadyLocalIdSingleHop r2(this->myid, localId);
    ReadySingleHop r3(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId);  

    // Others might use both Ready and Echo
    ReadyEchoInit re4(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, localId, msgId.echoOrReadySender, this->myid, emptyPath);
    ReadyEchoLocalId re5(this->myid, localId, msgId.echoOrReadySender, this->myid, emptyPath);
    ReadyEcho re6(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, msgId.echoOrReadySender, this->myid, emptyPath);
    
    // Nodes in path should only receive the new Ready
    int count = 0;
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID cur = selfNeighbors[j];
        
        if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(cur) != neighborsToAvoid[broadId].end()) {continue;}

        if (cur == msgId.echoOrReadySender or (MBD_8 and contains(rcvReadys[broadId], cur))) { // only new ready
            
            if (MBD_1) {
                if (not pidsThatKnowLocalId[localId][cur]) { 
                    this->pnet.send_msg(r1, (salticidae::PeerId) std::get<1>(peers[cur]));
                    pidsThatKnowLocalId[localId].set(cur);
                } else { // send Echo_localId
                    this->pnet.send_msg(r2, (salticidae::PeerId) std::get<1>(peers[cur]));
                }
            } else {
                this->pnet.send_msg(r3, (salticidae::PeerId) std::get<1>(peers[cur]));
            }
            count++;
        } else { // forward both the Ready and Echo
            
            if (MBD_1) {
                if (not pidsThatKnowLocalId[localId][cur]) {
                    this->pnet.send_msg(re4, (salticidae::PeerId) std::get<1>(peers[cur]));
                    pidsThatKnowLocalId[localId].set(cur);
                } else { // send Echo_localId
                    this->pnet.send_msg(re5, (salticidae::PeerId) std::get<1>(peers[cur]));
                }
            } else {
                this->pnet.send_msg(re6, (salticidae::PeerId) std::get<1>(peers[cur]));
            }
            count++;
        } 
        if (MBD_12 and count >= 2*numFaulty+1) { 
            break;
        }
    }
}

bool OptBrachaDolevHandler::send_ready(struct mId msgId, const char * val) {

    if (!isSelectedToSendReadys(msgId.broadcasterId)) {return false;}
    
    if (DEBUG) 
        std::cout << KGRN << "\t" << myid << " sends own READY" << KNRM << std::endl;
    
    struct bId broadId = {msgId.broadcasterId, msgId.broadcastId};
    
    int localId = 0;
    if (MBD_1) {
        localId = bidToLocalId[broadId];
    } 
    
    ReadyInitSingleHop d1(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId, localId);
    ReadyLocalIdSingleHop d2(this->myid, localId);
    ReadySingleHop m(payloadSize, val, this->myid, msgId.broadcasterId, msgId.broadcastId);
    
    int count = 0;
    for (int j = 0; j < selfNeighbors.size(); j++) { 
        PID cur = selfNeighbors[j];
        
        if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(cur) != neighborsToAvoid[broadId].end()) {continue;}
        
        if (MBD_1) {
            if (not pidsThatKnowLocalId[localId][cur]) { 
                this->pnet.send_msg(d1, (salticidae::PeerId) std::get<1>(peers[cur]));
                pidsThatKnowLocalId[localId].set(cur);
            } else { // send Ready_localId
                this->pnet.send_msg(d2, (salticidae::PeerId) std::get<1>(peers[cur]));
            }
            count++;
        } else {
            this->pnet.send_msg(m, (salticidae::PeerId) std::get<1>(peers[cur]));
            count++;
        }
        
        if (MBD_12 and count >= 2*numFaulty+1) {
            break;
        }
    }
    
    struct mId readyMsgId = {HDR_READY, msgId.broadcasterId, msgId.broadcastId, myid};
    rcvReadys[broadId].insert(myid);
    deliveredMsgs.insert(readyMsgId); 
    
    // New 
    struct mId echoMsgId = {HDR_ECHO, msgId.broadcasterId, msgId.broadcastId, myid};
    rcvEchos[broadId].insert(myid); 
    deliveredMsgs.insert(echoMsgId); 
    
    return true;
}

// std::map<struct mId, std::vector<std::pair<std::string, int>> *> allNonIntersectingSets; 
bool OptBrachaDolevHandler::recomputeAllNonIntersectingSets(struct mId msgId, std::string newPath) {
    
    int threshold = numFaulty + 1;
    
    long long newPathBin = 0; 
    for (int i = 0; i < newPath.length(); i++) { 
        if (newPath[i]) { 
            newPathBin |= (1 << i);
        }
    }
    
    if (allNonIntersectingSets.find(msgId) == allNonIntersectingSets.end()) { 
        if (threshold == 1) {
            return true; 
        } else {
            allNonIntersectingSets[msgId] = new std::vector<std::pair<long long, int>>();
            allNonIntersectingSets[msgId]->push_back(std::make_pair<long long, int>(std::move(newPathBin), 1));
        }
    } else {
        std::vector<std::pair<long long, int>> newSets;
        std::vector<std::pair<long long, int>> * cur_allNonIntersectingSets = allNonIntersectingSets[msgId];
        
        for (int i = 0; i < cur_allNonIntersectingSets->size(); i++) {
            
            long long prevCombination = (*cur_allNonIntersectingSets)[i].first;
            int numPaths = (*cur_allNonIntersectingSets)[i].second;
            
            if (newPathBin ^ prevCombination == 0) { // to test if sets are disjoint
                long long unionBin = newPathBin | prevCombination;  
                
                newSets.push_back(std::make_pair<long long, int>(std::move(unionBin), numPaths + 1));
                if (numPaths+1 >= threshold) { 
//                     delete(allNonIntersectingSets[msgId]);
                    return true;
                }
            } 
        }
        
        for (int i = 0; i < newSets.size(); i++) { 
            allNonIntersectingSets[msgId]->push_back(newSets[i]);
        }
        allNonIntersectingSets[msgId]->push_back(std::make_pair<long long, int>(std::move(newPathBin), 1));
    }
    
    return false; 
}

bool OptBrachaDolevHandler::msgCanBeDelivered(struct mId msgId, PID linkSenderId) {
    
    auto start = getCurrentTime();
    
    // Check if (f+1) disjoint paths can be identified
    int threshold = numFaulty + 1;
    
    int countNonEmpty = 0;
    for (int i = 0; i < numNodes; i++) {
        if (pathsBitmasks[msgId][i].size()>0) {
            countNonEmpty++;
        }
    }
    if (countNonEmpty < threshold) {
        return false;
    }
    
//     if (myid == 1) { 
//     for (int i = 0; i < numNodes; i++) {
//         if (pathsBitmasks[msgId][i].size()>0) {
//             std::cout << "i: " << std::to_string(i) << std::endl;
//             for (auto& it : pathsBitmasks[msgId][i]) {
//                 std::cout << "path:";
//                 for (int i = 0; i < numNodes; i++) {
//                     if (it[i] == 0)
//                         std::cout << '0';
//                     else
//                         std::cout << '1';
//                 }                                          
//                 std::cout << std::endl;
//             }
//         }
//     }
//     }
    
    std::string sourceNeighborsBitmask(threshold-1, 1); 
    sourceNeighborsBitmask.resize(numNodes, 0);
    // used to generate all possible combinations of f+1 nodes
    
    do {
        
        if (sourceNeighborsBitmask[linkSenderId]==1) {
            continue;
        } 
        std::string neighborsBitmask(sourceNeighborsBitmask);
        neighborsBitmask[linkSenderId] = 1;
        
        int selectedIds[threshold]; // stores the ids of the f+1 nodes whose paths will be examined 
        bool invalidNodes = false;
        int cur = 0;
        int senderPosInSelected = -1;
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
        
        if (invalidNodes or senderPosInSelected==-1) {continue;}
        
        // TODO: would be (much) better to generate combinations that only involves the newly received message
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
                auto endt = getCurrentTime();
//                 std::cout << myid << " (true) time = " << (endt-start)/1000 <<"ms"<< std::endl;
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
        
    } while (std::prev_permutation(sourceNeighborsBitmask.begin(), sourceNeighborsBitmask.end()));
    
    auto endt = getCurrentTime();
//     std::cout << myid << " (false) time = " << (endt-start)/1000 << "ms"<< std::endl;
    return false;
}


void OptBrachaDolevHandler::handle_sendInit(SendInit msg, const PeerNet::conn_t &conn) {

    bId broadId = {msg.linkSenderId, msg.broadcastId};

    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numSendInits.find(broadId) == numSendInits.end()) {
            numSendInits[broadId] = 1;
            bytesSendInits[broadId] = msg.sizeMsg();
        } else {
            numSendInits[broadId]++;
            bytesSendInits[broadId] += msg.sizeMsg();
        }
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    std::vector<unsigned int> e;
    handle_echoInit_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.linkSenderId, msg.broadcastId, msg.localId, msg.linkSenderId, e, 0, false);
    
    consumeAllPending(msg.linkSenderId, msg.localId, conn);
}

void OptBrachaDolevHandler::handle_send(Send msg, const PeerNet::conn_t &conn) {

    bId broadId = {msg.linkSenderId, msg.broadcastId};

    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numSends.find(broadId) == numSends.end()) {
            numSends[broadId] = 1;
            bytesSends[broadId] = msg.sizeMsg();
        } else {
            numSends[broadId]++;
            bytesSends[broadId] += msg.sizeMsg();
        }  
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    std::vector<unsigned int> e; 
    handle_echo_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.linkSenderId, msg.broadcastId, msg.linkSenderId, e, 0, false);
}

void OptBrachaDolevHandler::handle_echo(Echo msg, const PeerNet::conn_t &conn) {
    
//     checkPath(msg); // can be removed in optimized version
    
    handle_echo_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.echoSender, msg.path, msg.sizeMsg(), true);
}

void OptBrachaDolevHandler::handle_echoSingleHop(EchoSingleHop msg, const PeerNet::conn_t &conn) {
    
    bId broadId = {msg.linkSenderId, msg.broadcastId};
    
    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numEchos.find(broadId) == numEchos.end()) {
            numEchos[broadId] = 1;
            bytesEchos[broadId] = msg.sizeMsg();
        } else {
            numEchos[broadId]++;
            bytesEchos[broadId] += msg.sizeMsg();
        }  
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    }

    std::vector<unsigned int> e;
    handle_echo_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.linkSenderId, e, 0, false);
}

void OptBrachaDolevHandler::handle_echoInitSingleHop(EchoInitSingleHop msg, const PeerNet::conn_t &conn) {
    bId broadId = {msg.linkSenderId, msg.broadcastId};
    
    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numEchos.find(broadId) == numEchos.end()) {
            numEchos[broadId] = 1;
            bytesEchos[broadId] = msg.sizeMsg();
        } else {
            numEchos[broadId]++;
            bytesEchos[broadId] += msg.sizeMsg();
        }  
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    std::vector<unsigned int> e;
    handle_echoInit_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.localId, msg.linkSenderId, e, 0, false);
    
    consumeAllPending(msg.linkSenderId, msg.localId, conn);
}

void OptBrachaDolevHandler::handle_echoLocalIdSingleHop(EchoLocalIdSingleHop msg, const PeerNet::conn_t &conn) {
    
    if (pidToLocalInfo.find(msg.linkSenderId) == pidToLocalInfo.end() or pidToLocalInfo[msg.linkSenderId].find(msg.localId) == pidToLocalInfo[msg.linkSenderId].end()) {
        pendingEchoLocalIdSingleHop[msg.linkSenderId][msg.localId].push(msg);
        return;
    }
    
    localInfo li = pidToLocalInfo[msg.linkSenderId][msg.localId];

    
    struct bId broadId = {li.broadcasterId, li.broadcastId};
    if (minBidMeasure <= li.broadcastId and li.broadcastId < maxBidMeasure) {
        if (numEchoLocalIds.find(broadId) == numEchoLocalIds.end()) {
            numEchoLocalIds[broadId] = 1;
            bytesEchoLocalIds[broadId] = msg.sizeMsg();
        } else {
            numEchoLocalIds[broadId]++;
            bytesEchoLocalIds[broadId] += msg.sizeMsg();
        }  
        if (li.broadcastId == numBcasts-1) {
            printStats();
        }
    }  

    std::vector<unsigned int> e;
    handle_echo_2(li.payloadSize, li.val, msg.linkSenderId, li.broadcasterId,  li.broadcastId, msg.linkSenderId, e, 0, false);
}

void OptBrachaDolevHandler::handle_echo_2(const unsigned int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &echoSender, const std::vector<PID> &msgPath, const unsigned int &sizeMsg, const bool doStats) {

    if (DEBUG) {
        std::cout<<linkSenderId<<" -> "<<myid<<": ECHO("<<echoSender<<"), path [";
        for (int i = 0; i < msgPath.size(); i++) { 
            std::cout << msgPath[i];
            if (i != msgPath.size()-1) { std::cout << ", ";} 
        }
        std::cout << "]" << std::endl;
    }
    
    bId broadId = {broadcasterId, broadcastId};
        
    if (doStats and minBidMeasure <= broadcastId and broadcastId < maxBidMeasure) {
        if (numEchos.find(broadId) == numEchos.end()) {
            numEchos[broadId] = 1;
            bytesEchos[broadId] = sizeMsg;
        } else {
            numEchos[broadId]++;
            bytesEchos[broadId] += sizeMsg;
        }  
        if (broadcastId == numBcasts-1) {
            printStats();
        }
    }

    mId echoMsgId = {HDR_ECHO, broadcasterId, broadcastId, echoSender};

    // if corresponding Ready delivered then return
    if (MBD_6) {
        struct mId readyMsgId = {HDR_READY, broadcasterId, broadcastId, echoSender};
        if (deliveredMsgs.find(readyMsgId) != deliveredMsgs.end()) {return;}
    }
    
    // if global broadcast delivered then return
    if (MBD_7 and deliveredBroad.find(broadId) != deliveredBroad.end()) {return;}
    
    bool wasDelivered = (deliveredMsgs.find(echoMsgId) != deliveredMsgs.end());
         
    if (MOD_5_BONOMI and wasDelivered) {return;} // ignore if already delivered
    
    if (MOD_4_BONOMI) { // if empty path from q, store it, and will ignore paths that contain q in the future
        if (neighborsThatDelivered.find(echoMsgId) != neighborsThatDelivered.end()) {
            for (int i = 0; i < msgPath.size(); i++) { 
                if (neighborsThatDelivered[echoMsgId].find(msgPath[i]) != neighborsThatDelivered[echoMsgId].end()) {
                    return;
                }
            }
        }
        if (msgPath.size() == 0) {
            neighborsThatDelivered[echoMsgId].insert(linkSenderId);
        }
    }
	
    // Assemble path: rcvd path + link sender Id
    std::vector<PID> path;
    std::string pathBitmask = std::string(numNodes, 0);
    for (int i = 0; i < msgPath.size(); i++) { 
        path.push_back(msgPath[i]);
        pathBitmask[msgPath[i]] = 1;
    }
    if (linkSenderId != echoSender) {
        path.push_back(linkSenderId);
        pathBitmask[linkSenderId] = 1; // no need to add echoSender (would appear everywhere)
    }
    
    if (pathsBitmasks.find(echoMsgId) == pathsBitmasks.end()) {
        pathsBitmasks[echoMsgId] = new std::vector<std::string>[numNodes];
    } 
    
    // check if received echo path is a superpath of known path
    if (MBD_10) { 
         for (int j = 0; j < selfNeighbors.size(); j++) {
            PID sender = selfNeighbors[j];
            for (int j = 0; j < pathsBitmasks[echoMsgId][sender].size(); j++) {
                std::string prevBitmask = pathsBitmasks[echoMsgId][sender][j];
                bool isSuperPath = true; 
                for (int k = 0; k < numNodes; k++) { // index in bitmask
                    if (pathBitmask[k] and !prevBitmask[k]) {
                        isSuperPath = false;
                    }
                }
                if (!prevBitmask[echoSender]) {
                    isSuperPath = false;
                }
                if (isSuperPath) {
                    return;
                }
            }
        }
    }
    // Insert path into data structure
    pathsBitmasks[echoMsgId][linkSenderId].push_back(pathBitmask);
    
    bool deliveredSource = false;
    
    // Direct deliver? 
    bool directDeliver = MOD_1_BONOMI and linkSenderId == echoSender;
    
    // Indirect Dolev-deliver ? 
    bool indirectDeliver; 
    if (precomputeSets) {
        std::string fullPathBitmask(pathBitmask);
        fullPathBitmask[linkSenderId] = 1;
        indirectDeliver = (!wasDelivered and !directDeliver and recomputeAllNonIntersectingSets(echoMsgId, fullPathBitmask));
    } else {
        indirectDeliver = (!wasDelivered and !directDeliver and msgCanBeDelivered(echoMsgId, linkSenderId)); 
    }
    
    bool sentReadyEcho = false;
    bool sentEchoEcho = false;
    
    mId sourceMsgId = {HDR_ECHO, broadcasterId, broadcastId, broadcasterId};
        
    bool wasDeliveredSource = (deliveredMsgs.find(sourceMsgId) != deliveredMsgs.end()); 
    
    bool indirectDeliverSource = false;
    
    if (!wasDeliveredSource and myid != broadcasterId and echoSender != broadcasterId and linkSenderId != broadcasterId) {
        
        bool sourceInPath = false;
        for (int i = 0; i < msgPath.size(); i++) {
            if (msgPath[i] == broadcasterId) {
                sourceInPath = true;
                break;
            }
        }
        
        if (!sourceInPath) {
            std::vector<PID> sourcePath; // note that broadcasterId not added to path
            sourcePath.push_back(echoSender); // != broadcasterId
            for (int i = 0; i < msgPath.size(); i++) {
                sourcePath.push_back(msgPath[i]);
            }
            if (linkSenderId != echoSender) {
                sourcePath.push_back(linkSenderId);
            }
            
            std::string sourcePathBitmask = std::string(numNodes, 0);
            for (int i = 0; i < sourcePath.size(); i++) { 
                sourcePathBitmask[sourcePath[i]] = 1;
            }
            
            if (pathsBitmasks.find(sourceMsgId) == pathsBitmasks.end()) {
                pathsBitmasks[sourceMsgId] = new std::vector<std::string>[numNodes];
            } 
            pathsBitmasks[sourceMsgId][linkSenderId].push_back(sourcePathBitmask);
            
            if (precomputeSets) { 
                std::string fullPathBitmask(sourcePathBitmask);
                fullPathBitmask[linkSenderId] = 1;
                indirectDeliverSource = recomputeAllNonIntersectingSets(sourceMsgId, fullPathBitmask);
            } else {
                indirectDeliverSource = msgCanBeDelivered(sourceMsgId, linkSenderId);
            }
            
            if (indirectDeliverSource) { 
//                 if (myid==27) std::cout << KGRN<<"\t"<<myid << " delivers source Echo" << KNRM<<std::endl;
                deliveredMsgs.insert(sourceMsgId);
                rcvEchos[broadId].insert(broadcasterId);
            }
        }
    }
    
    bool sentEcho = false;
    bool sentReady = false;
    
    if (!wasDelivered and (directDeliver or indirectDeliver)) {
        
        deliveredMsgs.insert(echoMsgId); 
        rcvEchos[broadId].insert(echoSender);
        
        if (DEBUG) {
            std::cout<<KRED<<"\t"<<myid<<" delivers ECHO("<<echoSender<<") and has "<<rcvEchos[broadId].size()<<"/"<<(numFaulty+1)<<" ECHOs [";
            std::set<unsigned int>::iterator it;
            int a = 0;
            for (it=rcvEchos[broadId].begin(); it!=rcvEchos[broadId].end(); ++it) {
                if (a!=0) std::cout << ",";
                std::cout << *it;
                a++;
            }
            std::cout <<"]" << KNRM<<std::endl;
        }
        
        // Generate own Ready? 
        // possible to skip directly to sending ready
        if (isSelectedToSendReadys(broadcasterId) and !contains(rcvReadys[broadId], myid) and 
            (rcvEchos[broadId].size() >= byzQuorumSize or rcvReadys[broadId].size() >= numFaulty+1)) {
            if (DEBUG) std::cout<<KCYN<<"\t"<<myid<<" is READY"<<KNRM<<std::endl;
            if (MBD_4 and echoSender != broadcasterId) {
                send_readyEcho(echoMsgId, val, path);
                sentReadyEcho = true;// own ready + rcv echo
            } else {
                sentReady = send_ready(echoMsgId, val);
            }
        } // Or else try to send Echo?
        else if (isSelectedToSendEchos(broadcasterId) and !contains(rcvEchos[broadId], myid) and 
            (contains(rcvEchos[broadId], broadcasterId) or rcvEchos[broadId].size() >= numFaulty + 1) ) {   
            if (MBD_3 and echoSender != broadcasterId) {
                send_echoEcho(echoMsgId, val, path); 
                sentEchoEcho = true;
            } else {
                sentEcho = send_echo(echoMsgId, val); // own echo
            }
        }
    } else if (!wasDeliveredSource and indirectDeliverSource) {
        
        if (isSelectedToSendReadys(broadcasterId) and !contains(rcvReadys[broadId], myid) and 
            (rcvEchos[broadId].size() >= byzQuorumSize or rcvReadys[broadId].size() >= numFaulty+1)) {
            if (MBD_4 and echoSender != broadcasterId) {
                send_readyEcho(sourceMsgId, val, path);
                sentReadyEcho = true;// own ready + rcv echo
            } else {
                sentReady = send_ready(sourceMsgId, val);
            }
        } // Or else try to send Echo?
        else if (isSelectedToSendEchos(broadcasterId) and !contains(rcvEchos[broadId], myid) and 
            (contains(rcvEchos[broadId], broadcasterId) or rcvEchos[broadId].size() >= numFaulty + 1) ) {   
            if (MBD_3 and echoSender != broadcasterId) {
                send_echoEcho(sourceMsgId, val, path); 
                sentEchoEcho = true;
            } else {
                sentEcho = send_echo(sourceMsgId, val); // own echo
            }
        }
        
    }
    
    // About to forward the received Echo, not necessary if already done as part of ReadyEcho or EchoEcho
    if (sentReadyEcho or sentEchoEcho) return;
        
//         or ((sentEcho ) and echoSender == broadcasterId)) {return;}
    
    int localId = 0; 
    if (MBD_1) {
        localId = bidToLocalId[broadId];
    } 
    
    EchoInit d1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, echoSender, emptyPath);
    EchoLocalId d2(this->myid, localId, echoSender, emptyPath);
    Echo d3(payloadSize, val, this->myid, broadcasterId, broadcastId, echoSender, emptyPath);
    
    EchoInit d4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, echoSender, path);
    EchoLocalId d5(this->myid, localId, echoSender, path);
    Echo d6(payloadSize, val, this->myid, broadcasterId, broadcastId, echoSender, path);
    
    // Relay Echo message with modified path to neighbors
//     if (MBD_11 or !(MBD_2 and linkSenderId == broadcasterId)) { // avoid relaying send msgs
    if (!isSelectedToSendEchos(broadcasterId) or !MBD_2 or (MBD_2 and echoSender != broadcasterId)) {
        for (int i = 0; i < selfNeighbors.size(); i++) {
            PID curNeighbor = selfNeighbors[i];
            if (curNeighbor == echoSender) {continue;}
            
            if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(curNeighbor) != neighborsToAvoid[broadId].end()) {continue;}
            
            if (MBD_8 and 
                rcvReadys.find(broadId)!=rcvReadys.end() and
                contains(rcvReadys[broadId], curNeighbor)) {continue;}
            
            // Send empty path upon delivery
            if (MOD_2_BONOMI and (directDeliver or indirectDeliver)) {
                
                if (MBD_1) {
                    if (not pidsThatKnowLocalId[localId][curNeighbor]) {
                        this->pnet.send_msg(d1, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        pidsThatKnowLocalId[localId].set(curNeighbor);
                    } else { // send Echo_localId
                        this->pnet.send_msg(d2, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                } else {
                    this->pnet.send_msg(d3, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                }
        
            } else { // else relay with modified path
                if (!pathBitmask[curNeighbor] and curNeighbor!=linkSenderId) {
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][curNeighbor]) {
                            this->pnet.send_msg(d4, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                            pidsThatKnowLocalId[localId].set(curNeighbor);
                        } else { // send Echo_localId
                            this->pnet.send_msg(d5, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        }
                    } else {
                        this->pnet.send_msg(d6, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                }
            }
        }
    }

}

void OptBrachaDolevHandler::handle_echoInit(EchoInit msg, const PeerNet::conn_t &conn) {
    
    handle_echoInit_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.localId, msg.echoSender, msg.path, msg.sizeMsg(), true);
    
    consumeAllPending(msg.linkSenderId, msg.localId, conn);
}

void OptBrachaDolevHandler::handle_echoInit_2(const unsigned int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const int &localId, const PID &echoSender, const std::vector<PID> msgPath, const unsigned int &sizeMsg, bool doStats) {
    
    struct bId broadId = {broadcasterId, broadcastId};
    
    if (doStats and minBidMeasure <= broadcastId and broadcastId < maxBidMeasure) {
        if (numEchoInits.find(broadId) == numEchoInits.end()) {
            numEchoInits[broadId] = 1;
            bytesEchoInits[broadId] = sizeMsg;
        } else {
            numEchoInits[broadId]++;
            bytesEchoInits[broadId] += sizeMsg;
        }  
        if (broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    struct localInfo li = {broadcasterId, broadcastId, payloadSize, val}; 
    pidToLocalInfo[linkSenderId][localId] = li; // senderId -> localId -> localInfo
    
    // Do we have a local id for this content?
    ifNeededCreateLocalId(broadId, li);
    
    handle_echo_2(payloadSize, val, linkSenderId, broadcasterId,  broadcastId, echoSender, msgPath, 0, false);
    
}

void OptBrachaDolevHandler::handle_echoLocalId(EchoLocalId msg, const PeerNet::conn_t &conn) {
    
    if (pidToLocalInfo.find(msg.linkSenderId) == pidToLocalInfo.end() or pidToLocalInfo[msg.linkSenderId].find(msg.localId) == pidToLocalInfo[msg.linkSenderId].end()) {
        pendingEchoLocalId[msg.linkSenderId][msg.localId].push(msg);
        return;
    }
    
    localInfo li = pidToLocalInfo[msg.linkSenderId][msg.localId];

    
    struct bId broadId = {li.broadcasterId, li.broadcastId};
    if (minBidMeasure <= li.broadcastId and li.broadcastId < maxBidMeasure) {
        if (numEchoLocalIds.find(broadId) == numEchoLocalIds.end()) {
            numEchoLocalIds[broadId] = 1;
            bytesEchoLocalIds[broadId] = msg.sizeMsg();
        } else {
            numEchoLocalIds[broadId]++;
            bytesEchoLocalIds[broadId] += msg.sizeMsg();
        }  
        if (li.broadcastId == numBcasts-1) {
            printStats();
        }
    }  
    
    handle_echo_2(li.payloadSize, li.val, msg.linkSenderId, li.broadcasterId,  li.broadcastId, msg.echoSender, msg.path, 0, false);
}

void OptBrachaDolevHandler::handle_ready(Ready msg, const PeerNet::conn_t &conn) {
    
    checkPath(msg);
    
    handle_ready_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.readySender, msg.path, msg.sizeMsg(), true);
}

void OptBrachaDolevHandler::handle_readySingleHop(ReadySingleHop msg, const PeerNet::conn_t &conn) {
    struct bId broadId = {msg.linkSenderId, msg.broadcastId};

    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numReadys.find(broadId) == numReadys.end()) {
            numReadys[broadId] = 1;
            bytesReadys[broadId] = msg.sizeMsg();
        } else {
            numReadys[broadId]++;
            bytesReadys[broadId] += msg.sizeMsg();
        }  
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    std::vector<unsigned int> e;
    handle_ready_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.linkSenderId, e, 0, false);
}

void OptBrachaDolevHandler::handle_readyInitSingleHop(ReadyInitSingleHop msg, const PeerNet::conn_t &conn) {
    bId broadId = {msg.broadcasterId, msg.broadcastId};
    
    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numReadyInits.find(broadId) == numReadyInits.end()) {
            numReadyInits[broadId] = 1;
            bytesReadyInits[broadId] = msg.sizeMsg();
        } else {
            numReadyInits[broadId]++;
            bytesReadyInits[broadId] += msg.sizeMsg();
        }  
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    } 
    
    // Save the localId and associate it to the broadcast id
    // If not, create one that we will use for future communications about this broadcast 
    // complete code would need some more checks to verify that sender does not send multiple msgs
    
    localInfo li = {msg.broadcasterId, msg.broadcastId, payloadSize, msg.val}; 
    pidToLocalInfo[msg.linkSenderId][msg.localId] = li; // senderId -> localId -> localInfo
    
    ifNeededCreateLocalId(broadId, li);

    std::vector<unsigned int> e;
    handle_ready_2(payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.linkSenderId, e, 0, false);
    
    consumeAllPending(msg.linkSenderId, msg.localId, conn);
}

void OptBrachaDolevHandler::handle_readyLocalIdSingleHop(ReadyLocalIdSingleHop msg, const PeerNet::conn_t &conn){
    // if asynchronous, translation might not succeed
    // Store message, and process it later
    if (pidToLocalInfo.find(msg.linkSenderId) == pidToLocalInfo.end() or pidToLocalInfo[msg.linkSenderId].find(msg.localId) == pidToLocalInfo[msg.linkSenderId].end()) {
        pendingReadyLocalIdSingleHop[msg.linkSenderId][msg.localId].push(msg);
        return;
    }
    
    int localId = msg.localId; 
    localInfo li = pidToLocalInfo[msg.linkSenderId][localId];
    
    bId broadId = {li.broadcasterId, li.broadcastId};
    
    if (minBidMeasure <= li.broadcastId and li.broadcastId < maxBidMeasure) {
        if (numReadyLocalIds.find(broadId) == numReadyLocalIds.end()) {
            numReadyLocalIds[broadId] = 1;
            bytesReadyLocalIds[broadId] = msg.sizeMsg();
        } else {
            numReadyLocalIds[broadId]++;
            bytesReadyLocalIds[broadId] += msg.sizeMsg();
        }  
        if (li.broadcastId == numBcasts-1) {
            printStats();
        }
    } 

    std::vector<unsigned int> e;
    handle_ready_2(li.payloadSize, li.val, msg.linkSenderId, li.broadcasterId, li.broadcastId, msg.linkSenderId, e, 0, false);
}

void OptBrachaDolevHandler::handle_ready_2(const int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &readySender, const std::vector<PID> msgPath, const unsigned int &sizeMsg, const bool doStats) {
    
    if (DEBUG) { 
        std::cout<<linkSenderId<<" -> "<<myid<<": READY("<<readySender<<"), path [";
        for (int i = 0; i < msgPath.size(); i++) { 
            std::cout << msgPath[i]; 
            if (i != msgPath.size()-1) { std::cout << ", ";}
        }
        std::cout << "]" << std::endl;
    }
    
    bId broadId = {broadcasterId, broadcastId};

    if (doStats and minBidMeasure <= broadcastId and broadcastId < maxBidMeasure) {
        if (numReadys.find(broadId) == numReadys.end()) {
            numReadys[broadId] = 1;
            bytesReadys[broadId] = sizeMsg;
        } else {
            numReadys[broadId]++;
            bytesReadys[broadId] += sizeMsg;
        }  
        if (broadcastId == numBcasts-1) {
            printStats();
        }
    } 
    
    mId readyMsgId = {HDR_READY, broadcasterId, broadcastId, readySender};
    
    bool wasDelivered = (deliveredMsgs.find(readyMsgId) != deliveredMsgs.end()); 
    
    if (MOD_5_BONOMI and wasDelivered) {
        return;
    } // ignore if already delivered
    
    if (MOD_4_BONOMI) { // if empty path from q, store it, and will ignore paths that contain q in the future
        for (int i = 0; i < msgPath.size(); i++) { 
            if (neighborsThatDelivered[readyMsgId].find(msgPath[i]) != neighborsThatDelivered[readyMsgId].end()) {
                return;
            }
        }
        if (msgPath.size() == 0) {
            neighborsThatDelivered[readyMsgId].insert(linkSenderId);
            if (MBD_9) {
                readysWithEmptyPaths[broadId][linkSenderId].insert(readySender);
                if (readysWithEmptyPaths[broadId][linkSenderId].size() >= 2*numFaulty+1) {
                    neighborsToAvoid[broadId].insert(linkSenderId);
                }
            }
        }
    }
    
    // Assemble path: rcvd path + link sender Id
    std::vector<PID> path;
    std::string pathBitmask = std::string(numNodes, 0);
    for (int i = 0; i < msgPath.size(); i++) { 
        pathBitmask[msgPath[i]] = 1;
        path.push_back(msgPath[i]);
    }
    if (readySender != linkSenderId) {
        pathBitmask[linkSenderId] = 1; // bug fix, was missing (?)
        path.push_back(linkSenderId);
    }
    
    if (pathsBitmasks.find(readyMsgId) == pathsBitmasks.end()) {
        pathsBitmasks[readyMsgId] = new std::vector<std::string>[numNodes];
    } 
    
    if (MBD_10) { // check if received ready path is a superpath of known path
        for (int j = 0; j < selfNeighbors.size(); j++) {
            PID sender = selfNeighbors[j];
            for (int j = 0; j < pathsBitmasks[readyMsgId][sender].size(); j++) {
                std::string prevBitmask = pathsBitmasks[readyMsgId][sender][j];
                bool isSuperPath = true; 
                for (int k = 0; k < numNodes; k++) {
                    if (pathBitmask[k] and !prevBitmask[k]) {
                        isSuperPath = false;
                    }
                }
                if (!prevBitmask[readySender]) {
                    isSuperPath = false;
                }
                if (isSuperPath) {
                    return;
                }
            }
        }
    }
    
    // Insert path into data structure
    pathsBitmasks[readyMsgId][linkSenderId].push_back(pathBitmask);
    
    mId sourceMsgId = {HDR_ECHO, broadcasterId, broadcastId, broadcasterId};
        
    bool wasDeliveredSource = (deliveredMsgs.find(sourceMsgId) != deliveredMsgs.end()); 
    
    bool indirectDeliverSource = false;
    
    if (!wasDeliveredSource and myid != broadcasterId and readySender != broadcasterId and linkSenderId != broadcasterId) {
        
        bool sourceInPath = false;
        for (int i = 0; i < msgPath.size(); i++) {
            if (msgPath[i] == broadcasterId) {
                sourceInPath = true;
                break;
            }
        }
        
        if (!sourceInPath) {
            std::vector<PID> sourcePath; // note that broadcasterId not added to path
            sourcePath.push_back(readySender); // != broadcasterId
            for (int i = 0; i < msgPath.size(); i++) {
                sourcePath.push_back(msgPath[i]);
            }
            if (linkSenderId != readySender) {
                sourcePath.push_back(linkSenderId);
            }
            
            std::string sourcePathBitmask = std::string(numNodes, 0);
            for (int i = 0; i < sourcePath.size(); i++) { 
                sourcePathBitmask[sourcePath[i]] = 1;
            }
            
            if (pathsBitmasks.find(sourceMsgId) == pathsBitmasks.end()) {
                pathsBitmasks[sourceMsgId] = new std::vector<std::string>[numNodes];
            } 
            pathsBitmasks[sourceMsgId][linkSenderId].push_back(sourcePathBitmask);
            
            if (precomputeSets) { 
                std::string fullPathBitmask(sourcePathBitmask);
                fullPathBitmask[linkSenderId] = 1;
                indirectDeliverSource = recomputeAllNonIntersectingSets(sourceMsgId, fullPathBitmask);
            } else {
                indirectDeliverSource = msgCanBeDelivered(sourceMsgId, linkSenderId);
            }
            
            if (indirectDeliverSource) { 
                deliveredMsgs.insert(sourceMsgId);
                rcvEchos[broadId].insert(broadcasterId);
            }
        }
    }
    
    // Direct deliver? 
    bool directDeliver = MOD_1_BONOMI && linkSenderId == readySender;
    
    // Indirect Dolev-deliver? 
    bool indirectDeliver;
    if (precomputeSets) { 
        std::string fullPathBitmask(pathBitmask);
        fullPathBitmask[linkSenderId] = 1;
        indirectDeliver = (!wasDelivered and !directDeliver and recomputeAllNonIntersectingSets(readyMsgId, fullPathBitmask));
    } else {
        indirectDeliver = (!wasDelivered and !directDeliver and msgCanBeDelivered(readyMsgId, linkSenderId)); 
    }
    
    bool sentReady = false;
    bool sentEcho = false;
    
    if (!wasDelivered and (directDeliver or indirectDeliver)) {
        
        rcvReadys[broadId].insert(readySender);
        rcvEchos[broadId].insert(readySender);
        deliveredMsgs.insert(readyMsgId); // save delivered broadcast id
        struct mId echoMsgId = {HDR_ECHO, readyMsgId.broadcasterId, readyMsgId.broadcastId, readySender};
        deliveredMsgs.insert(echoMsgId);
        
        if (DEBUG) { 
            std::cout<<KRED<<"\t"<<myid<<" delivers READY("<<readySender<<") and has "<<rcvReadys[broadId].size()<<"/"<<byzQuorumSize<<" READYs [";
            std::set<unsigned int>::iterator it;
            int a = 0;
            for (it=rcvReadys[broadId].begin(); it!=rcvReadys[broadId].end(); ++it) {
                if (a!=0) std::cout << ",";
                std::cout << *it;
                a++;
            }
            std::cout <<"]" << KNRM<<std::endl;
        }

//         std::cout<<KCYN<<"\t"<<linkSenderId<<" -> "<<myid<<": READY("<<readySender<<"), path [";
//         for (int i = 0; i < msgPath.size(); i++) { 
//             std::cout << msgPath[i] << ", "; 
//         }
//         std::cout << "]" << std::endl;
//         
//         std::cout<<KCYN<<"\t"<<linkSenderId<<" -> "<<myid<<": READY("<<readySender<<")" << " has " << rcvReadys[broadId].size() << "/"<<(2*numFaulty+1)<<" readys" << KNRM << std::endl;
//         : [ " ;
//         for (auto it = rcvReadys[broadId].begin(); it != rcvReadys[broadId].end(); it++) {
//             std::cout << *it << ", ";
//         }
//         std::cout << "]" << KNRM << std::endl;    
//         
//         std::cout << KCYN << "\t\t" << myid << " has " << rcvEchos[broadId].size() << "/"<<byzQuorumSize<<" echos : [ " ;
//         for (auto it = rcvEchos[broadId].begin(); it != rcvEchos[broadId].end(); it++) {
//             std::cout << *it << ", ";
//         }
//         std::cout << "]" << KNRM << std::endl; 

        
        // Generate own Ready? 
        // skip directly to sending ready
        if (isSelectedToSendReadys(broadcasterId) and !contains(rcvReadys[broadId], myid) and 
            (rcvEchos[broadId].size() >= byzQuorumSize or rcvReadys[broadId].size() >= numFaulty+1)) {
//             if (MBD_4) {
//                 send_readyEcho(msgId, val, path);
//                 sentReadyEcho = true;// own ready + rcv echo
//             } else {
//                 std::cout << myid << " is entering send_ready" <<std::endl;
                sentReady = send_ready(readyMsgId, val);
//             }
        } // Or else try to send Echo?
        else if (isSelectedToSendEchos(broadcasterId) and !contains(rcvEchos[broadId], myid) and 
            (contains(rcvEchos[broadId], broadcasterId) or rcvEchos[broadId].size() >= numFaulty + 1)) {   
//             if (MBD_3) {
//                 send_echoEcho(msgId, val, path); 
//                 sentEchoEcho = true;
//             } else {
                sentEcho = send_echo(readyMsgId, val); // own echo
//             }
        }

        
    } else if (!wasDeliveredSource and indirectDeliverSource) {

        if (isSelectedToSendReadys(broadcasterId) and !contains(rcvReadys[broadId], myid) and 
            (rcvEchos[broadId].size() >= byzQuorumSize or rcvReadys[broadId].size() >= numFaulty+1)) {
                sentReady = send_ready(readyMsgId, val);
                std::cout << "Send Ready following reception of ready" << std::endl;
        } // Or else try to send Echo?
        else if (isSelectedToSendEchos(broadcasterId) and !contains(rcvEchos[broadId], myid) and 
            (contains(rcvEchos[broadId], broadcasterId) or rcvEchos[broadId].size() >= numFaulty + 1)) {
//                 std::cout << "Send echo following reception of ready" << std::endl;
                sentEcho = send_echo(readyMsgId, val); // own echo
        }
    }
    
    // Can we deliver?
    bool justDelivered = false;
    if (rcvReadys[broadId].size() >= 2*numFaulty+1 and deliveredBroad.find(broadId) == deliveredBroad.end()) {
        std::cout << KGRN << myid<<" DELIVERED ["<<broadcasterId<<","<<broadcastId<<"]" << KNRM << std::endl;
        
//         deliveredBroad.insert(broadId); 
        deliveredBroad[broadId] = std::make_pair(getCurrentTime(), true);
        printDeliverTime(broadcasterId, broadcastId);
        
        justDelivered = true;
    }
    
    int localId = 0;
    if (MBD_1) {
        localId = bidToLocalId[broadId];
    } 
    
    ReadyInit d1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, readySender, emptyPath);
    ReadyLocalId d2(this->myid, localId, readySender, emptyPath);
    Ready d3(payloadSize, val, this->myid, broadcasterId, broadcastId, readySender, emptyPath); 
    
    ReadyInit d4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, readySender, path);
    ReadyLocalId d5(this->myid, localId, readySender, path);
    Ready d6(payloadSize, val, this->myid, broadcasterId, broadcastId, readySender, path);
    
    // Relay Ready message with modified path to neighbors
    for (int i = 0; i < selfNeighbors.size(); i++) {
        PID curNeighbor = selfNeighbors[i];
        if (curNeighbor == readySender) {continue;}
        
        if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and  neighborsToAvoid[broadId].find(curNeighbor) != neighborsToAvoid[broadId].end()) {continue;}
        
        // Relay with empty path upon delivery 
        if (MOD_2_BONOMI and (directDeliver or indirectDeliver)) {
            
            if (MBD_1) {
                if (not pidsThatKnowLocalId[localId][curNeighbor]) {
                    this->pnet.send_msg(d1, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    pidsThatKnowLocalId[localId].set(curNeighbor);
                } else { // send Ready_localId
                    this->pnet.send_msg(d2, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                }
            } else {
                this->pnet.send_msg(d3, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
            }
    
        } else { // or relay with modified path
            
            if (!pathBitmask[curNeighbor] and curNeighbor!=linkSenderId) { // curNeighbor not in path of rcvd msg
                if (MBD_1) {
                    if (not pidsThatKnowLocalId[localId][curNeighbor]) {
                        this->pnet.send_msg(d4, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        pidsThatKnowLocalId[localId].set(curNeighbor);
                    } else { // send Echo_localId
                        this->pnet.send_msg(d5, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                } else {
                    this->pnet.send_msg(d6, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                }
            }
        }
    }
    
    if (justDelivered) { 
        clean_after_deliver(broadId);
    }
}

void OptBrachaDolevHandler::clean_after_deliver(bId broadId) {
    
    // need to insert back to avoid sending again ready or echos
    rcvEchos.erase(broadId); 
    rcvReadys.erase(broadId); 
//     rcvReadys[broadId].insert(myid); 
//     rcvEchos[broadId].insert(myid);
    
    readysWithEmptyPaths.erase(broadId);
    neighborsToAvoid.erase(broadId);
    
    // std::map<int, std::map<int, localInfo>> pidToLocalInfo; // PID -> localId -> {broadcaster, broadcastId, payloadSize, val}
    if (MBD_1) {
        int localId = bidToLocalId[broadId]; // std::map<int, std::bitset <100>> pidsThatKnowLocalId; 
        if (pidsThatKnowLocalId.find(localId) != pidsThatKnowLocalId.end()) { 
            pidsThatKnowLocalId.erase(localId);
        }        
        bidToLocalId.erase(broadId); //std::map<bId, int> bidToLocalId; // bid -> local Id 
        for (int i = 0; i < selfNeighbors.size(); i++) {
            PID curNeighbor = selfNeighbors[i];
            if (pidToLocalInfo.find(curNeighbor) != pidToLocalInfo.end()) {
                for (auto it=pidToLocalInfo[curNeighbor].cbegin(); it!=pidToLocalInfo[curNeighbor].cend(); ) {
                    if (it->second.broadcasterId == broadId.broadcasterId and it->second.broadcastId == broadId.broadcastId) {
                        pidToLocalInfo[curNeighbor].erase(it++);
                    } else {
                        ++it;
                    }
                }
            }
        }
        
        if (pendingEchoLocalId.find(localId) != pendingEchoLocalId.end()) { 
            pendingEchoLocalId[localId].clear();
            pendingEchoLocalId.erase(localId);
        }
        if (pendingEchoLocalIdSingleHop.find(localId) != pendingEchoLocalIdSingleHop.end()) { 
            pendingEchoLocalIdSingleHop[localId].clear();
            pendingEchoLocalIdSingleHop.erase(localId);
        }
        if (pendingReadyLocalId.find(localId) != pendingReadyLocalId.end()) { 
            pendingReadyLocalId[localId].clear();
            pendingReadyLocalId.erase(localId);
        }
        if (pendingReadyLocalIdSingleHop.find(localId) != pendingReadyLocalIdSingleHop.end()) { 
            pendingReadyLocalIdSingleHop[localId].clear();
            pendingReadyLocalIdSingleHop.erase(localId);
        }
        if (pendingEchoEchoLocalId.find(localId) != pendingEchoEchoLocalId.end()) { 
            pendingEchoEchoLocalId[localId].clear();
            pendingEchoEchoLocalId.erase(localId);
        }
        if (pendingReadyEchoLocalId.find(localId) != pendingReadyEchoLocalId.end()) { 
            pendingReadyEchoLocalId[localId].clear();
            pendingReadyEchoLocalId.erase(localId);
        }
    }

    //     std::map<struct mId, std::vector<std::string> *> pathsBitmasks; 
    // an array of vector of paths (one per possible node in the network) per broadcast id
    for (PID i = 0; i < numNodes; i++) {
        mId echoMsgId = {HDR_ECHO, broadId.broadcasterId, broadId.broadcastId, i};
        mId readyMsgId = {HDR_READY, broadId.broadcasterId, broadId.broadcastId, i};
        
        if (pathsBitmasks.find(echoMsgId) != pathsBitmasks.end()) {
            pathsBitmasks[echoMsgId]->clear(); 
            pathsBitmasks.erase(echoMsgId); // calls destructor
        }
        if (pathsBitmasks.find(readyMsgId) != pathsBitmasks.end()) {
            pathsBitmasks[readyMsgId]->clear(); 
            pathsBitmasks.erase(readyMsgId);
        }
        
        if (allNonIntersectingSets.find(echoMsgId) != allNonIntersectingSets.end()) { 
            allNonIntersectingSets[echoMsgId]->clear();
            allNonIntersectingSets.erase(echoMsgId);
        }
        if (allNonIntersectingSets.find(readyMsgId) != allNonIntersectingSets.end()) { 
            allNonIntersectingSets[readyMsgId]->clear();
            allNonIntersectingSets.erase(readyMsgId);
        }
        
        auto it = deliveredMsgs.find(echoMsgId);
        if (it != deliveredMsgs.end()) {
            deliveredMsgs.erase(it);
        }
        it = deliveredMsgs.find(readyMsgId);
        if (it != deliveredMsgs.end()) {
            deliveredMsgs.erase(it);
        }
        
        if (neighborsThatDelivered.find(echoMsgId) != neighborsThatDelivered.end()) {
            neighborsThatDelivered.erase(echoMsgId);
        }
        if (neighborsThatDelivered.find(readyMsgId) != neighborsThatDelivered.end()) {
            neighborsThatDelivered.erase(readyMsgId);
        }
    }
}

void OptBrachaDolevHandler::ifNeededCreateLocalId(bId broadId, localInfo li) {
    if (bidToLocalId.find(broadId) == bidToLocalId.end()) {
        int localId = nextLocalId++;
        pidToLocalInfo[myid][localId] = li;
        bidToLocalId[broadId] = localId;
    }
}


void OptBrachaDolevHandler::handle_readyInit(ReadyInit msg, const PeerNet::conn_t &conn) {
    
    bId broadId = {msg.broadcasterId, msg.broadcastId};
    
    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numReadyInits.find(broadId) == numReadyInits.end()) {
            numReadyInits[broadId] = 1;
            bytesReadyInits[broadId] = msg.sizeMsg();
        } else {
            numReadyInits[broadId]++;
            bytesReadyInits[broadId] += msg.sizeMsg();
        }  
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    } 
    
    // Save the localId and associate it to the broadcast id
    // If not, create one that we will use for future communications about this broadcast 
    // complete code would need some more checks to verify that sender does not send multiple msgs
    
    localInfo li = {msg.broadcasterId, msg.broadcastId, payloadSize, msg.val}; 
    pidToLocalInfo[msg.linkSenderId][msg.localId] = li; // senderId -> localId -> localInfo
    
    ifNeededCreateLocalId(broadId, li);

    handle_ready_2(payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.readySender, msg.path, 0, false);
    
    consumeAllPending(msg.linkSenderId, msg.localId, conn);
}

void OptBrachaDolevHandler::handle_readyLocalId(ReadyLocalId msg, const PeerNet::conn_t &conn) {
    
    // if asynchronous, translation might not succeed
    // Store message, and process it later
    if (pidToLocalInfo.find(msg.linkSenderId) == pidToLocalInfo.end() or pidToLocalInfo[msg.linkSenderId].find(msg.localId) == pidToLocalInfo[msg.linkSenderId].end()) {
        pendingReadyLocalId[msg.linkSenderId][msg.localId].push(msg);
        return;
    }
    
    int localId = msg.localId; 
    localInfo li = pidToLocalInfo[msg.linkSenderId][localId];
    
    bId broadId = {li.broadcasterId, li.broadcastId};
    
    if (minBidMeasure <= li.broadcastId and li.broadcastId < maxBidMeasure) {
        if (numReadyLocalIds.find(broadId) == numReadyLocalIds.end()) {
            numReadyLocalIds[broadId] = 1;
            bytesReadyLocalIds[broadId] = msg.sizeMsg();
        } else {
            numReadyLocalIds[broadId]++;
            bytesReadyLocalIds[broadId] += msg.sizeMsg();
        }  
        if (li.broadcastId == numBcasts-1) {
            printStats();
        }
    } 

    handle_ready_2(li.payloadSize, li.val, msg.linkSenderId, li.broadcasterId, li.broadcastId, msg.readySender, msg.path, 0, false);
}

void OptBrachaDolevHandler::handle_echoEcho(EchoEcho msg, const PeerNet::conn_t &conn) {
    handle_echoEcho_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.firstEchoSender, msg.secondEchoSender, msg.path, msg.sizeMsg(), true);
}

void OptBrachaDolevHandler::handle_echoEcho_2(const int &payloadSize, const char * val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &firstEchoSender, const PID &secondEchoSender, const std::vector<PID> msgPath, const unsigned int &sizeMsg, const bool doStats) {
    
    if (DEBUG) {
        std::cout<<linkSenderId<<" -> "<<myid<< ": ECHOECHO("<<broadcastId<<","<<firstEchoSender<<","<<secondEchoSender<<"), path [";
        for (int i = 0; i < msgPath.size(); i++) { 
            std::cout << msgPath[i] << ", "; 
        }
        std::cout << "]" << std::endl;
    }
    
    bId broadId = {broadcasterId, broadcastId};
    
    if (doStats and minBidMeasure <= broadcastId and broadcastId < maxBidMeasure) {
        if (numEchoEchos.find(broadId) == numEchoEchos.end()) {
            numEchoEchos[broadId] = 1;
            bytesEchoEchos[broadId] = sizeMsg;
        } else {
            numEchoEchos[broadId]++;
            bytesEchoEchos[broadId] += sizeMsg;
        }  
        if (broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
//     handle_echo_2(payloadSize, val, linkSenderId, broadcasterId,  broadcastId, secondEchoSender, msgPath, 0, false); // echo sender is linkSenderId
//     handle_echo_2(payloadSize, val, linkSenderId, broadcasterId,  broadcastId, firstEchoSender, msgPath, 0, false);
    
    mId echo1MsgId = {HDR_ECHO, broadcasterId, broadcastId, firstEchoSender};
    mId echo2MsgId = {HDR_ECHO, broadcasterId, broadcastId, secondEchoSender};
    
    bool processEcho1 = true;
    bool processEcho2 = true;
    
    bool echo1WasDelivered = (deliveredMsgs.find(echo1MsgId) != deliveredMsgs.end()); 
    bool echo2WasDelivered = (deliveredMsgs.find(echo2MsgId) != deliveredMsgs.end());
    
    // Compute if Echos should be Ignored (or processed)
    if (MBD_6) {
        struct mId ready1MsgId = {HDR_READY, broadcasterId, broadcastId, firstEchoSender};
        if (deliveredMsgs.find(ready1MsgId) != deliveredMsgs.end()) {processEcho1 = false;}
        
        struct mId ready2MsgId = {HDR_READY, broadcasterId, broadcastId, secondEchoSender};
        if (deliveredMsgs.find(ready2MsgId) != deliveredMsgs.end()) {processEcho2 = false;}
    }
    
    if (MBD_7 and deliveredBroad.find(broadId) != deliveredBroad.end()) {return;}
    
    if (MOD_5_BONOMI and echo1WasDelivered) {processEcho1 = false;} // ignore if already delivered
    
    if (MOD_5_BONOMI and echo2WasDelivered) {processEcho2 = false;} // ignore if already delivered
    
    if (MOD_4_BONOMI) { 
        if (neighborsThatDelivered.find(echo1MsgId) != neighborsThatDelivered.end()) {
            for (int i = 0; i < msgPath.size(); i++) { 
                if (neighborsThatDelivered[echo1MsgId].find(msgPath[i]) != neighborsThatDelivered[echo1MsgId].end()) {
                    processEcho1 = false;
                    break;
                }
            }
        }
        if (msgPath.size() == 0) { // if empty path from q, store it, and will ignore paths that contain q in the future
            neighborsThatDelivered[echo1MsgId].insert(linkSenderId);
        }
    }
    
    if (MOD_4_BONOMI) { 
        if (neighborsThatDelivered.find(echo2MsgId) != neighborsThatDelivered.end()) {
            for (int i = 0; i < msgPath.size(); i++) { 
                if (neighborsThatDelivered[echo2MsgId].find(msgPath[i]) != neighborsThatDelivered[echo2MsgId].end()) {
                    processEcho2 = false;
                    break;
                }
            }
        }
        if (msgPath.size() == 0) { // if empty path from q, store it, and will ignore paths that contain q in the future
            neighborsThatDelivered[echo2MsgId].insert(linkSenderId);
        }
    }
    
    // Assemble the Echo paths: rcvd path + link sender Id
    std::vector<PID> echo1Path;
    std::vector<PID> echo2Path;
    
    std::string echo1PathBitmask = std::string(numNodes, 0);
    std::string echo2PathBitmask = std::string(numNodes, 0);
    
    for (int i = 0; i < msgPath.size(); i++) { 
        echo1Path.push_back(msgPath[i]);
        echo2Path.push_back(msgPath[i]);
        echo1PathBitmask[msgPath[i]] = 1;
        echo2PathBitmask[msgPath[i]] = 1;
    }
    if (secondEchoSender != linkSenderId) {
        echo2Path.push_back(linkSenderId);
        echo2PathBitmask[linkSenderId] = 1;
    }
    if (firstEchoSender != linkSenderId) {
        echo1Path.push_back(linkSenderId);
        echo1PathBitmask[linkSenderId] = 1;
    }
    
        // Init data structure if needed 
    if (pathsBitmasks.find(echo1MsgId) == pathsBitmasks.end()) {
        pathsBitmasks[echo1MsgId] = new std::vector<std::string>[numNodes];
    } 
    // same for ready
    if (pathsBitmasks.find(echo2MsgId) == pathsBitmasks.end()) {
        pathsBitmasks[echo2MsgId] = new std::vector<std::string>[numNodes];
    } 
    
        // check if received Echo path is a superpath of known path
    if (processEcho1 and MBD_10) { 
        for (PID sender = 0; sender < numNodes; sender++) {
            for (int j = 0; j < pathsBitmasks[echo1MsgId][sender].size(); j++) {
                std::string prevBitmask = pathsBitmasks[echo1MsgId][sender][j];
                bool isSuperPath = true; 
                for (int k = 0; k < numNodes; k++) {
                    if (echo1PathBitmask[k] and !prevBitmask[k]) {
                        isSuperPath = false;
                    }
                }
                if (!prevBitmask[firstEchoSender]) {
                    isSuperPath = false;
                }
                if (isSuperPath) {
                    processEcho1 = false;
                    break;
                }
            }
        }
    }
    
    if (processEcho2 and MBD_10) { 
        for (PID sender = 0; sender < numNodes; sender++) {
            for (int j = 0; j < pathsBitmasks[echo2MsgId][sender].size(); j++) {
                std::string prevBitmask = pathsBitmasks[echo2MsgId][sender][j];
                bool isSuperPath = true; 
                for (int k = 0; k < numNodes; k++) {
                    if (echo2PathBitmask[k] and !prevBitmask[k]) {
                        isSuperPath = false;
                    }
                }
                if (!prevBitmask[secondEchoSender]) {
                    isSuperPath = false;
                }
                if (isSuperPath) {
                    processEcho2 = false;
                    break;
                }
            }
        }
    }
    
    // Insert Echo path into data structure
    if (processEcho1) { 
        pathsBitmasks[echo1MsgId][linkSenderId].push_back(echo1PathBitmask);
    }
    if (processEcho2) { 
        pathsBitmasks[echo2MsgId][linkSenderId].push_back(echo2PathBitmask);
    }
    
    // Study source path possibly embedded in Echo 1
    mId sourceMsgId = {HDR_ECHO, broadcasterId, broadcastId, broadcasterId};
    if (MBD_2 and myid != broadcasterId and firstEchoSender != broadcasterId and 
        (deliveredMsgs.find(sourceMsgId) == deliveredMsgs.end())
    ) {
        std::vector<PID> sourcePath; // note that broadcasterId not added to path
        sourcePath.push_back(firstEchoSender); // != broadcasterId
        sourcePath.push_back(secondEchoSender);
        for (int i = 0; i < msgPath.size(); i++) {
            sourcePath.push_back(msgPath[i]);
        }
        sourcePath.push_back(linkSenderId); 
        
        std::string sourcePathBitmask = std::string(numNodes, 0);
        for (int i = 0; i < sourcePath.size(); i++) { 
            sourcePathBitmask[sourcePath[i]] = 1;
        }
        
        if (pathsBitmasks.find(sourceMsgId) == pathsBitmasks.end()) {
            pathsBitmasks[sourceMsgId] = new std::vector<std::string>[numNodes];
        } 
        pathsBitmasks[sourceMsgId][linkSenderId].push_back(sourcePathBitmask);
        
        bool indirectDeliver;
        if (precomputeSets) { 
            std::string fullPathBitmask(sourcePathBitmask);
            fullPathBitmask[linkSenderId] = 1;
            indirectDeliver = recomputeAllNonIntersectingSets(sourceMsgId, fullPathBitmask);
        } else {
            indirectDeliver = msgCanBeDelivered(sourceMsgId, linkSenderId);
        }
        
        if (indirectDeliver) {
            deliveredMsgs.insert(sourceMsgId);
            rcvEchos[broadId].insert(broadcasterId);
            // THIS HAPPENS, but not leveraged
//             std::cout << "INDIRECT DELIVER SOURCE in ECHO_ECHO (firstEchoSender)" << std::endl;
        }
    }
    
     // Study source path possibly embedded in Echo 2
    if (MBD_2 and myid != broadcasterId and secondEchoSender != broadcasterId and 
        (deliveredMsgs.find(sourceMsgId) == deliveredMsgs.end())
    ) {
        std::vector<PID> sourcePath; // note that broadcasterId not added to path
        sourcePath.push_back(secondEchoSender);
        for (int i = 0; i < msgPath.size(); i++) {
            sourcePath.push_back(msgPath[i]);
        }
        sourcePath.push_back(linkSenderId); 
        
        std::string sourcePathBitmask = std::string(numNodes, 0);
        for (int i = 0; i < sourcePath.size(); i++) { 
            sourcePathBitmask[sourcePath[i]] = 1;
        }
        
        if (pathsBitmasks.find(sourceMsgId) == pathsBitmasks.end()) {
            pathsBitmasks[sourceMsgId] = new std::vector<std::string>[numNodes];
        } 
        pathsBitmasks[sourceMsgId][linkSenderId].push_back(sourcePathBitmask);
        
        bool indirectDeliver;
        if (precomputeSets) { 
            std::string fullPathBitmask(sourcePathBitmask);
            fullPathBitmask[linkSenderId] = 1;
            indirectDeliver = recomputeAllNonIntersectingSets(sourceMsgId, sourcePathBitmask);
        } else {
            indirectDeliver = msgCanBeDelivered(sourceMsgId, linkSenderId);
        }
        
        if (indirectDeliver) {
            deliveredMsgs.insert(sourceMsgId);
            rcvEchos[broadId].insert(broadcasterId);
//             std::cout << "INDIRECT DELIVER SOURCE in ECHO_ECHO(secondEchoSender)" << std::endl;
        }
    }
    
    // Compute whether Echo 1 can be delivered
    bool echo1DirectDeliver = MOD_1_BONOMI and linkSenderId == firstEchoSender;
    
    bool echo1IndirectDeliver;
    if (precomputeSets) { 
        std::string fullPathBitmask1(echo1PathBitmask);
        fullPathBitmask1[linkSenderId] = 1;
        echo1IndirectDeliver = (!echo1WasDelivered and !echo1DirectDeliver and recomputeAllNonIntersectingSets(echo1MsgId, fullPathBitmask1));
    } else {
        echo1IndirectDeliver = (!echo1WasDelivered and !echo1DirectDeliver and msgCanBeDelivered(echo1MsgId, linkSenderId));
    }
    
    bool echo1JustDelivered = !echo1WasDelivered and (echo1DirectDeliver or echo1IndirectDeliver);
    
    // Compute whether Echo 2 can be delivered
    bool echo2DirectDeliver = MOD_1_BONOMI and linkSenderId == secondEchoSender;
    
    bool echo2IndirectDeliver;
    if (precomputeSets) { 
        std::string fullPathBitmask2(echo2PathBitmask);
        fullPathBitmask2[linkSenderId] = 1;
        echo2IndirectDeliver = (!echo2WasDelivered and !echo2DirectDeliver and recomputeAllNonIntersectingSets(echo2MsgId, fullPathBitmask2));
    } else {
        echo2IndirectDeliver = (!echo2WasDelivered and !echo2DirectDeliver and msgCanBeDelivered(echo2MsgId, linkSenderId));
    }
    
    bool echo2JustDelivered = !echo2WasDelivered and (echo2DirectDeliver or echo2IndirectDeliver);
    
    // TODO: Send own Echo or Ready if possible
    // TODO: try to encapsulate with another one if possible? (several cases to handle...)
    // TODO: try to change order
    if (echo1JustDelivered or echo2JustDelivered) {
        
        if (echo1JustDelivered) { 
            deliveredMsgs.insert(echo1MsgId); 
            rcvEchos[broadId].insert(firstEchoSender);
//             std::cout << KCYN << "\t" << myid << " delivers ECHO " << firstEchoSender << KNRM << std::endl;
        }
        if (echo2JustDelivered) { 
            deliveredMsgs.insert(echo2MsgId); 
            rcvEchos[broadId].insert(secondEchoSender);
//             std::cout << KCYN << "\t" << myid << " delivers ECHO " << secondEchoSender << KNRM << std::endl;
        }
        
//         std::cout<<KCYN<<"\t"<<linkSenderId<<" -> "<<myid<< " ECHOECHO("<<firstEchoSender<<","<<secondEchoSender<<"), path [";
//         for (int i = 0; i < msgPath.size(); i++) { 
//             std::cout << msgPath[i] << ", "; 
//         }
//         std::cout << "]" << std::endl;
//         std::cout << KCYN << "\t\t" << myid << " has " << rcvEchos[broadId].size() << "/"<<byzQuorumSize<<" echos: [ " ;
//         for (auto it = rcvEchos[broadId].begin(); it != rcvEchos[broadId].end(); it++) {
//             std::cout << *it << ", ";
//         }
//         std::cout << "]" << KNRM << std::endl;

        // Try to send own Echo or ready // skip directly to ready and avoid sending Echo
        if (isSelectedToSendReadys(broadcasterId) and !contains(rcvReadys[broadId], myid) and 
            (rcvEchos[broadId].size() >= byzQuorumSize or rcvReadys[broadId].size() >= numFaulty+1)) {
                send_ready(echo1MsgId, val); // TODO change interface to because it only uses broadcasterId and broadcastId
        } else if (isSelectedToSendEchos(broadcasterId) and !contains(rcvEchos[broadId], myid) and 
            (contains(rcvEchos[broadId], broadcasterId) or rcvEchos[broadId].size() >= numFaulty + 1)) {   
                send_echo(echo1MsgId, val); // own echo
        }
    }
    
    // Relay Echo and Ready messages: only one or the other, or both 
    int localId = 0;
    if (MBD_1) {
        localId = bidToLocalId[broadId];
    }
        
    if (processEcho1 and !processEcho2) { // forward only first echo
        if (DEBUG) {
            std::cout << "\t"<< myid << " ECHOECHO: forward only first echo" << std::endl;
        }
        
        EchoInit e1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, emptyPath);
        EchoLocalId e2(this->myid, localId, firstEchoSender, emptyPath);
        Echo e3(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, emptyPath);
        
        EchoInit e4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, echo1Path);
        EchoLocalId e5(this->myid, localId, firstEchoSender, echo1Path);
        Echo e6(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, echo1Path);
        
        // Relay Echo message with modified path to neighbors
//         if (MBD_11 or !MBD_2 or linkSenderId != broadcasterId) { // avoid relaying send msgs
        for (int i = 0; i < selfNeighbors.size(); i++) {
            PID curNeighbor = selfNeighbors[i];
            
            if (curNeighbor == firstEchoSender) {continue;}
            
            if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(curNeighbor) != neighborsToAvoid[broadId].end()) {continue;}
            
            if (MBD_8 and 
                rcvReadys.find(broadId)!=rcvReadys.end() and
                contains(rcvReadys[broadId], curNeighbor)) {continue;}
            
            // Send empty path upon delivery
            if (MOD_2_BONOMI and echo1JustDelivered) {
                
                if (MBD_1) {
                    if (not pidsThatKnowLocalId[localId][curNeighbor]) {
                        this->pnet.send_msg(e1, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        pidsThatKnowLocalId[localId].set(curNeighbor);
                    } else { // send Echo_localId
                        this->pnet.send_msg(e2, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                } else {
                    this->pnet.send_msg(e3, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                }
        
            } else { // else relay with modified path
                if (!echo1PathBitmask[curNeighbor] and curNeighbor!=linkSenderId) {
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][curNeighbor]) {
                            this->pnet.send_msg(e4, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                            pidsThatKnowLocalId[localId].set(curNeighbor);
                        } else { // send Echo_localId
                            this->pnet.send_msg(e5, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        }
                    } else {
                        this->pnet.send_msg(e6, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                }
            }
        }
//         }
    } else if (!processEcho1 and processEcho2) { // forward only second echo
        if (DEBUG) {
            std::cout << "\t"<< myid << " ECHOECHO: forward only second echo" << std::endl;
        }
        
        if (MBD_1) {
            localId = bidToLocalId[broadId];
        } 
        
        EchoInit e1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondEchoSender, emptyPath);
        EchoLocalId e2(this->myid, localId, secondEchoSender, emptyPath);
        Echo e3(payloadSize, val, this->myid, broadcasterId, broadcastId, secondEchoSender, emptyPath);
        
        EchoInit e4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondEchoSender, echo2Path);
        EchoLocalId e5(this->myid, localId, secondEchoSender, echo2Path);
        Echo e6(payloadSize, val, this->myid, broadcasterId, broadcastId, secondEchoSender, echo2Path);
        

        
        // Relay Echo message with modified path to neighbors
//         if (MBD_11 or !MBD_2 or linkSenderId != broadcasterId) { // avoid relaying send msgs
        for (int i = 0; i < selfNeighbors.size(); i++) {
            PID curNeighbor = selfNeighbors[i];
            
            if (curNeighbor == secondEchoSender) {continue;}
            
            if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(curNeighbor) != neighborsToAvoid[broadId].end()) {continue;}
            
            if (MBD_8 and 
                rcvReadys.find(broadId)!=rcvReadys.end() and
                contains(rcvReadys[broadId], curNeighbor)) {continue;}
            
            // Send empty path upon delivery
            if (MOD_2_BONOMI and echo2JustDelivered) {
                
                if (MBD_1) {
                    if (not pidsThatKnowLocalId[localId][curNeighbor]) {
                        this->pnet.send_msg(e1, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        pidsThatKnowLocalId[localId].set(curNeighbor);
                    } else { // send Echo_localId
                        this->pnet.send_msg(e2, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                } else {
                    this->pnet.send_msg(e3, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                }
        
            } else { // else relay with modified path
                if (!echo2PathBitmask[curNeighbor] and curNeighbor!=linkSenderId) {
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][curNeighbor]) {
                            this->pnet.send_msg(e4, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                            pidsThatKnowLocalId[localId].set(curNeighbor);
                        } else { // send Echo_localId
                            this->pnet.send_msg(e5, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        }
                    } else {
                        this->pnet.send_msg(e6, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                }
            }
        }
//         }
        
    } else if (processEcho1 and processEcho2) { // both to forward
        
        if (DEBUG) {
            std::cout << "\t"<< myid << " ECHOECHO: forward both echos" << std::endl;
        }
        
        if (MBD_1) {
            localId = bidToLocalId[broadId];
        } 
        
        EchoInit e11(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, emptyPath);
        EchoLocalId e12(this->myid, localId, firstEchoSender, emptyPath);
        Echo e13(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, emptyPath);
        
        EchoInit e14(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, echo1Path);
        EchoLocalId e15(this->myid, localId, firstEchoSender, echo1Path);
        Echo e16(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, echo1Path);
        
        EchoInit e21(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondEchoSender, emptyPath);
        EchoLocalId e22(this->myid, localId, secondEchoSender, emptyPath);
        Echo e23(payloadSize, val, this->myid, broadcasterId, broadcastId, secondEchoSender, emptyPath);
        
        EchoInit e24(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondEchoSender, echo2Path);
        EchoLocalId e25(this->myid, localId, secondEchoSender, echo2Path);
        Echo e26(payloadSize, val, this->myid, broadcasterId, broadcastId, secondEchoSender, echo2Path);
        
        // If both echos delivered -> empty path
        EchoEchoInit ee1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, secondEchoSender, emptyPath);
        EchoEchoLocalId ee2(this->myid, localId, firstEchoSender, secondEchoSender, emptyPath);
        EchoEcho ee3(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, secondEchoSender, emptyPath);
        
        // otherwise if both are not delivered echo1Path = echo2Path
        EchoEchoInit ee4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, secondEchoSender, echo1Path);
        EchoEchoLocalId ee5(this->myid, localId, firstEchoSender, secondEchoSender, echo1Path);
        EchoEcho ee6(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, secondEchoSender, echo1Path);
        
        // We can relay both the Echo and the Ready, but some neighbors need only one
        for (int j = 0; j < selfNeighbors.size(); j++) { 
            
            PID cur = selfNeighbors[j];
            
            if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(cur) != neighborsToAvoid[broadId].end()) {continue;}
            
            if (MBD_8 and 
                rcvReadys.find(broadId)!=rcvReadys.end() and
                contains(rcvReadys[broadId], cur)) {continue;}

            if (cur == firstEchoSender) { // only 2nd echo for that neighbour
                
                // Relay with empty path upon delivery 
                if (MOD_2_BONOMI and echo2JustDelivered) {
                    
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
                            this->pnet.send_msg(e21, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Ready_localId
                            this->pnet.send_msg(e22, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
                        this->pnet.send_msg(e23, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
            
                } else { // or relay with modified path
                    
                    if (!echo2PathBitmask[cur] and cur!=linkSenderId) { // curNeighbor not in path of rcvd msg
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
                                this->pnet.send_msg(e24, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
                                this->pnet.send_msg(e25, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
                            this->pnet.send_msg(e26, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
                }
                
            } else if (cur == secondEchoSender) { // only 1st echo for that neighbour
                
                if (MBD_8 and 
                rcvReadys.find(broadId)!=rcvReadys.end() and
                contains(rcvReadys[broadId], cur)) {continue;}
                
                // Send empty path upon delivery
                if (MOD_2_BONOMI and echo1JustDelivered) {
                    
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
                            this->pnet.send_msg(e11, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Echo_localId
                            this->pnet.send_msg(e12, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
                        this->pnet.send_msg(e13, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
            
                } else { // else relay with modified path
                    if (!echo1PathBitmask[cur] and cur!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
                                this->pnet.send_msg(e14, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
                                this->pnet.send_msg(e15, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
                            this->pnet.send_msg(e16, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
                }
                
            } else { // both the Ready and Echo for that neighbour

                // the ready and echo might have different paths!
                if (MOD_2_BONOMI and echo1JustDelivered and echo2JustDelivered) { 
                    // echoEcho with empty paths
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
                            this->pnet.send_msg(ee1, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Echo_localId
                            this->pnet.send_msg(ee2, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
                        this->pnet.send_msg(ee3, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
                } else if (MOD_2_BONOMI and !echo1JustDelivered and !echo2JustDelivered) {
                    // echoEcho with modified path
                    if (!echo2PathBitmask[cur] and cur!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
                                this->pnet.send_msg(ee4, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
                                this->pnet.send_msg(ee5, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
                            this->pnet.send_msg(ee6, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
                    // TODO: some neighbors don't need both ?
                } else if (MOD_2_BONOMI and echo1JustDelivered and !echo2JustDelivered) { 
                    // echo1 with empty path, echo2 with modified path
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
                            this->pnet.send_msg(e11, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Echo_localId
                            this->pnet.send_msg(e12, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
                        this->pnet.send_msg(e13, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
                    if (!echo2PathBitmask[cur] and cur!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
                                this->pnet.send_msg(e24, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
                                this->pnet.send_msg(e25, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
                            this->pnet.send_msg(e26, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
                } else {
                    // echo1 with modified path, echo2 with empty path
                    if (!echo1PathBitmask[cur] and cur!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
                                this->pnet.send_msg(e14, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
                                this->pnet.send_msg(e15, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
                            this->pnet.send_msg(e16, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
            
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
                            this->pnet.send_msg(e21, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Ready_localId
                            this->pnet.send_msg(e22, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
                        this->pnet.send_msg(e23, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
        
                } 
            }
        
        }
        
        
    }
    
}

void OptBrachaDolevHandler::handle_echoEchoInit(EchoEchoInit msg, const PeerNet::conn_t &conn) {
    
    bId broadId = {msg.broadcasterId, msg.broadcastId}; 
    
    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numEchoEchoInits.find(broadId) == numEchoEchoInits.end()) {
            numEchoEchoInits[broadId] = 1;
            bytesEchoEchoInits[broadId] = msg.sizeMsg();
        } else {
            numEchoEchoInits[broadId]++;
            bytesEchoEchoInits[broadId] += msg.sizeMsg();
        }  
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    // Save the localId and associate it to the broadcast id
    // If not, create one that we will use for future communications about this broadcast 
    // complete code would need some more checks to verify that sender does not send multiple msgs
    
    localInfo li = {msg.broadcasterId, msg.broadcastId, payloadSize, msg.val}; 
    pidToLocalInfo[msg.linkSenderId][msg.localId] = li; // senderId -> localId -> localInfo
    
    // Do we have a local id for this content?
    ifNeededCreateLocalId(broadId, li);
    
    handle_echoEcho_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.firstEchoSender, msg.secondEchoSender, msg.path, 0, false);
    
    consumeAllPending(msg.linkSenderId, msg.localId, conn);
}

void OptBrachaDolevHandler::handle_echoEchoLocalId(EchoEchoLocalId msg, const PeerNet::conn_t &conn) {
//     std::cout << KRED << "\t" << nfo() << " <- " << msg.linkSenderId << " : receive echoEchoLocalId" << std::endl;
    // Receive a message from a neighbour that contains a localId that will be used in the future
    
    // if asynchronous, translation might not succeed
    // Store message, and process it later
    if (pidToLocalInfo.find(msg.linkSenderId) == pidToLocalInfo.end() or pidToLocalInfo[msg.linkSenderId].find(msg.localId) == pidToLocalInfo[msg.linkSenderId].end()) {
        pendingEchoEchoLocalId[msg.linkSenderId][msg.localId].push(msg);
//         std::cout << "ALERT (echoEchoLocalId): UNKNOWN LOCAL ID RECEIVED!" << std::endl;
        return;
    }
    
    int localId = msg.localId; 
    localInfo li = pidToLocalInfo[msg.linkSenderId][localId];
    
    bId broadId = {li.broadcasterId, li.broadcastId};
    
    if (minBidMeasure <= li.broadcastId and li.broadcastId < maxBidMeasure) {
        if (numEchoEchoLocalIds.find(broadId) == numEchoEchoLocalIds.end()) {
            numEchoEchoLocalIds[broadId] = 1;
            bytesEchoEchoLocalIds[broadId] = msg.sizeMsg();
        } else {
            numEchoEchoLocalIds[broadId]++;
            bytesEchoEchoLocalIds[broadId] += msg.sizeMsg();
        }  
        if (li.broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    handle_echoEcho_2(li.payloadSize, li.val, msg.linkSenderId, li.broadcasterId, li.broadcastId, msg.firstEchoSender, msg.secondEchoSender, msg.path, 0, false);
}


void OptBrachaDolevHandler::handle_readyEcho(ReadyEcho msg, const PeerNet::conn_t &conn) {
    
    handle_readyEcho_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.firstEchoSender, msg.secondReadySender, msg.path, msg.sizeMsg(), true);
    
}

void OptBrachaDolevHandler::handle_readyEcho_2(const int &payloadSize, const char * val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &firstEchoSender, const PID &secondReadySender, const std::vector<PID> msgPath, const unsigned int &sizeMsg, bool doStats) {
    
    if (DEBUG) {
        std::cout<<linkSenderId<<" -> "<<myid<< ": READYECHO("<<secondReadySender<<","<<firstEchoSender<<"), path [";
        for (int i = 0; i < msgPath.size(); i++) { 
            std::cout << msgPath[i]; 
            if (i != msgPath.size()-1) { std::cout << ", ";}
        }
        std::cout << "]" << std::endl;
    }
    
    bId broadId = {broadcasterId, broadcastId}; 
    
    if (doStats and minBidMeasure <= broadcastId and broadcastId < maxBidMeasure) {
        if (numReadyEchos.find(broadId) == numReadyEchos.end()) {
            numReadyEchos[broadId] = 1;
            bytesReadyEchos[broadId] = sizeMsg;
        } else {
            numReadyEchos[broadId]++;
            bytesReadyEchos[broadId] += sizeMsg;
        }  
        if (broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
//     handle_ready_2(payloadSize, val, linkSenderId, broadcasterId,  broadcastId, secondReadySender, msgPath, 0, false);
//     handle_echo_2(payloadSize, val, linkSenderId, broadcasterId,  broadcastId, firstEchoSender, msgPath, 0, false);
    
    // All code below (better) replaces the two lines above
    mId echoMsgId = {HDR_ECHO, broadcasterId, broadcastId, firstEchoSender};
    mId readyMsgId = {HDR_READY, broadcasterId, broadcastId, secondReadySender};
    
    bool processEcho = true;
    bool processReady = true;
    
    bool echoWasDelivered = (deliveredMsgs.find(echoMsgId) != deliveredMsgs.end()); 
    bool readyWasDelivered = (deliveredMsgs.find(readyMsgId) != deliveredMsgs.end()); 
    
    // Compute if Echo and Ready should be Ignored (or processed)
    if (MBD_6) { // MBD_6: Ignore Echo if Ready has been delivered
        struct mId readyMsgId = {HDR_READY, broadcasterId, broadcastId, firstEchoSender};
        if (deliveredMsgs.find(readyMsgId) != deliveredMsgs.end()) {processEcho = false;}
    }
    
    if (MBD_7 and deliveredBroad.find(broadId) != deliveredBroad.end()) {processEcho = false;} // MBD_7: Ignore Echos after delivery
    
    if (MOD_5_BONOMI and echoWasDelivered) {processEcho = false;} // ignore if already delivered
    
    if (MOD_5_BONOMI and readyWasDelivered) {processReady = false;}
    
    if (MOD_4_BONOMI) { // if empty path from q, store it, and will ignore paths that contain q in the future
        if (neighborsThatDelivered.find(echoMsgId) != neighborsThatDelivered.end()) {
            for (int i = 0; i < msgPath.size(); i++) { 
                if (neighborsThatDelivered[echoMsgId].find(msgPath[i]) != neighborsThatDelivered[echoMsgId].end()) {
                    processEcho = false;
                    break;
                }
            }
        }
        if (msgPath.size() == 0) {
            neighborsThatDelivered[echoMsgId].insert(linkSenderId);
        }
    }
    
    if (MOD_4_BONOMI) { // if empty path from q, store it, and will ignore paths that contain q in the future
        for (int i = 0; i < msgPath.size(); i++) { 
            if (neighborsThatDelivered[readyMsgId].find(msgPath[i]) != neighborsThatDelivered[readyMsgId].end()) {
                processReady = false;
                break;
            }
        }
        if (msgPath.size() == 0) {
            neighborsThatDelivered[readyMsgId].insert(linkSenderId);
            if (MBD_9) {
                readysWithEmptyPaths[broadId][linkSenderId].insert(secondReadySender);
                if (readysWithEmptyPaths[broadId][linkSenderId].size() >= 2*numFaulty+1) {
                    neighborsToAvoid[broadId].insert(linkSenderId);
                }
            }
        }
    }

    // Assemble Echo and Ready paths: rcvd path + link sender Id
    std::vector<PID> echoPath;
    std::vector<PID> readyPath;
    
    std::string echoPathBitmask = std::string(numNodes, 0);
    std::string readyPathBitmask = std::string(numNodes, 0);
    
    for (int i = 0; i < msgPath.size(); i++) { 
        echoPath.push_back(msgPath[i]);
        readyPath.push_back(msgPath[i]);
        echoPathBitmask[msgPath[i]] = 1;
        readyPathBitmask[msgPath[i]] = 1;
    }
    if (secondReadySender != linkSenderId) {
        readyPath.push_back(linkSenderId);
        readyPathBitmask[linkSenderId] = 1;
    }
    if (firstEchoSender != linkSenderId) {
        echoPath.push_back(linkSenderId);
        echoPathBitmask[linkSenderId] = 1;
    }
    
    // Init data structure if needed 
    if (pathsBitmasks.find(echoMsgId) == pathsBitmasks.end()) {
        pathsBitmasks[echoMsgId] = new std::vector<std::string>[numNodes];
    } 
    // same for ready
    if (pathsBitmasks.find(readyMsgId) == pathsBitmasks.end()) {
        pathsBitmasks[readyMsgId] = new std::vector<std::string>[numNodes];
    } 
    
    // check if received Echo path is a superpath of known path
    if (processEcho and MBD_10) { 
        for (PID sender = 0; sender < numNodes; sender++) {
            for (int j = 0; j < pathsBitmasks[echoMsgId][sender].size(); j++) {
                std::string prevBitmask = pathsBitmasks[echoMsgId][sender][j];
                bool isSuperPath = true; 
                for (int k = 0; k < numNodes; k++) {
                    if (echoPathBitmask[k] and !prevBitmask[k]) {
                        isSuperPath = false;
                    }
                }
                if (!prevBitmask[firstEchoSender]) {
                    isSuperPath = false;
                }
                if (isSuperPath) {
                    processEcho = false;
                    break;
                }
            }
        }
    }
    
    // check if received Ready path is a superpath of known path
    if (processReady and MBD_10) { // check if received ready path is a superpath of known path
        for (PID sender = 0; sender < numNodes; sender++) {
            for (int j = 0; j < pathsBitmasks[readyMsgId][sender].size(); j++) {
                std::string prevBitmask = pathsBitmasks[readyMsgId][sender][j];
                bool isSuperPath = true; 
                for (int k = 0; k < numNodes; k++) {
                    if (readyPathBitmask[k] and !prevBitmask[k]) {
                        isSuperPath = false;
                    }
                }
                if (!prevBitmask[secondReadySender]) {
                    isSuperPath = false;
                }
                if (isSuperPath) {
                    processReady = false;
                    break;
                }
            }
        }
    }
    // Insert Echo path into data structure
    if (processEcho) { 
        pathsBitmasks[echoMsgId][linkSenderId].push_back(echoPathBitmask);
    }
    
    // Insert Ready path into data structure
    if (processReady) {
        pathsBitmasks[readyMsgId][linkSenderId].push_back(readyPathBitmask);
    }
    
    // Study source path possibly embedded in Echo 
    mId sourceMsgId = {HDR_ECHO, broadcasterId, broadcastId, broadcasterId};
    if (MBD_2 and myid != broadcasterId and firstEchoSender != broadcasterId and 
        (deliveredMsgs.find(sourceMsgId) == deliveredMsgs.end())
    ) {
        std::vector<PID> sourcePath; // note that broadcasterId not added to path
        sourcePath.push_back(firstEchoSender); // != broadcasterId
        sourcePath.push_back(secondReadySender);
        for (int i = 0; i < msgPath.size(); i++) {
            sourcePath.push_back(msgPath[i]);
        }
        sourcePath.push_back(linkSenderId); 
        
        std::string sourcePathBitmask = std::string(numNodes, 0);
        for (int i = 0; i < sourcePath.size(); i++) { 
            sourcePathBitmask[sourcePath[i]] = 1;
        }
        
        if (pathsBitmasks.find(sourceMsgId) == pathsBitmasks.end()) {
            pathsBitmasks[sourceMsgId] = new std::vector<std::string>[numNodes];
        } 
        pathsBitmasks[sourceMsgId][linkSenderId].push_back(sourcePathBitmask);
        
        bool indirectDeliver;
        if (precomputeSets) { 
            std::string fullPathBitmask(sourcePathBitmask);
            fullPathBitmask[linkSenderId] = 1;
            indirectDeliver = recomputeAllNonIntersectingSets(sourceMsgId, fullPathBitmask);
        } else {
            indirectDeliver = msgCanBeDelivered(sourceMsgId, linkSenderId);
        }
        
        if (indirectDeliver) {
            deliveredMsgs.insert(sourceMsgId);
            rcvEchos[broadId].insert(broadcasterId);
            std::cout << "INDIRECT DELIVER SOURCE ECHO 4" << std::endl;
        }
    }
    
    // Study source path possibly embedded in Ready
    if (MBD_2 and myid != broadcasterId and secondReadySender != broadcasterId and 
        (deliveredMsgs.find(sourceMsgId) == deliveredMsgs.end())
    ) {
        std::vector<PID> sourcePath; // note that broadcasterId not added to path
        sourcePath.push_back(secondReadySender);
        for (int i = 0; i < msgPath.size(); i++) {
            sourcePath.push_back(msgPath[i]);
        }
        sourcePath.push_back(linkSenderId); 
        
        std::string sourcePathBitmask = std::string(numNodes, 0);
        for (int i = 0; i < sourcePath.size(); i++) { 
            sourcePathBitmask[sourcePath[i]] = 1;
        }
        
        if (pathsBitmasks.find(sourceMsgId) == pathsBitmasks.end()) {
            pathsBitmasks[sourceMsgId] = new std::vector<std::string>[numNodes];
        } 
        pathsBitmasks[sourceMsgId][linkSenderId].push_back(sourcePathBitmask);
        
        bool indirectDeliver;
        if (precomputeSets) { 
            std::string fullPathBitmask(sourcePathBitmask);
            fullPathBitmask[linkSenderId] = 1;
            indirectDeliver = recomputeAllNonIntersectingSets(sourceMsgId, fullPathBitmask);
        } else {        
            indirectDeliver = msgCanBeDelivered(sourceMsgId, linkSenderId);
        }
        
        if (indirectDeliver) {
            deliveredMsgs.insert(sourceMsgId);
            rcvEchos[broadId].insert(broadcasterId);
            std::cout << "INDIRECT DELIVER SOURCE ECHO 5" << std::endl;
        }
    }
    
    // Compute whether Echo can be delivered
    bool echoDirectDeliver = MOD_1_BONOMI and linkSenderId == firstEchoSender;
    
    bool echoIndirectDeliver = false;
    if (precomputeSets) { 
        std::string fullPathBitmask(echoPathBitmask);
        fullPathBitmask[linkSenderId] = 1;
        if (processEcho) {
            echoIndirectDeliver = (!echoWasDelivered and !echoDirectDeliver and recomputeAllNonIntersectingSets(echoMsgId, fullPathBitmask));
        }
    } else {
        echoIndirectDeliver = (!echoWasDelivered and !echoDirectDeliver and msgCanBeDelivered(echoMsgId, linkSenderId));
    }
    
    bool echoJustDelivered = !echoWasDelivered and (echoDirectDeliver or echoIndirectDeliver);
    
    // Compute whether Ready can be delivered
    bool readyDirectDeliver = MOD_1_BONOMI && linkSenderId == secondReadySender;
    
    bool readyIndirectDeliver = false;
    if (precomputeSets) { 
        std::string fullPathBitmask2(readyPathBitmask);
        fullPathBitmask2[linkSenderId] = 1;
        readyIndirectDeliver = (!readyWasDelivered and !readyDirectDeliver and recomputeAllNonIntersectingSets(readyMsgId, fullPathBitmask2));
    } else {
        readyIndirectDeliver = (!readyWasDelivered and !readyDirectDeliver and msgCanBeDelivered(readyMsgId, linkSenderId));
    }
    
    bool readyJustDelivered = !readyWasDelivered and (readyDirectDeliver or readyIndirectDeliver);
    
    // Send own Echo or Ready if possible
    // TODO: try to encapsulate with another one if possible? (several cases to handle...)
    // TODO: try to change order
    bool justDelivered = false;
    if (echoJustDelivered or readyJustDelivered) {
        
        if (echoJustDelivered) { 
            deliveredMsgs.insert(echoMsgId); 
            rcvEchos[broadId].insert(firstEchoSender);
            
            if (DEBUG) {
                std::cout<<KRED<<"\t"<<myid<<" delivers RE ECHO("<<firstEchoSender<<") and has "<<rcvEchos[broadId].size()<<"/"<<(numFaulty+1)<<" ECHOs [";
                std::set<unsigned int>::iterator it;
                int a = 0;
                for (it=rcvEchos[broadId].begin(); it!=rcvEchos[broadId].end(); ++it) {
                    if (a!=0) std::cout << ",";
                    std::cout << *it;
                    a++;
                }
                std::cout <<"]" << KNRM<<std::endl;
            }
        } 
        if (readyJustDelivered) { 
            deliveredMsgs.insert(readyMsgId); 
            rcvReadys[broadId].insert(secondReadySender);
            
            if (DEBUG) { 
                std::cout<<KRED<<"\t"<<myid<<" delivers RE READY("<<secondReadySender<<") and has "<<rcvReadys[broadId].size()<<"/"<<byzQuorumSize<<" READYs [";
                std::set<unsigned int>::iterator it;
                int a = 0;
                for (it=rcvReadys[broadId].begin(); it!=rcvReadys[broadId].end(); ++it) {
                    if (a!=0) std::cout << ",";
                    std::cout << *it;
                    a++;
                }
                std::cout <<"]" << KNRM<<std::endl;
            }
        }
        
        // Can we deliver?
        if (rcvReadys[broadId].size() >= 2*numFaulty+1 and deliveredBroad.find(broadId) == deliveredBroad.end()) {
            
            std::cout << KGRN << myid<<" RE DELIVERED ["<<broadcasterId<<","<<broadcastId<<"]" << KNRM << std::endl;
            
//             deliveredBroad.insert(broadId); 
            deliveredBroad[broadId] = std::make_pair(getCurrentTime(), true); 
            printDeliverTime(broadcasterId, broadcastId);
            
            justDelivered = true;
        }
        
        // Generate own Ready? 
//         bool sentReadyEcho = false;
//         bool sentEchoEcho = false;
        // skip directly to sending Ready
        
        if (isSelectedToSendReadys(broadcasterId) and !contains(rcvReadys[broadId], myid) and 
            (rcvEchos[broadId].size() >= byzQuorumSize or rcvReadys[broadId].size() >= numFaulty+1)) {
//             if (MBD_4) {
//                 send_readyEcho(echoMsgId, val, path);
//                 sentReadyEcho = true;// own ready + rcv echo
//                 return;
//             } else {
                send_ready(echoMsgId, val);
//             }
        } // Or else try to send Echo?
        else if (isSelectedToSendEchos(broadcasterId) and !contains(rcvEchos[broadId], myid) and 
            (contains(rcvEchos[broadId], broadcasterId) or rcvEchos[broadId].size() >= numFaulty + 1)) {   
//             if (MBD_3) {
//                 send_echoEcho(msgId, val, path); 
//                 sentEchoEcho = true;
//                 return;
//             } else {
                send_echo(echoMsgId, val); // own echo
//             }
        }
    }
    
    // TODO: Relay Echo and Ready messages: only Echo, only Ready, or both 
    int localId = (MBD_1)? bidToLocalId[broadId]: 0;
    
    if (processEcho and !processReady) { // forward only Echo
//     if (processEcho) {
//         std::cout << "fwd only echo" << std::endl;
        EchoInit e1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, emptyPath);
//         EchoInit *e1_ptr = NULL;
        EchoLocalId e2(this->myid, localId, firstEchoSender, emptyPath);
//         EchoLocalId * e2_ptr = NULL;
        Echo e3(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, emptyPath);
//         Echo *e3_ptr = NULL;
        
        EchoInit e4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, echoPath);
//         EchoInit *e4_ptr = NULL;
        EchoLocalId e5(this->myid, localId, firstEchoSender, echoPath);
//         EchoLocalId *e5_ptr = NULL;
        Echo e6(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, echoPath);
//         Echo *e6_ptr = NULL;
        
        // Relay Echo message with modified path to neighbors
//         if (MBD_11 or !MBD_2 or linkSenderId != broadcasterId) { // avoid relaying send msgs
            for (int i = 0; i < selfNeighbors.size(); i++) {
                PID curNeighbor = selfNeighbors[i];
                
                if (curNeighbor == firstEchoSender) {continue;}
                
                if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(curNeighbor) != neighborsToAvoid[broadId].end()) {continue;}
                
                if (MBD_8 and 
                    rcvReadys.find(broadId)!=rcvReadys.end() and
                    contains(rcvReadys[broadId], curNeighbor)) {continue;}
                
                // Send empty path upon delivery
                if (MOD_2_BONOMI and echoJustDelivered) {
                    
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][curNeighbor]) {
//                             if (e1_ptr==NULL) {
//                                 e1_ptr = new EchoInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, emptyPath);
//                             }
                            this->pnet.send_msg(e1, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                            pidsThatKnowLocalId[localId].set(curNeighbor);
                        } else { // send Echo_localId
//                             if (e2_ptr==NULL) {
//                                 e2_ptr = new EchoLocalId(this->myid, localId, firstEchoSender, emptyPath);
//                             }
                            this->pnet.send_msg(e2, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        }
                    } else {
//                         if (e3_ptr==NULL) {
//                             e3_ptr = new Echo(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, emptyPath);
//                         }
                        this->pnet.send_msg(e3, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
            
                } else { // else relay with modified path
                    if (!echoPathBitmask[curNeighbor] and curNeighbor!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][curNeighbor]) {
//                                 if (e4_ptr==NULL) {
//                                     e4_ptr = new EchoInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, echoPath);
//                                 }
                                this->pnet.send_msg(e4, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                                pidsThatKnowLocalId[localId].set(curNeighbor);
                            } else { // send Echo_localId
//                                 if (e5_ptr==NULL) {
//                                     e5_ptr = new EchoLocalId(this->myid, localId, firstEchoSender, echoPath);
//                                 }
                                this->pnet.send_msg(e5, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                            }
                        } else {
//                             if (e6_ptr==NULL) { 
//                                 e6_ptr = new Echo(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, echoPath);
//                             }
                            this->pnet.send_msg(e6, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        }
                    }
                }
            }
//         }
    } 
    else if (!processEcho and processReady) { // forward only Ready
//         std::cout << myid << " fwd only ready" << std::endl;
        
        if (MBD_1) {
            localId = bidToLocalId[broadId];
        } 
        
        ReadyInit r1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, emptyPath);
//         ReadyInit *r1_ptr = NULL;
        ReadyLocalId r2(this->myid, localId, secondReadySender, emptyPath);
//         ReadyLocalId *r2_ptr = NULL;
        Ready r3(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, emptyPath); 
//         Ready *r3_ptr = NULL;
        
        ReadyInit r4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, readyPath);
//         ReadyInit *r4_ptr = NULL;
        ReadyLocalId r5(this->myid, localId, secondReadySender, readyPath);
//         ReadyLocalId * r5_ptr = NULL;
        Ready r6(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, readyPath);
//         Ready * r6_ptr = NULL;
        
        // Relay Ready message with modified path to neighbors
        for (int i = 0; i < selfNeighbors.size(); i++) {
            PID curNeighbor = selfNeighbors[i];
            
            if (curNeighbor == secondReadySender) {continue;}
            
            if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and  neighborsToAvoid[broadId].find(curNeighbor) != neighborsToAvoid[broadId].end()) {continue;}
            
            // Relay with empty path upon delivery 
            if (MOD_2_BONOMI and readyJustDelivered) {
                
                if (MBD_1) {
                    if (not pidsThatKnowLocalId[localId][curNeighbor]) {
//                         if (r1_ptr==NULL) {
//                             r1_ptr = new ReadyInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, emptyPath);
//                         }
                        this->pnet.send_msg(r1, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        pidsThatKnowLocalId[localId].set(curNeighbor);
                    } else { // send Ready_localId
//                         if (r2_ptr==NULL) {
//                             r2_ptr = new ReadyLocalId(this->myid, localId, secondReadySender, emptyPath);
//                         }
                        this->pnet.send_msg(r2, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                } else {
//                     if (r3_ptr==NULL) {
//                         r3_ptr = new Ready(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, emptyPath); 
//                     }
                    this->pnet.send_msg(r3, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                }
        
            } else { // or relay with modified path
                
                if (!readyPathBitmask[curNeighbor] and curNeighbor!=linkSenderId) { // curNeighbor not in path of rcvd msg
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][curNeighbor]) {
//                             if (r4_ptr==NULL) {
//                                 r4_ptr = new ReadyInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, readyPath);
//                             }
                            this->pnet.send_msg(r4, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                            pidsThatKnowLocalId[localId].set(curNeighbor);
                        } else { // send Echo_localId
//                             if (r5_ptr==NULL) {
//                                 r5_ptr = new ReadyLocalId(this->myid, localId, secondReadySender, readyPath);
//                             }
                            this->pnet.send_msg(r5, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                        }
                    } else {
//                         if (r6_ptr==NULL) {
//                             r6_ptr = new Ready(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, readyPath);
//                         }
                        this->pnet.send_msg(r6, (salticidae::PeerId) std::get<1>(peers[curNeighbor]));
                    }
                }
            }
        }
        
    } else if (processEcho and processReady) { // both Echo and Ready to forward, but need to check for each neighbour
//         std::cout << myid << " fwd both ready and echo" << std::endl;
        if (MBD_1) {
            localId = bidToLocalId[broadId];
        } 
        
//         EchoInit *e1_ptr = NULL;
        EchoInit e1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, emptyPath);
//         EchoLocalId *e2_ptr = NULL;
        EchoLocalId e2(this->myid, localId, firstEchoSender, emptyPath);
//         Echo *e3_ptr = NULL;
        Echo e3(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, emptyPath);
//         EchoInit *e4_ptr = NULL;
        EchoInit e4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, echoPath);
//         EchoLocalId *e5_ptr = NULL;
        EchoLocalId e5(this->myid, localId, firstEchoSender, echoPath);
//         Echo *e6_ptr = NULL;
        Echo e6(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, echoPath);
        
        ReadyInit r1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, emptyPath);
//         ReadyInit *r1_ptr = NULL;
        ReadyLocalId r2(this->myid, localId, secondReadySender, emptyPath);
//         ReadyLocalId *r2_ptr = NULL;
        Ready r3(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, emptyPath);
//         Ready *r3_ptr = NULL;
        
        ReadyInit r4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, readyPath);
//         ReadyInit *r4_ptr = NULL;
        ReadyLocalId r5(this->myid, localId, secondReadySender, readyPath);
//         ReadyLocalId *r5_ptr = NULL;
        Ready r6(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, readyPath);
//         Ready *r6_ptr = NULL;

        // If both ready and echo delivered -> empty path
        ReadyEchoInit re1(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, secondReadySender, emptyPath);
//         ReadyEchoInit * re1_ptr = NULL;
        ReadyEchoLocalId re2(this->myid, localId, firstEchoSender, secondReadySender, emptyPath);
//         ReadyEchoLocalId * re2_ptr = NULL;
        ReadyEcho re3(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, secondReadySender, emptyPath);
//         ReadyEcho * re3_ptr = NULL;
        
        // otherwise if both are not delivered readyPath = echoPath
        ReadyEchoInit re4(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, secondReadySender, readyPath);
//         ReadyEchoInit * re4_ptr = NULL;
        ReadyEchoLocalId re5(this->myid, localId, firstEchoSender, secondReadySender, readyPath);
//         ReadyEchoLocalId *re5_ptr = NULL;
        ReadyEcho re6(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, secondReadySender, readyPath);
//         ReadyEcho *re6_ptr = NULL;
        
        // We can relay both the Echo and the Ready, but some neighbors need only one
        for (int j = 0; j < selfNeighbors.size(); j++) { 
            PID cur = selfNeighbors[j];
            
            if (MBD_9 and neighborsToAvoid.find(broadId) != neighborsToAvoid.end() and neighborsToAvoid[broadId].find(cur) != neighborsToAvoid[broadId].end()) {continue;}

            if (cur == firstEchoSender or (MBD_8 and rcvReadys.find(broadId)!=rcvReadys.end() and contains(rcvReadys[broadId], cur)) and cur!=secondReadySender) { // only ready for that neighbour
//                 std::cout << myid << "forward ready to "<<cur<<std::endl;
                
                // Relay with empty path upon delivery 
                if (MOD_2_BONOMI and readyJustDelivered) {
                    
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
//                             if (r1_ptr == NULL) { 
//                                 r1_ptr = new ReadyInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, emptyPath);
//                             }
                            this->pnet.send_msg(r1, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Ready_localId
//                             if (r2_ptr==NULL) {
//                                 r2_ptr = new ReadyLocalId(this->myid, localId, secondReadySender, emptyPath);
//                             }
                            this->pnet.send_msg(r2, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
//                         if (r3_ptr==NULL) {
//                             r3_ptr = new Ready(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, emptyPath);
//                         }
                        this->pnet.send_msg(r3, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
            
                } else { // or relay with modified path
                    
                    if (!readyPathBitmask[cur] and cur!=linkSenderId) { // curNeighbor not in path of rcvd msg
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
//                                 if (r4_ptr==NULL) {
//                                     r4_ptr = new ReadyInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, readyPath);
//                                 }
                                this->pnet.send_msg(r4, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
//                                 if (r5_ptr==NULL) {
//                                     r5_ptr = new ReadyLocalId(this->myid, localId, secondReadySender, readyPath);
//                                 }
                                this->pnet.send_msg(r5, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
//                             if (r6_ptr==NULL) { 
//                                 r6_ptr = new Ready(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, readyPath);   
//                             }
                            this->pnet.send_msg(r6, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
                }
                
            } else if (cur == secondReadySender) { // only echo for that neighbour
//                 std::cout << myid << "forward echo to "<<cur<<std::endl;
                if (MBD_8 and 
                rcvReadys.find(broadId)!=rcvReadys.end() and
                contains(rcvReadys[broadId], cur)) {continue;}
                
                // Send empty path upon delivery
                if (MOD_2_BONOMI and echoJustDelivered) {
                    
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
//                             if (e1_ptr==NULL) {
//                                 e1_ptr = new EchoInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, emptyPath);
//                             }
                            this->pnet.send_msg(e1, (salticidae::PeerId) std::get<1>(peers[cur]));
//                             this->pnet.send_msg(e1, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Echo_localId
//                             if (e2_ptr == NULL) {
//                                 e2_ptr = new EchoLocalId(this->myid, localId, firstEchoSender, emptyPath);
//                             }
                            this->pnet.send_msg(e2, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
//                         if (e3_ptr==NULL) {
//                             e3_ptr = new Echo(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, emptyPath);
//                         }
                        this->pnet.send_msg(e3, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
            
                } else { // else relay with modified path
                    if (!echoPathBitmask[cur] and cur!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
//                                 if (e4_ptr==NULL) {
//                                     e4_ptr = new EchoInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, echoPath);
//                                 }
                                
                                this->pnet.send_msg(e4, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
//                                 if (e5_ptr == NULL) {
//                                     e5_ptr = new EchoLocalId(this->myid, localId, firstEchoSender, echoPath);
//                                 }
                                this->pnet.send_msg(e5, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
//                             if (e6_ptr==NULL) {
//                                 e6_ptr = new Echo(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, echoPath);
//                             }
                            this->pnet.send_msg(e6, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
                }
                
            } else { // both the Ready and Echo for that neighbour

                // the ready and echo might have different paths!
                if (MOD_2_BONOMI and echoJustDelivered and readyJustDelivered) { 
                    // readyEcho with empty paths
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
//                             if (re1_ptr==NULL) {
//                                 re1_ptr = new ReadyEchoInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, secondReadySender, emptyPath);
//                             }
                            this->pnet.send_msg(re1, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Echo_localId
//                             if (re2_ptr==NULL) { 
//                                 re2_ptr = new ReadyEchoLocalId(this->myid, localId, firstEchoSender, secondReadySender, emptyPath);
//                             }
                            this->pnet.send_msg(re2, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
//                         if (re3_ptr==NULL) {
//                             re3_ptr = new ReadyEcho(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, secondReadySender, emptyPath);
//                         }
                        this->pnet.send_msg(re3, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
                } else if (MOD_2_BONOMI and !echoJustDelivered and !readyJustDelivered) {
                    // readyEcho with modified path
                    if (!readyPathBitmask[cur] and cur!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
//                                 if (re4_ptr==NULL) {
//                                     re4_ptr = new ReadyEchoInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, secondReadySender, readyPath);
//                                 }
                                this->pnet.send_msg(re4, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
//                                 if (re5_ptr==NULL) {
//                                     re5_ptr = new ReadyEchoLocalId(this->myid, localId, firstEchoSender, secondReadySender, readyPath);
//                                 }
                                this->pnet.send_msg(re5, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
//                             if (re6_ptr==NULL) {
//                                 re6_ptr = new ReadyEcho(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, secondReadySender, readyPath);
//                             }
                            this->pnet.send_msg(re6, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
                    // TODO: some neighbors don't need both 
                } else if (MOD_2_BONOMI and echoJustDelivered and !readyJustDelivered) { 
                    // echo with empty path, ready with modified path
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
//                             if (e1_ptr==NULL) {
//                                 e1_ptr = new EchoInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, emptyPath);
//                             }
                            this->pnet.send_msg(e1, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Echo_localId
//                             if (e2_ptr == NULL) {
//                                 e2_ptr = new EchoLocalId(this->myid, localId, firstEchoSender, emptyPath);
//                             }
                            this->pnet.send_msg(e2, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
//                         if (e3_ptr==NULL) {
//                             e3_ptr = new Echo(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, emptyPath);
//                         }
                        this->pnet.send_msg(e3, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
                    if (!readyPathBitmask[cur] and cur!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
//                                 if (r4_ptr==NULL) {
//                                     r4_ptr = new ReadyInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, readyPath);
//                                 }
                                this->pnet.send_msg(r4, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
//                                 if (r5_ptr==NULL) {
//                                     r5_ptr = new ReadyLocalId(this->myid, localId, secondReadySender, readyPath);
//                                 }
                                this->pnet.send_msg(r5, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
//                             if (r6_ptr==NULL) { 
//                                 r6_ptr = new Ready(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, readyPath);   
//                             }
                            this->pnet.send_msg(r6, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
                } else {
                    // echo with modified path, ready with empty path
                    if (!echoPathBitmask[cur] and cur!=linkSenderId) {
                        if (MBD_1) {
                            if (not pidsThatKnowLocalId[localId][cur]) {
//                                 if (e4_ptr==NULL) {
//                                     e4_ptr = new EchoInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, firstEchoSender, echoPath);
//                                 }
                                this->pnet.send_msg(e4, (salticidae::PeerId) std::get<1>(peers[cur]));
                                pidsThatKnowLocalId[localId].set(cur);
                            } else { // send Echo_localId
//                                 if (e5_ptr == NULL) {
//                                     e5_ptr = new EchoLocalId(this->myid, localId, firstEchoSender, echoPath);
//                                 }
                                
                                this->pnet.send_msg(e5, (salticidae::PeerId) std::get<1>(peers[cur]));
                            }
                        } else {
//                             if (e6_ptr==NULL) {
//                                 e6_ptr = new Echo(payloadSize, val, this->myid, broadcasterId, broadcastId, firstEchoSender, echoPath);
//                             }
                            this->pnet.send_msg(e6, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    }
            
                    if (MBD_1) {
                        if (not pidsThatKnowLocalId[localId][cur]) {
//                             if (r1_ptr == NULL) { 
//                                 r1_ptr = new ReadyInit(payloadSize, val, this->myid, broadcasterId, broadcastId, localId, secondReadySender, emptyPath);
//                             }
                            this->pnet.send_msg(r1, (salticidae::PeerId) std::get<1>(peers[cur]));
                            pidsThatKnowLocalId[localId].set(cur);
                        } else { // send Ready_localId
//                             if (r2_ptr==NULL) {
//                                 r2_ptr = new ReadyLocalId(this->myid, localId, secondReadySender, emptyPath);
//                             }
                            this->pnet.send_msg(r2, (salticidae::PeerId) std::get<1>(peers[cur]));
                        }
                    } else {
//                         if (r3_ptr==NULL) {
//                             r3_ptr = new Ready(payloadSize, val, this->myid, broadcasterId, broadcastId, secondReadySender, emptyPath);
//                         }
                        this->pnet.send_msg(r3, (salticidae::PeerId) std::get<1>(peers[cur]));
                    }
        
                } 
            }
        
        }
        
    }
    
    if (justDelivered) { 
        clean_after_deliver(broadId);
    }
}

void OptBrachaDolevHandler::handle_readyEchoInit(ReadyEchoInit msg, const PeerNet::conn_t &conn) {

    bId broadId = {msg.broadcasterId, msg.broadcastId}; 
    
    if (minBidMeasure <= msg.broadcastId and msg.broadcastId < maxBidMeasure) {
        if (numReadyEchoInits.find(broadId) == numReadyEchoInits.end()) {
            numReadyEchoInits[broadId] = 1;
            bytesReadyEchoInits[broadId] = msg.sizeMsg();
        } else {
            numReadyEchoInits[broadId]++;
            bytesReadyEchoInits[broadId] += msg.sizeMsg();
        }  
        if (msg.broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    // Save the localId and associate it to the broadcast id
    // If not, create one that we will use for future communications about this broadcast 
    // complete code would need some more checks to verify that sender does not send multiple msgs
    
    localInfo li = {msg.broadcasterId, msg.broadcastId, payloadSize, msg.val}; 
    pidToLocalInfo[msg.linkSenderId][msg.localId] = li; // senderId -> localId -> localInfo
    
    // Do we have a local id for this content?
    ifNeededCreateLocalId(broadId, li);

    handle_readyEcho_2(msg.payloadSize, msg.val, msg.linkSenderId, msg.broadcasterId, msg.broadcastId, msg.firstEchoSender, msg.secondReadySender, msg.path, 0, false);
    
    consumeAllPending(msg.linkSenderId, msg.localId, conn);
}

void OptBrachaDolevHandler::handle_readyEchoLocalId(ReadyEchoLocalId msg, const PeerNet::conn_t &conn) {
    
    // Receive a message from a neighbour that contains a localId that will be used in the future
    
    // if asynchronous, translation might not succeed
    // Store message, and process it later
    if (pidToLocalInfo.find(msg.linkSenderId) == pidToLocalInfo.end() or pidToLocalInfo[msg.linkSenderId].find(msg.localId) == pidToLocalInfo[msg.linkSenderId].end()) {
        pendingReadyEchoLocalId[msg.linkSenderId][msg.localId].push(msg);
        return;
    }
    
    int localId = msg.localId; 
    localInfo li = pidToLocalInfo[msg.linkSenderId][localId];
    
    bId broadId = {li.broadcasterId, li.broadcastId};
    
    if (minBidMeasure <= li.broadcastId and li.broadcastId < maxBidMeasure) {
        if (numReadyEchoLocalIds.find(broadId) == numReadyEchoLocalIds.end()) {
            numReadyEchoLocalIds[broadId] = 1;
            bytesReadyEchoLocalIds[broadId] = msg.sizeMsg();
        } else {
            numReadyEchoLocalIds[broadId]++;
            bytesReadyEchoLocalIds[broadId] += msg.sizeMsg();
        }  
        if (li.broadcastId == numBcasts-1) {
            printStats();
        }
    }
    
    handle_readyEcho_2(li.payloadSize, li.val, msg.linkSenderId, li.broadcasterId, li.broadcastId, msg.firstEchoSender, msg.secondReadySender, msg.path, 0, false);
}
