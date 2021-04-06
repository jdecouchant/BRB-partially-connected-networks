#include "Peer.h"
#include "base.h"
#include "packets_m.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/packet/chunk/ByteCountChunk.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"

#include <math.h> // ceil
#include <string>
#include <map>

#include <bits/stdc++.h>

#include <stdio.h>
//#include <python3.6/Python.h>
#include <iostream>
#include <fstream>

using namespace std;

namespace inet {

Define_Module(Peer);

map<L3Address, int> Peer::addrToId;
vector<SimTime> Peer::deliverTime;
int Peer::deliverCount;

bool **Peer::connections;

int Peer::numFinish;
string Peer::outputPrefix;
double Peer::PAYLOAD_SIZE;

long Peer::numMsgTotal;
long Peer::numBitsTotal;
int Peer::nTransmissionsPayload;
long int Peer::cumulatedProcessingTime;

int Peer::getNextNodeToDst(int srcNodeID, int dstNodeID) {
    // for on-demand route finding
    // TODO sanity-check for self addressing
    cTopology::Node *curNode = topo->getNode(srcNodeID);
    // on-demand routing calculation
    topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(dstNodeID));
    // sanity-check if not connected at all
    if (!curNode || !curNode->getPath(0)) {
        EV << "ERROR: no path for routing found?!";
        return -1;
    }
    // identify gate for next hop on-demand
    cTopology::Node * nextNode = curNode->getPath(0)->getRemoteNode();
    // TODO: ugly
    for (int i = 0; i < nodesNbr; i++) {
        if (topo->getNode(i) == nextNode) {
            return i;
        }
    }
    return -1;
}

int Peer::getNextGateToDst(int srcNodeID, int dstNodeID) {
    // for on-demand route finding
    // TODO sanity-check for self addressing
    cTopology::Node *curNode = topo->getNode(srcNodeID);
    //    cout << "Num in links " << topo->getNode(0)->getNumOutLinks() << endl; //Node(srcNodeID)->getNumInLinks() << endl;
    // on-demand routing calculation
    topo->calculateUnweightedSingleShortestPathsTo(topo->getNode(dstNodeID));

    // sanity-check if not connected at all
    if (!curNode || !curNode->getPath(0)) {
        EV << "ERROR: no path for routing found?!";
        return -1;
    }

    // identify gate for next hop on-demand
    cGate *parentModuleGate = curNode->getPath(0)->getLocalGate();
    int gateIndex = parentModuleGate->getIndex();

    return gateIndex;
}

void Peer::initializeTopo() {
    topo = new cTopology("topo");
    vector<string> nedTypes;

    //    for (cModule::SubmoduleIterator it(getParentModule()->getParentModule()); !it.end(); ++it) {
    //        cModule *submodule = *it;
    //        cout << submodule->getFullName() << " " << submodule->getNedTypeName() << endl;
    //        nedTypes.push_back(submodule->getNedTypeName());
    //        topo->extractByNedTypeName(nedTypes);
    ////        break;
    //    }
    //    return;
    nedTypes.push_back("inet.node.inet.StandardHost");
    topo->extractByNedTypeName(nedTypes);


    //    cout << getParentModule()->getSubmodule("process", 0)->getFullName() << endl;
    //    nedTypes.push_back(getParentModule()->getSubmodule("Process", 0)->getNedTypeName());
    //    topo->extractByNedTypeName(nedTypes);

    int nodesCount = topo->getNumNodes();
    cout << "cTopology found " << nodesCount << " nodes\n";

    //    for (int i = 0; i < nodesCount; i++) {
    //        if (i != selfId) {
    //            cout << "Gate to use between " << selfId << " and " << i << ": " << getNextGateToDst(selfId, i) << endl;
    //        }
    //    }

    connections = new bool*[nodesCount];

    //    bool connections[nodesCount][nodesCount];
    for (int i = 0; i < nodesCount; i++) {
        connections[i] = new bool[nodesCount];
        for (int j = 0; j < nodesCount; j++) {
            if (i == j) {
                connections[i][j] = false;
            } else {
                int nextNodeId = getNextNodeToDst(i, j);
                if (getNextNodeToDst(i, j) == j) {
                    connections[i][j] = true;
                } else {
                    connections[i][j] = false;
                }
            }
        }
    }

    for (int i = 0; i < nodesNbr; i++) {
        for (int j = 0; j < nodesNbr; j++) {
            EV << connections[i][j] << " ";
        }
        EV << endl;
    }
}

vector<int> Peer::getAllNeighbors() {
    vector<int> neighbors;
    for (int j = 0; j < nodesNbr; j++) {
        if (connections[selfId][j] == 1) {
            neighbors.push_back(j);
        }
    }
    return neighbors;
}

void Peer::initialize(int stage) {
    ApplicationBase::initialize(stage);
    localPort = par("localPort");

    roundId = 0;

    vector<int> echos;
    vector<int> readys;

    if (stage == INITSTAGE_LOCAL) {

        //        cPar * startTimePar = &par("startTimePar");
        startTime = (simtime_t) par("startTimePar"); //*startTimePar;

        nodesNbr = par("nPar");
        f = par("fPar");
        k = par("kPar");

        int tmp = nodesNbr + f + 1;
        if (tmp % 2 == 1)
            tmp++;
        quorumSize = tmp/2;

#ifdef BRACHADOLEV2
        int V = nodesNbr*(nodesNbr+1);
        rgraph = new int*[V];
        for (int i = 0; i < V; i++) {
            rgraph[i] = new int[V];
        }
#endif

    }
}

void Peer::finish() {

#ifdef BRACHA
    BRACHAfinish();
#endif
#ifdef DOLEV
    DOLEVfinish();
#endif
#ifdef BRACHADOLEV
    BRACHADOLEVfinish();
#endif
#ifdef BRACHACPA
    BRACHACPAfinish();
#endif
#ifdef BRACHADOLEV2
    BRACHADOLEV2finish();
#endif
#ifdef V1
    V1finish();
#endif
#ifdef V2
    V2finish();
#endif

    if (selfId == nodesNbr-1) {
        for (int i = 0; i < nodesNbr; i++) {
            delete[] connections[i];
        }
        delete[] connections;
    }

    numFinish++;
    std::string s = outputPrefix;
    std::ofstream f;

    if (selfId == 0) {
        s = outputPrefix + ".delivers"; //"stats/file";
        f.open(s, std::ofstream::out | std::ofstream::app);
        f << deliverTime.size() << endl;
        f.close();

        s = outputPrefix + ".lat"; //"stats/file";
        f.open(s, std::ofstream::out | std::ofstream::app);
        for (auto dtime: deliverTime)
            f <<  dtime << endl;
        f.close();

        s = outputPrefix + ".proc"; //"stats/file";
        f.open(s, std::ofstream::out | std::ofstream::app);
        f << (cumulatedProcessingTime / nodesNbr) << endl;
        f.close();
    }

    s = outputPrefix + ".msgs"; //"stats/file";
    f.open(s, std::ofstream::out | std::ofstream::app);
    f << this->numSentMsg << endl;
    f.close();

    s = outputPrefix + ".bits"; //"stats/file";
    f.open(s, std::ofstream::out | std::ofstream::app);
    f << this->numBitsSent << endl;
    f.close();

    ApplicationBase::finish();
}

bool Peer::sortbysec(const pair<Packet *, SimTime> &a, const pair<Packet *, SimTime> &b) {
    return (a.second < b.second);
}

void Peer::handleMessageWhenUp(cMessage *msg) {

    if (msg->isSelfMessage()) {
        // Initial send by the source of the message (process 0)
        if (roundId == 0) {
            if (selfId == 0) {
                deliverCount = 0;
                delay = false;
#ifdef BRACHA
                BriefPacket * bp = BRACHAfirstMessage();
#endif
#ifdef DOLEV
                BriefPacket * bp = DOLEVfirstMessage();
#endif
#ifdef BRACHADOLEV
                BriefPacket * bp = BRACHADOLEVfirstMessage();
#endif
#ifdef BRACHACPA
                BriefPacket * bp = BRACHACPAfirstMessage();
#endif
#ifdef BRACHADOLEV2
                BriefPacket * bp = BRACHADOLEV2firstMessage();
#endif
#ifdef V1
                BriefPacket * bp = V1firstMessage();
#endif
#ifdef V2
                BriefPacket * bp = V2firstMessage();
#endif

                vector<int> nids; // Send to neighbors
                for (int j = 0; j < nodesNbr; j++) {
                    if (connections[selfId][j] == 1) {
                        nids.push_back(j);
                        if (OPTIM_SEND_2f1 and nids.size() >= 2*f+1) {
                            break;
                        }
                    }
                }
                sendTo(bp, nids);
                delay = true;
                msgId++;
            }
            roundId++;

        } else {
            //            cout << selfId << " received channel timer message" << endl;
            //            cout << "\t" << destId << endl;
            int destId;
            std::istringstream iss (msg->getName());
            iss >> destId;


            //            sort(pendingMsgsPerNeighbors[destId].begin(), pendingMsgsPerNeighbors[destId].end(), sortbysec);
            Packet *pk = pendingMsgsPerNeighbors[destId].front().first;
            pendingMsgsPerNeighbors[destId].erase(pendingMsgsPerNeighbors[destId].begin());

            if (pendingMsgsPerNeighbors[destId].size() > 0) {
                scheduleAt(pendingMsgsPerNeighbors[destId][0].second, msg);
            }

            //            cout << selfId << " sending message with debugid = " << ((BriefPacket *) bp->peekAtBack().get())->getDebugId() << endl;
            socket.sendTo(pk, nodesAddr[destId], 9999);
        }

    } else {
        socket.processMessage(msg); // and then call socketDataArrived
    }
}

void Peer::socketDataArrived(UdpSocket *socket, Packet *packet) {
    // process incoming packet
    //    cout << "begin receiveStream" << endl;
    receiveStream(packet);
    //    cout << "end receiveStream" << endl;
}

void Peer::socketErrorArrived(UdpSocket *socket, Indication *indication) {
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void Peer::socketClosed(UdpSocket *socket) {
    if (operationalState == State::STOPPING_OPERATION)
        startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void Peer::receiveStream(Packet *pk) {

    EV_INFO << "Video stream packet: " << UdpSocket::getReceivedPacketInfo(pk) << endl;
    auto bp = pk->peekAtBack().get();

    BriefPacket * x = (BriefPacket *) pk->peekAtBack().get();
    receiveECHO_or_READY(x);

    delete pk;
}

void Peer::deleteBriefPacket(BriefPacket *bp) {
    bp->~BriefPacket();
    delete bp;
}

void Peer::sendTo(BriefPacket * bp, vector<int> ids) {

    int count = 0;
    for (int destId : ids) {
        count++;

        //
        // Compute the size of the message
        bp->setIncludeData(false);
        if (bp->getMsgType() == ECHO) {
            // One could reduce a bit the consumption by removing the echoOrReadySender in the Send message (using another message type)
            if (bp->getEchoOrReadySender() == bp->getBroadcasterId()) { // SEND message
                if (OPTIM_MESSAGE_FIELDS) {
                    bp->setChunkLength(B( ceil( (MSG_TYPE_SIZE+MSG_ID_SIZE+PAYLOAD_SIZE)/8 ))); //+ PATH_LENGTH+PROCESS_ID_SIZE*bp->getPathArraySize()));
                } else {
                    bp->setChunkLength(B( ceil( (MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PATH_LENGTH_SIZE + PAYLOAD_SIZE)/8 ))); //+PROCESS_ID_SIZE*bp->getPathArraySize()));
                }
                bp->setIncludeData(true);
            } else {
                if (OPTIM_MESSAGE_FIELDS) {
                    int numBits = MSG_TYPE_SIZE + MSG_ID_BIT + LOCAL_ID_BIT + PAYLOAD_PRESENCE_BIT + SELF_MSG_BIT;
                    if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED and rcvMsgFrom[destId]) {
                        numBits += LOCAL_ID_SIZE;
                    } else {
                        numBits += PROCESS_ID_SIZE + MSG_ID_SIZE + LOCAL_ID_SIZE + PAYLOAD_SIZE;
                        bp->setIncludeData(true);
                        nTransmissionsPayload++;
                    }
                    if (bp->getEchoOrReadySender() != bp->getLinkSenderId()) { // own echo or ready sender, related to self_msg_bit
                        numBits += PROCESS_ID_SIZE; // id of the node who sent the echo/ready
//                    } else { // otherwise add path
                        numBits += PATH_LENGTH_SIZE + bp->getPathArraySize() * PROCESS_ID_SIZE; // associated path
                    }
                    bp->setChunkLength(B( ceil(numBits/8.0) ));
                } else {
                    if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED and rcvMsgFrom[destId]) {
                        bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE + MSG_ID_SIZE)+PROCESS_ID_SIZE + PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize())/8.0)));
                    } else {
                        bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PROCESS_ID_SIZE + PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize()+PAYLOAD_SIZE)/8.0)));
                        bp->setIncludeData(true);
                        nTransmissionsPayload++;
                    }
                }
            }
        } else if (bp->getMsgType() == READY) { // To resume from here
            if (OPTIM_MESSAGE_FIELDS) {
                int numBits = MSG_TYPE_SIZE + MSG_ID_BIT + LOCAL_ID_BIT + PAYLOAD_PRESENCE_BIT + SELF_MSG_BIT;
                if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED and rcvMsgFrom[destId]) {
                    numBits += LOCAL_ID_SIZE;
                } else {
                    numBits += PROCESS_ID_SIZE + MSG_ID_SIZE + LOCAL_ID_SIZE + PAYLOAD_SIZE;
                    bp->setIncludeData(true);
                    nTransmissionsPayload++;
                }
                if (bp->getEchoOrReadySender() != bp->getLinkSenderId()) { // own echo or ready sender
                    numBits += PROCESS_ID_SIZE;
//                } else { // otherwise add path
                    numBits += PATH_LENGTH_SIZE + bp->getPathArraySize() * PROCESS_ID_SIZE;
                }
                bp->setChunkLength(B( ceil(numBits/8.0) ));
            } else {
                if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED and rcvMsgFrom[destId]) {
                    bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE + MSG_ID_SIZE)+PROCESS_ID_SIZE + PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize())/8.0)));
                } else {
                    bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PROCESS_ID_SIZE + PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize()+PAYLOAD_SIZE)/8.0)));
                    bp->setIncludeData(true);
                    nTransmissionsPayload++;
                }
            }
        } else if (bp->getMsgType() == READY_ECHO) {
            if (OPTIM_MESSAGE_FIELDS) {
                int numBits = MSG_TYPE_SIZE + MSG_ID_BIT + LOCAL_ID_BIT + PAYLOAD_PRESENCE_BIT + SELF_MSG_BIT;
                if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED and rcvMsgFrom[destId]) {
                    numBits += LOCAL_ID_SIZE;
                } else {
                    numBits += PROCESS_ID_SIZE + MSG_ID_SIZE + LOCAL_ID_SIZE + PAYLOAD_SIZE;
                    bp->setIncludeData(true);
                    nTransmissionsPayload++;
                }
                if (bp->getEchoOrReadySender() != bp->getLinkSenderId()) { // own echo or ready sender
                    numBits += PROCESS_ID_SIZE;
//                } else { // otherwise add path
                    numBits += PATH_LENGTH_SIZE + bp->getPathArraySize() * PROCESS_ID_SIZE;
                }
                numBits += PROCESS_ID_SIZE; // additional embedded echo
                bp->setChunkLength(B( ceil(numBits/8.0) ));
            } else {
                if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED and rcvMsgFrom[destId]) {
                    bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE + MSG_ID_SIZE)+PROCESS_ID_SIZE+PROCESS_ID_SIZE+ PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize())/8.0)));
                } else {
                    bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PROCESS_ID_SIZE+PROCESS_ID_SIZE+PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize()+PAYLOAD_SIZE)/8.0)));
                    bp->setIncludeData(true);
                    nTransmissionsPayload++;
                }
            }
        } else if (bp->getMsgType() == ECHO_ECHO) {
            if (OPTIM_MESSAGE_FIELDS) {
                int numBits = MSG_TYPE_SIZE + MSG_ID_BIT + LOCAL_ID_BIT + PAYLOAD_PRESENCE_BIT + SELF_MSG_BIT;
                if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED and rcvMsgFrom[destId]) {
                    numBits += LOCAL_ID_SIZE;
                } else {
                    numBits += PROCESS_ID_SIZE + MSG_ID_SIZE + LOCAL_ID_SIZE + PAYLOAD_SIZE;
                    bp->setIncludeData(true);
                    nTransmissionsPayload++;
                }
                if (bp->getEchoOrReadySender() != bp->getLinkSenderId()) { // own echo or ready sender
                    numBits += PROCESS_ID_SIZE;
//                } else { // otherwise add path
                    numBits += PATH_LENGTH_SIZE + bp->getPathArraySize() * PROCESS_ID_SIZE;
                }
                numBits += PROCESS_ID_SIZE; // additional embedded echo
                bp->setChunkLength(B( ceil(numBits/8.0) ));
            } else {
                if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED and rcvMsgFrom[destId]) {
                    bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE + MSG_ID_SIZE)+PROCESS_ID_SIZE+PROCESS_ID_SIZE+ PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize())/8.0)));
                } else {
                    bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+(PROCESS_ID_SIZE+MSG_ID_SIZE)+PROCESS_ID_SIZE+PROCESS_ID_SIZE+PATH_LENGTH_SIZE+PROCESS_ID_SIZE*bp->getPathArraySize()+PAYLOAD_SIZE)/8.0)));
                    bp->setIncludeData(true);
                    nTransmissionsPayload++;
                }
            }
        } else if (bp->getMsgType() == PAYLOAD_ACK) {
            bp->setChunkLength(B( ceil((MSG_TYPE_SIZE+LOCAL_ID_SIZE)/8.0) ));
        } else {
            cout << "sendTo: msgType not supported!" << endl;
            return;
        }

        numBitsSent += 8*B(bp->getChunkLength()).get();

        Packet *briefPacket;
        if (bp->getMsgType() == ECHO) {
            briefPacket = new Packet("ECHO");
        } else if (bp->getMsgType() == READY) {
            briefPacket = new Packet("READY");
        } else if (bp->getMsgType() == READY_ECHO) {
            briefPacket = new Packet("READY_ECHO");
        } else if (bp->getMsgType() == ECHO_ECHO) {
            briefPacket = new Packet("ECHO_ECHO");
        } else if (bp->getMsgType() == PAYLOAD_ACK) {
            briefPacket = new Packet("PAYLOAD_ACK");
        }

//        bp->setIncludeData(false);
//        if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED) {
//            //            cout << "optim payload avoid if received" << endl;
//            //            cout << B(bp->getChunkLength()) + B(PAYLOAD_ID_SIZE) << " with optim" << endl;
//            //            cout << B(bp->getChunkLength()) + B(msgSize) + B(PAYLOAD_ID_SIZE) << " without optim" << endl;
//            if (bp->getMsgType() == PAYLOAD_ACK) {
//                numBitsSent += B(bp->getChunkLength()).get();
//            } else {
//                if (rcvMsgFrom[destId]) {
//                    bp->setChunkLength( B(bp->getChunkLength()) + B(PAYLOAD_ID_SIZE) - B(PROCESS_ID_SIZE - MSG_ID_SIZE)); // message id is replaced by an ID that the two nodes agreed on
//                    numBitsSent += B(bp->getChunkLength()).get() + PAYLOAD_ID_SIZE - (PROCESS_ID_SIZE+MSG_ID_SIZE);
//                } else {
//                    //                rcvMsgFrom[destId] = true;
//                    bp->setChunkLength( B(bp->getChunkLength()) + B(PAYLOAD_SIZE) + B(PAYLOAD_ID_SIZE)); // In Bytes
//                    numBitsSent += B(bp->getChunkLength()).get() + PAYLOAD_SIZE + PAYLOAD_ID_SIZE;
//                    nTransmissionsPayload++;
//                    bp->setIncludeData(true);
//                    //                    cout << selfId << " sends payload to " << destId << endl;
//                }
//            }
//        } else {
//            //            cout << "optim payload none" << endl;
//            bp->setChunkLength( B(bp->getChunkLength()) + B(PAYLOAD_SIZE) ); // In Bytes
//            numBitsSent += B(bp->getChunkLength()).get();
//            nTransmissionsPayload++;
//            bp->setIncludeData(true);
//        }

        rcvMsgFrom[destId] = true; // Useful when sending at most once the payload to a neighbor

        bp->setDebugId(debugId++);
        BriefPacket &tmp = *bp;
        const auto& payload = makeShared<BriefPacket>(tmp);
        briefPacket->insertAtBack(payload);

        SimTime emissionTime;
        if (delay) {
//            cout << "Delay msg sending by " << p << " micros" << endl;
//            //        cout << "scale = " << SimTime::getScale() << endl;
            auto p = std::chrono::duration_cast<std::chrono::microseconds>(high_resolution_clock::now() - timeMsgReception).count();
//            cout << "processing delay = " << p << endl;
            SimTime processingDelay = SimTime(p, SIMTIME_US);
//            cout << "ASYNC value " << ASYNC << endl;
            if (ASYNC) {
                emissionTime = simTime() +  processingDelay + min(80.0, max(0.0, (*distribution[destId])(generator))); //rand() % 100; //accumulatedProcessing+
            } else {
                emissionTime = simTime() +  processingDelay;
            }
//            cout << "before emission time = " << (emissionTime-processingDelay) << " to " << emissionTime << endl;
        } else {
//            cout << "ASYNC value " << ASYNC << endl;
            if (ASYNC) {
                emissionTime = simTime() + min(80.0, max(0.0, (*distribution[destId])(generator)));
            } else {
                emissionTime = simTime();
            }
        }
        //SimTime emissionTime = simTime() + max(0.0, (*distribution[destId])(generator)); //rand() % 100;

        vector<pair<Packet *, SimTime>>::iterator iter = pendingMsgsPerNeighbors[destId].begin();
        while (iter != pendingMsgsPerNeighbors[destId].end() && iter->second < emissionTime) {
            iter++;
        }
        pendingMsgsPerNeighbors[destId].insert(iter, make_pair(briefPacket, emissionTime));

        //        pendingMsgsPerNeighbors[destId].push_back(make_pair(briefPacket, emissionTime));
        //        sort(pendingMsgsPerNeighbors[destId].begin(), pendingMsgsPerNeighbors[destId].end(), sortbysec);
        SimTime emissionFirst = pendingMsgsPerNeighbors[destId].front().second;

        if (!processLinkTimers[destId]->isScheduled()) {
            scheduleAt(emissionFirst, processLinkTimers[destId]);
        } else {
            cancelEvent(processLinkTimers[destId]);
            scheduleAt(emissionFirst, processLinkTimers[destId]);
        }
        //        socket.sendTo(briefPacket, nodesAddr[destId], 9999);
        numSentMsg++;
    }
    deleteBriefPacket(bp);
}

void Peer::receiveECHO_or_READY(BriefPacket *x) {

    timeMsgReception = high_resolution_clock::now();
    auto start = high_resolution_clock::now();
    rcvMsgFrom[x->getLinkSenderId()] = true;
#ifdef BRACHA // Works only on a fully connected network
    BRACHAreceiveMessage(x);
#endif
#ifdef DOLEV
    DOLEVreceiveMessage(x);
#endif
#ifdef BRACHADOLEV
    BRACHADOLEVreceiveMessage(x);
#endif
#ifdef BRACHACPA
    BRACHACPAreceiveMessage(x, true);
#endif
#ifdef BRACHADOLEV2
    //    cout << "\t" << selfId << " receives message from " << x->getLinkSenderId() << endl;
    BRACHADOLEV2receiveMessage(x);
    //        accumulatedProcessingTime = simTime() - timeMsgReception;
#endif
#ifdef V1
    V1receiveMessage(x);
#endif
#ifdef V2
    V2receiveMessage(x);
#endif
    auto end = high_resolution_clock::now();
//    if (selfId == 0) {
    cumulatedProcessingTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
//    cout << "cumulated processing time = " << cumulatedProcessingTime << endl;
//    }
}

void Peer::handleStartOperation(LifecycleOperation *operation) {
    simtime_t startTimePar = par("startTimePar");
    simtime_t startTime = std::max(startTimePar, simTime());
    //    scheduleAt(startTime, selfMsg);

    socket.setOutputGate(gate("socketOut"));
    socket.bind(localPort);
    socket.setCallback(this);

    for (int nodeId = 0; nodeId < nodesNbr; nodeId++) { // TODO: avoid global copies
        nodesAddr.push_back(L3AddressResolver().resolve(("cli["+std::to_string(nodeId)+"]").c_str()));
    }

    //    if (selfId == NULL_NODE_ID) {
    if (this->addrToId.size() == 0) {
        for (int nodeId = 0; nodeId < nodesNbr; nodeId++) {
            const char * address = ("cli["+to_string(nodeId)+"]").c_str();
            L3Address addr = L3AddressResolver().resolve(address);
            this->addrToId[addr] = nodeId;
        }
    }

    L3Address selfAddr = L3AddressResolver().addressOf(getParentModule(),L3AddressResolver().ADDR_IPv4);
    selfId = addrToId[selfAddr];

    roundEvent = new cMessage;
    scheduleAt(startTime, roundEvent);

    byzStringPar = &par("byzStringPar");
    string byzString = *byzStringPar;

    PAYLOAD_SIZE = par("msgSizePar");
    numMsgTotal = 0;
    numBitsTotal = 0;
    cumulatedProcessingTime = 0;

    outputPrefix = par("outputPrefixPar").stringValue();

    //    cout << "Read byzstring = " << byzString << endl;
    int i = 0;
    stringstream ssin(byzString);
    while (ssin.good() && i < f) {
        string tmp;
        ssin >> tmp;
        if (tmp.length()>0 and std::stoi(tmp) == selfId) {
            selfIsByzantine = true;
            //            cout << selfId << " is Byzantine" << endl;
        }
        i++;
    }
    //    for (int nodeId = 0; nodeId < nodesNbr; nodeId++) {
    //        const char * address = ("cli["+to_string(nodeId)+"]").c_str();
    //        if (selfAddr == L3AddressResolver().resolve(address)) {
    //            selfId = nodeId;
    //            //            cout << "Found selfId = " << nodeId << endl;
    //            break;
    //        }
    //    }
    //    }
    cout << "Initialized node " << selfId << endl;
    if (selfIsByzantine) {
        cout << "\t" << selfId << " is Byzantine" << endl;
    }

    if (selfId == 0) {
        int seed = par("seed");
        generator.seed(seed);
        initializeTopo();
        numFinish = 0;
        nTransmissionsPayload = 0;
        cumulatedProcessingTime = 0;
    }

    distribution = new normal_distribution<double>*[nodesNbr];
    for (int i = 0; i < nodesNbr; i++) {
        if (connections[selfId][i]) {
//            float mean = (float(rand())/float((RAND_MAX)) * 0); //10);
//            float stddev = (float(rand())/float((RAND_MAX)) * 0); //mean/10);
            //cout << "Chose mean=" << mean << ", and stddev = " << stddev << endl;
            if (ASYNC) {
                distribution[i] = new normal_distribution<double>(50, 20); //mean, stddev);
            } else {
                distribution[i] = new normal_distribution<double>(0, 0); //mean, stddev);
            }
        }
    }

    pendingMsgsPerNeighbors = new vector<pair<Packet *, SimTime>>[nodesNbr];
    processLinkTimers = new cMessage*[nodesNbr];
    for (int i = 0; i < nodesNbr; i++) {
        if (connections[selfId][i]) {
            processLinkTimers[i] = new cMessage(std::to_string(i).c_str());
            //std::cout << selfId << " ** creating channel timer (id=" << channelTimers[i]->getId() << ") for " << i << std::endl;
        }
    }

    rcvMsgFrom = new bool[nodesNbr];
    for (int i = 0; i < nodesNbr; i++) {
        rcvMsgFrom[i] = false;
    }
    sentAck = new bool[nodesNbr];
    for (int i = 0; i < nodesNbr; i++) {
        sentAck[i] = false;
    }

#ifdef BRACHA
    BRACHAinitialize();
#endif
#ifdef DOLEV
    DOLEVinitialize();
#endif
#ifdef BRACHADOLEV
    BRACHADOLEVinitialize();
#endif
#ifdef BRACHACPA
    BRACHACPAinitialize();
#endif
#ifdef BRACHADOLEV2
    BRACHADOLEV2initialize();
#endif
#ifdef V1
    V1initialize();
#endif
#ifdef V2
    V2initialize();
#endif
}

void Peer::handleStopOperation(LifecycleOperation *operation) {
    //    if (selfId==0)
    cancelEvent(roundEvent);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void Peer::handleCrashOperation(LifecycleOperation *operation) {
    //    if (selfId==0)
    cancelEvent(roundEvent);
    if (operation->getRootModule() != getContainingNode(this))     // closes socket when the application crashed only
        socket.destroy();    //TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
}

} // namespace inet

