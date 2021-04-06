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
#ifdef DOLEV

void Peer::DOLEVinitialize() {
    gotEmptyPathSetFrom = new bool[nodesNbr];
    for (int i = 0; i < nodesNbr; i++) {
        gotEmptyPathSetFrom[i] = false;
    }
    pathGraph = (bool ***) new bool*[nodesNbr]; // Depth
    for (int d = 0; d < nodesNbr; d++) {
        pathGraph[d] = new bool*[nodesNbr];
        for (int i = 0; i < nodesNbr; i++) {
            pathGraph[d][i] = new bool[nodesNbr];
            for (int j = 0; j < nodesNbr; j++) {
                pathGraph[d][i][j] = false;
            }
        }
    }
}

void Peer::DOLEVfinish() {
    delete[] gotEmptyPathSetFrom;
    for (int d = 0; d < nodesNbr; d++) {
        for (int i = 0; i < nodesNbr; i++) {
            delete pathGraph[d][i];
        }
        delete pathGraph[d];
    }
    delete[] pathGraph;
    cout << "Peer " << selfId << " sent " << this->numSentMsg << " msgs" << endl;
}

BriefPacket * Peer::DOLEVfirstMessage() {
    BriefPacket * bp = new BriefPacket();
    bp->setSenderId(selfId);
    bp->setBroadcasterId(selfId);
    bp->setMsgId(msgId);

    bp->setMsgType(ECHO);
    bp->setPathArraySize(0);
    for (int depth = 0; depth < nodesNbr; depth++) {
        pathGraph[depth][selfId][selfId] = true;
    }
    if (!delivered) {
        delivered = true;
        deliverCount++;
        deliverTime.push_back(simTime().dbl() * 1000- roundDuration*1000);
        cout << deliverCount << ": " << selfId << " [DOLEV] DELIVERS after " << simTime().dbl() * 1000 - roundDuration*1000 << "ms" <<  endl;
    }
    return bp;
}

void Peer::printGraph() {
    ofstream gfile;
    string gfname = "/home/jeremie/sim/BroadcastSign/simulations/graph_" + std::to_string(selfId) + ".txt";
    gfile.open(gfname);
    gfile << nodesNbr << endl << endl;
    for (int d = 0; d < nodesNbr; d++) {
        for (int i = 0; i < nodesNbr; i++) {
            for (int j = 0; j < nodesNbr; j++) {
                gfile << ((pathGraph[d][i][j])?1:0) << " ";
            }
            gfile << endl;
        }
        gfile << endl;
    }
    gfile.close();
}

void Peer::DOLEVreceiveMessage(BriefPacket *x) {
    if (delivered) { // Optim 5, Bonomi
        return;
    }

    for (int i = 0; i < x->getPathArraySize(); i++) {// Optim 4, Bonomi
        if (gotEmptyPathSetFrom[x->getPath(i)]) {
            return;
        }
    }

    if (x->getPathArraySize() == 0) { // Optim 2, 3 Bonomi
        gotEmptyPathSetFrom[x->getSenderId()] = true;
    }

    bool containsSelf = (x->getSenderId() == selfId); // Should not happen
    for (int i = 1; i < x->getPathArraySize(); i++) { // Important: otherwise the source never delivers
        if (x->getPath(i) == selfId) {
            containsSelf = true;
            break;
        }
    }
    vector<int> path;
    for (int i = 0; i < x->getPathArraySize(); i++) {
        path.push_back(x->getPath(i));
    }
    path.push_back(x->getSenderId());
    path.push_back(selfId);

    if (x->getSenderId() != x->getBroadcasterId() && x->getPathArraySize()==0) { // Optim 2 Bonomi
        path.insert(path.begin(), x->getBroadcasterId());
    }

    //    cout << "\t" << selfId << " rcv path = ";
    //    for (int i = 0; i < path.size(); i++) {
    //        cout << path[i] << " ";
    //    }
    //    cout << endl;

    int depth = 0;
    for (int i = 0; i < path.size()-1; i++) {
        int start = path[i];
        int next = path[i+1];
        pathGraph[depth][start][next] = true;
        depth++;
    }
    while (depth < nodesNbr) {
        pathGraph[depth][selfId][selfId] = true;
        depth++;
    }

    path.pop_back();

    printGraph();

    ofstream argfile;
    argfile.open("/home/jeremie/sim/BroadcastSign/simulations/args.txt");
    argfile << selfId << "\n";
    argfile << f << "\n";
    argfile.close();

    PyObject* pInt;
    Py_Initialize();

    FILE * PythonScriptFile = fopen("/home/jeremie/sim/BroadcastSign/simulations/disjointPathsToSource.py", "r");
    PyRun_SimpleFile(PythonScriptFile, "/home/jeremie/sim/BroadcastSign/simulations/disjointPathsToSource.py");
    fclose(PythonScriptFile);

    //    Py_Finalize(); // Should be executed only once!

    ifstream resfile ("/home/jeremie/sim/BroadcastSign/simulations/res.txt");
    int res;
    resfile >> res;
    //    cout << "\tres = " << res << endl;

    if (res == 1) {
        if (!delivered) {
            delivered = true;
            deliverCount++;
            deliverTime.push_back(simTime().dbl() * 1000- roundDuration*1000);
            cout << deliverCount << ": " << selfId << " [DOLEV] DELIVERS after " << simTime().dbl() * 1000 - roundDuration*1000 << "ms" <<  endl;
            path.clear(); // Optim 2 Bonomi
        }
    }
    resfile.close();


    if (containsSelf) return;

    vector<int> diff;
    if (!delivered) {
        set<int> pset;
        for (int i = 1; i < path.size(); i++) {
            pset.insert(path[i]);
        }
        for (int j = 0; j < nodesNbr; j++) {
            if (connections[selfId][j] == 1
                    && !gotEmptyPathSetFrom[j]
                                            && pset.find(j) == pset.end()) {
                diff.push_back(j);
            }
        }
    }
    else { // Optim 2 (Bonomi)
        for (int j = 0; j < nodesNbr; j++) {
            if (connections[selfId][j] == 1
                    && !gotEmptyPathSetFrom[j]) {
                diff.push_back(j);
            }
        }
    }

    if (diff.size() > 0) {
        BriefPacket * cp = new BriefPacket();
        cp->setSenderId(selfId);
        cp->setBroadcasterId(x->getBroadcasterId());
        cp->setMsgId(x->getMsgId());
        cp->setMsgType(ECHO);
        cp->setPathArraySize(path.size());
        for (int i = 0; i < path.size(); i++) {
            cp->setPath(i, path[i]);
        }
        //        cout << "\t" << selfId << " sending path = [";
        //        for (int i = 0; i < path.size(); i++) {
        //            cout << path[i] << " ";
        //        }
        //        cout << "]";
        //        std::string str(diff.begin(), diff.end());
        //        cout << "to [";
        //        for (int i : diff) cout << " " << i;
        //        cout << " ]";
        //        cout << endl;
        sendTo(cp, diff);
    }
}

#endif
}
