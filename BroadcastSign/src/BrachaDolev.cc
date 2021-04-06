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
#ifdef BRACHADOLEV

void Peer::BRACHADOLEVinitialize() {
    rcvMsgFrom = new bool[nodesNbr];
    for (int i = 0; i < nodesNbr; i++) {
        rcvMsgFrom[i] = false;
    }
}

void Peer::BRACHADOLEVfinish() {
    numMsgTotal += this->numSentMsg;
    cout << "Peer " << selfId << " sent " << this->numSentMsg << " msgs and " << this->numBytesSent << " B (msgId+payload = " << (msgSize+PROCESS_ID_SIZE+MSG_ID_SIZE) << " B) "
            << numBytesSent/(msgSize+PROCESS_ID_SIZE+MSG_ID_SIZE) << "X" <<  endl;
    if (selfId == nodesNbr-1) {
        cout << "Total number of messages: " << numMsgTotal << endl;
    }
    delete[] rcvMsgFrom;
}

BriefPacket * Peer::BRACHADOLEVfirstMessage() {
    BriefPacket * bp = new BriefPacket();
    bp->setMsgType(ECHO);
    bp->setBroadcasterId(selfId);
    bp->setMsgId(msgId);
    bp->setPathArraySize(0);

    bp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PATH_SIZE + msgSize));

    bp->setLinkSenderId(selfId);

    sentEcho = true;
    rcvECHOv0.insert(selfId);
    dolevDelivered[make_pair(ECHO, selfId)] = true;
    return bp;
}

void Peer::printGraphBrachaDolev(int msgType, int senderId) {
    ofstream gfile;
    string gfname;
    gfname = "/home/jeremie/sim/BroadcastSign/simulations/graph_"+to_string(msgType)+"_"+to_string(senderId)+"_"+to_string(selfId)+".txt";
    gfile.open(gfname);
    gfile << nodesNbr << endl << endl;
    for (int d = 0; d < nodesNbr; d++) {
        for (int i = 0; i < nodesNbr; i++) {
            for (int j = 0; j < nodesNbr; j++) {
                gfile << ((mapPathGraph[make_pair(msgType, senderId)][d][i][j])?1:0) << " ";
            }
            gfile << endl;
        }
        gfile << endl;
    }
    gfile.close();
}

void Peer::BRACHADOLEVreceiveMessage(BriefPacket *x) {
    if (dolevDelivered.find(make_pair(x->getMsgType(), x->getBroadcasterId())) == dolevDelivered.end()) {
        dolevDelivered[make_pair(x->getMsgType(), x->getBroadcasterId())] = false;
    }
    else if (dolevDelivered[make_pair(x->getMsgType(), x->getBroadcasterId())]) { // Optim 5, Bonomi
        return;
    }

    if (gotEmptyPathSetFrom.find(make_pair(x->getMsgType(), x->getBroadcasterId())) == gotEmptyPathSetFrom.end()) {
        gotEmptyPathSetFrom[make_pair(x->getMsgType(), x->getBroadcasterId())] = new bool[nodesNbr];
        for (int i = 0; i < nodesNbr; i++) {
            gotEmptyPathSetFrom[make_pair(x->getMsgType(), x->getBroadcasterId())][i] = false;
        }
    }
    else {
        for (int i = 0; i < x->getPathArraySize(); i++) {// Optim 4, Bonomi
            if (gotEmptyPathSetFrom[make_pair(x->getMsgType(), x->getBroadcasterId())][x->getPath(i)]) {
                return;
            }
        }
    }

    if (x->getPathArraySize() == 0) { // Optim 2, 3 Bonomi
        gotEmptyPathSetFrom[make_pair(x->getMsgType(), x->getBroadcasterId())][x->getLinkSenderId()] = true;
    }

    //    cout << "[BRACHADOLEV] " << selfId << " rcvd (" << ((x->getMsgType()==ECHO)?"ECHO":"READY") << ", " << x->getBroadcasterId() << ") from " << x->getLinkSenderId() << endl;
    bool containsSelf = (x->getLinkSenderId() == selfId); // Should not happen
    for (int i = 1; i < x->getPathArraySize(); i++) { // Important: otherwise the source never delivers
        if (x->getPath(i) == selfId) {
            containsSelf = true;
            break;
        }
    }

    vector<int> path;
    path.push_back(x->getBroadcasterId()); // getBroadcasterId is used like getEchoOrReady
    for (int i = 0; i < x->getPathArraySize(); i++) {
        path.push_back(x->getPath(i));
    }
    if (x->getBroadcasterId() != x->getLinkSenderId()) {
        path.push_back(x->getLinkSenderId());
    }
    path.push_back(selfId);

//    cout << selfId << " received " << ((x->getMsgType()==ECHO)? "(E=": "(R=") << x->getBroadcasterId() << ") from " << x->getLinkSenderId() << " with path = [";
//    for (int i = 0; i < x->getPathArraySize(); i++) {
//        cout << x->getPath(i) << " ";
//    }
//    cout << "]" << endl;
//    cout << "Assembled path = [";
//    for (int i : path) {
//        cout << i << " ";
//    }
//    cout << "]" << endl;

    if (mapPathGraph.find(make_pair(x->getMsgType(), x->getBroadcasterId())) == mapPathGraph.end()) {
        bool ***pathGraph = (bool ***) new bool*[nodesNbr]; // Depth
        for (int d = 0; d < nodesNbr; d++) {
            pathGraph[d] = new bool*[nodesNbr];
            for (int i = 0; i < nodesNbr; i++) {
                pathGraph[d][i] = new bool[nodesNbr];
                for (int j = 0; j < nodesNbr; j++) {
                    pathGraph[d][i][j] = false;
                }
            }
        }
        mapPathGraph[make_pair(x->getMsgType(), x->getBroadcasterId())] = pathGraph;
    }

    int depth = 0;
    for (int i = 0; i < path.size()-1; i++) {
        int start = path[i];
        int next = path[i+1];
        mapPathGraph[make_pair(x->getMsgType(), x->getBroadcasterId())][depth][start][next] = true;
        //        pathGraph[depth][start][next] = true;
        depth++;
    }
    while (depth < nodesNbr) {
        mapPathGraph[make_pair(x->getMsgType(), x->getBroadcasterId())][depth][selfId][selfId] = true;
        //        pathGraph[depth][selfId][selfId] = true;
        depth++;
    }

    printGraphBrachaDolev(x->getMsgType(), x->getBroadcasterId());

    ofstream argfile;
    argfile.open("/home/jeremie/sim/BroadcastSign/simulations/args.txt");
    argfile << selfId << "\n";
    argfile << f << "\n";
    argfile << x->getBroadcasterId() << "\n"; // ?Changed?
    argfile << x->getMsgType() << "\n";
    argfile.close();

    PyObject* pInt;
    Py_Initialize();

    FILE * PythonScriptFile = fopen("/home/jeremie/sim/BroadcastSign/simulations/disjointPaths.py", "r");
    PyRun_SimpleFile(PythonScriptFile, "/home/jeremie/sim/BroadcastSign/simulations/disjointPaths.py");
    fclose(PythonScriptFile);
    //    PyRun_SimpleString("print('Hello World from Embedded Python!!!')");

    //    Py_Finalize(); // Should be executed only once!

    ifstream resfile ("/home/jeremie/sim/BroadcastSign/simulations/res.txt");
    int res;
    resfile >> res;
    resfile.close();
    //    cout << "\tThe result is " << res << endl;

    path.erase(path.begin());
    path.pop_back(); // remove selfId

    if (not containsSelf) {
        vector<int> diff;
        if (!dolevDelivered[make_pair(x->getMsgType(), x->getBroadcasterId())]) {
            set<int> pset;
            for (int x : path) { // TODO: check in others they might have a problem too (had to change this loop)
                pset.insert(x);
            }
            for (int j = 0; j < nodesNbr; j++) {
                if (connections[selfId][j] == 1
                        && !gotEmptyPathSetFrom[make_pair(x->getMsgType(), x->getBroadcasterId())][j]
                        && pset.find(j) == pset.end()
                        && j != x->getBroadcasterId() && j != x->getLinkSenderId()) {
                    diff.push_back(j);
                }
            }
        } else {// Optim 2 (Bonomi)
            for (int j = 0; j < nodesNbr; j++) {
                if (connections[selfId][j] == 1
                        && !gotEmptyPathSetFrom[make_pair(x->getMsgType(), x->getBroadcasterId())][j]
                        && j != x->getBroadcasterId() && j != x->getLinkSenderId()) {
                    diff.push_back(j);
                }
            }
        }

        if (diff.size() > 0) {
            BriefPacket * cp = new BriefPacket();
            cp->setMsgType(x->getMsgType()); // changed
            cp->setBroadcasterId(x->getBroadcasterId());
            cp->setMsgId(x->getMsgId());
            cp->setLinkSenderId(selfId);
            cp->setPathArraySize(path.size());
            for (int i = 0; i < path.size(); i++) {
                cp->setPath(i, path[i]);
            }
            // The send message does not contain a senderId field
            cp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + ((x->getBroadcasterId() == x->getLinkSenderId())? 0: PROCESS_ID_SIZE) +
                    PATH_SIZE + PROCESS_ID_SIZE * cp->getPathArraySize() + msgSize));
            sendTo(cp, diff);
        }
    }

    if (res == 1) {
        dolevDelivered[make_pair(x->getMsgType(), x->getBroadcasterId())] = true; // Optim 2 Bonomi
        path.clear();

        if (x->getMsgType() == ECHO) {
            rcvECHOv0.insert(x->getBroadcasterId());
            rcvECHOv0.insert(selfId);
            //            cout << selfId << " validates ECHO from " << x->getBroadcasterId() <<" ("<<rcvECHOv0.size()<<")" << endl;
            if (rcvECHOv0.size() >= quorumSize) {
                rcvREADYv0.insert(selfId);
                if (rcvREADYv0.size() > 2*f) {
                    if (!delivered) {
                        delivered = true;
                        deliverCount++;
                        deliverTime.push_back(simTime().dbl() * 1000- roundDuration*1000);
                        cout << deliverCount << ": " << selfId << " [BRACHADOLEV] DELIVERS after " << simTime().dbl() * 1000 - roundDuration*1000 << "ms" <<  endl;
                    }
                }
            }
        } else if (x->getMsgType() == READY) {
            rcvREADYv0.insert(x->getBroadcasterId());
            if (rcvREADYv0.size() > f) {
                rcvREADYv0.insert(selfId);
            }
            //            cout << selfId << " validates READY from " << x->getBroadcasterId() <<" ("<<rcvREADYv0.size()<<")" << endl;
            if (rcvREADYv0.size() > 2*f) {
                if (!delivered) {
                    delivered = true;
                    deliverCount++;
                    deliverTime.push_back(simTime().dbl() * 1000- roundDuration*1000);
                    cout << deliverCount << ": " << selfId << " [BRACHADOLEV] DELIVERS after " << simTime().dbl() * 1000 - roundDuration*1000 << "ms" <<  endl;
                }
            }
        }

        bool isEcho = (x->getBroadcasterId() == x->getLinkSenderId() || rcvECHOv0.size() > f);
        bool isReady = rcvREADYv0.size() > f || rcvECHOv0.size() >= quorumSize;

        if (!sentReady && isReady) {
            cout << selfId << " is now ready!" << endl;
            sentReady = true;
            sentEcho = true;
            BriefPacket * cp = new BriefPacket();
            cp->setMsgType(READY);
            cp->setBroadcasterId(selfId); // changed
            cp->setMsgId(x->getMsgId());
            cp->setLinkSenderId(selfId);
            cp->setPathArraySize(0);
            vector<int> neighbors;
            for (int j = 0; j < nodesNbr; j++) {
                if (connections[selfId][j] == 1) {
                    neighbors.push_back(j);
                }
            }

            cp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PATH_SIZE + PROCESS_ID_SIZE * cp->getPathArraySize() + msgSize));
            sendTo(cp, neighbors);
        } else if (!sentEcho && isEcho) {
            sentEcho = true;
            BriefPacket * cp = new BriefPacket();
            cp->setMsgType(ECHO);
            cp->setBroadcasterId(selfId); // changed
            cp->setMsgId(x->getMsgId());
            cp->setLinkSenderId(selfId);
            cp->setPathArraySize(0);
            vector<int> neighbors;
            for (int j = 0; j < nodesNbr; j++) {
                if (connections[selfId][j] == 1) {
                    neighbors.push_back(j);
                }
            }

            cp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PATH_SIZE + PROCESS_ID_SIZE * cp->getPathArraySize() + msgSize));
            sendTo(cp, neighbors);
        }
    }
}


#endif
}
