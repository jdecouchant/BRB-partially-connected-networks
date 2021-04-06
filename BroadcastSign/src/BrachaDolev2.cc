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
#ifdef BRACHADOLEV2

void Peer::BRACHADOLEV2initialize() {
    OPTIM_SENDTOECHO = par("OPTIM_SENDTOECHO");
    OPTIM_ECHOTOECHO = par("OPTIM_ECHOTOECHO");
    OPTIM_ECHOTOREADY = par("OPTIM_ECHOTOREADY");
    OPTIM_CUTECHOREADY_AFTER_DELIVER = par("OPTIM_CUTECHOREADY_AFTER_DELIVER");
    OPTIM_CUT_ECHO_AFTER_READY_RECEIVED = par("OPTIM_CUT_ECHO_AFTER_READY_RECEIVED");
    OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED = par("OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED");
    OPTIM_CUT_ECHO_TO_READY_NEIGHBORS = par("OPTIM_CUT_ECHO_TO_READY_NEIGHBORS");
    OPTIM_BRACHA_OVERPROVISIONING = par("OPTIM_BRACHA_OVERPROVISIONING");
    OPTIM_BITSET = par("OPTIM_BITSET");
    OPTIM_MESSAGE_FIELDS = par("OPTIM_MESSAGE_FIELDS");
    OPTIM_PAYLOAD_AVOID_IF_RECEIVED = par("OPTIM_PAYLOAD_AVOID_IF_RECEIVED");
    OPTIM_SEND_2f1 = par("OPTIM_SEND_2f1");
    ASYNC = par("ASYNC");
}

void Peer::BRACHADOLEV2finish() {
    numMsgTotal += this->numSentMsg;
    numBitsTotal += this->numBitsSent;
    cout << "Peer " << selfId << " sent " << this->numSentMsg << " msgs and " << this->numBitsSent << " bits (msgId+payload = "
            << (PAYLOAD_SIZE+PROCESS_ID_SIZE+MSG_ID_SIZE) << " B) "
            << numBitsSent/(PAYLOAD_SIZE+PROCESS_ID_SIZE+MSG_ID_SIZE) << "X" <<  endl;
    if (selfId == nodesNbr-1) {
        SimTime avgDeliveryTime = 0;
        for (auto dtime : deliverTime) {
//            cout << dtime << " ";
            avgDeliveryTime += dtime;
        }
        avgDeliveryTime /= nodesNbr;
        cout << endl;
        cout << "Number of nodes who delivered: " << deliverTime.size() <<", i.e., "<< ((double) deliverTime.size() * 100) / ((double) nodesNbr) << "%" << endl;
        cout << "Average delivery time: " << avgDeliveryTime << "ms" << endl;
        cout << "# payload transmissions: " << ((double) nTransmissionsPayload) / ((double) nodesNbr) << endl;
        cout << "Total # messages: " << numMsgTotal << endl;
        cout << "Averages: " << (float) numMsgTotal / (float) nodesNbr <<" msgs, and "<< (float) numBitsTotal / (float) nodesNbr << " bits" << endl;
        cout << "Cumulated proc. time: " << (float) cumulatedProcessingTime / (float) nodesNbr << " us" << endl;

    }
    delete[] rcvMsgFrom;

    int V = (nodesNbr+1)*nodesNbr;
    for (int i = 0; i < V; i++) {
        delete[] rgraph[i];
    }
    delete[] rgraph;
}

BriefPacket * Peer::createBriefPacket(int msgType) {
    BriefPacket * bp = new BriefPacket();
    bp->setMsgType(msgType);
    bp->setLinkSenderId(selfId);
    return bp;
}

BriefPacket * Peer::createBriefPacket(int msgType, int broadcasterId, int msgId, int echoOrReadySender, vector<int> path) {
    BriefPacket * bp = new BriefPacket();
    bp->setMsgType(msgType);
    bp->setBroadcasterId(broadcasterId);
    bp->setMsgId(msgId);

    bp->setLinkSenderId(selfId);
    bp->setEchoOrReadySender(echoOrReadySender);
    bp->setPathArraySize(path.size());
    for (int i = 0; i < path.size(); i++) {
        bp->setPath(i, path[i]);
    }

    return bp;
}

BriefPacket * Peer::createBriefPacket(int msgType, int broadcasterId, int msgId, int echoOrReadySender, vector<int> path, int ackedEchoForREADY_ECHO) {
    BriefPacket * bp = new BriefPacket();
    bp->setMsgType(msgType);
    bp->setBroadcasterId(broadcasterId);
    bp->setMsgId(msgId);

    bp->setLinkSenderId(selfId);
    bp->setEchoOrReadySender(echoOrReadySender);
    bp->setPathArraySize(path.size());
    for (int i = 0; i < path.size(); i++) {
        bp->setPath(i, path[i]);
    }
    bp->setAckedEchoForREADY_ECHO(ackedEchoForREADY_ECHO);
    return bp;
}

BriefPacket * Peer::createBriefPacket(BriefPacket * x) {
    BriefPacket * bp = new BriefPacket();
    bp->setMsgType(x->getMsgType());
    bp->setBroadcasterId(x->getBroadcasterId());
    bp->setMsgId(x->getMsgId());

    bp->setLinkSenderId(x->getLinkSenderId());
    bp->setEchoOrReadySender(x->getEchoOrReadySender());
    bp->setPathArraySize(x->getPathArraySize());
    for (int i = 0; i < x->getPathArraySize(); i++) {
        bp->setPath(i, x->getPath(i));
    }
    bp->setAckedEchoForREADY_ECHO(x->getAckedEchoForREADY_ECHO());
    return bp;
}

BriefPacket * Peer::BRACHADOLEV2firstMessage() {
    vector<int> emptyPath;
    BriefPacket * bp = createBriefPacket(ECHO, selfId, msgId, selfId, emptyPath);

    bp->setIncludeData(true); // contains payload + id for payload

    sentEcho = true;
    rcvECHO.insert(selfId);
    dolevDelivered[make_pair(ECHO, selfId)] = true;
    rcvPayload = true;
    checkIfEmptyPathReceivedFromOneProcessInPath(make_pair(ECHO, selfId), emptyPath);
    return bp;
}

void Peer::printGraphBRACHADOLEV2(int msgType, int senderId) {
    ofstream gfile;
    string gfname;
    gfname = outputPrefix +'_'+ to_string(msgType)+"_"+to_string(senderId)+"_"+to_string(selfId)+ ".graph";
//    gfname = "/home/jeremie/sim/BroadcastSign/simulations/graph_"+to_string(msgType)+"_"+to_string(senderId)+"_"+to_string(selfId)+".txt";
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

vector<int> Peer::getAllNeighborsExceptDelivered() {
    vector<int> neighbors;
    for (int j = 0; j < nodesNbr; j++) {
        if (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED) {
            if (connections[selfId][j] == 1
                    && neighborsThatDelivered.find(j) == neighborsThatDelivered.end()) {
                neighbors.push_back(j);
            }
        } else {
            if (connections[selfId][j] == 1) {
                neighbors.push_back(j);
            }
        }
    }
    return neighbors;
}

vector<int> Peer::getAllNeighborsExceptDeliveredAnd(int nodeId) {
    vector<int> neighbors;
    for (int j = 0; j < nodesNbr; j++) {
        if (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED) {
            if (connections[selfId][j] == 1
                    && neighborsThatDelivered.find(j) == neighborsThatDelivered.end()
                    && j != nodeId) {
                neighbors.push_back(j);
            }
        } else {
            if (connections[selfId][j] == 1 && j != nodeId) {
                neighbors.push_back(j);
            }
        }
    }
    return neighbors;
}

void Peer::createGraphIfDoesNotExist(pair<int, int> xId) {
    if (mapPathGraph.find(xId) == mapPathGraph.end()) {
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
        mapPathGraph[xId] = pathGraph;
    }
}

void Peer::insertPathIntoGraph(pair<int, int> xId, vector<int> path) {
    // Insert the assembled path into the associated graph
    int depth = 0;
    for (int i = 0; i < path.size()-1; i++) {
        int start = path[i];
        int next = path[i+1];
        mapPathGraph[xId][depth][start][next] = true;
        depth++;
    }
    while (depth < nodesNbr) {
        mapPathGraph[xId][depth][selfId][selfId] = true;
        depth++;
    }
//    path.pop_back();
}

bool Peer::checkIfEmptyPathReceivedFromOneProcessInPath(pair<int, int> xId, vector<int> path) {
    if (gotEmptyPathSetFrom.find(xId) != gotEmptyPathSetFrom.end()) {
        for (int i = 0; i < path.size(); i++) {// Optim 4, Bonomi
            if (gotEmptyPathSetFrom[xId][path[i]]) {
                return true;
            }
        }
    } else {
        gotEmptyPathSetFrom[xId] = new bool[nodesNbr];
        for (int i = 0; i < nodesNbr; i++) {
            gotEmptyPathSetFrom[xId][i] = false;
        }
    }
    return false;
}

bool Peer::checkIfDolevDelivered(pair<int, int> xId) {
    if (dolevDelivered.find(xId) != dolevDelivered.end()) { // Optim 5, Bonomi
        if (dolevDelivered[xId]) {
            return true;
        }
    } else {
        dolevDelivered[xId] = false;
    }
    return false;
}

/* Returns true if there is a path from source 's' to sink 't' in
  residual graph. Also fills parent[] to store the path */
bool Peer::bfs(int V, int **rGraph, int s, int t, int * parent) {
//    cout << "bfs with arguments " << "s=" << s << ", t=" << t << endl;
//    for (int i=0; i<V; i++) {
//        for (int j=0; j<V; j++) {
//            cout << rGraph[i][j] << " ";
//        }
//        cout << endl;
//    }

    // Create a visited array and mark all vertices as not visited
    bool visited[V] = {false};

    // Create a queue, enqueue source vertex and mark source vertex as visited
    queue <int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;

//    cout << "before depiling" << endl;
    while (!q.empty()) {     // Standard BFS Loop
        int u = q.front();
        q.pop();

        for (int d = 0; d < nodesNbr; d++) {
            //            cout << "depth = " << d << endl;
            for (int i = 0; i < nodesNbr; i++) {
                int v = nodesNbr*d+i;
                if (visited[v]==false && rGraph[u][v] > 0) {
                    //                cout << u << " " << v << " " << rGraph[u][v] << " Found neighbor " << v << endl; sleep(1);
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;
                }

            }
        }
    }
//    for (int v=0; v<V; v++) {
//        if (visited[v]==false && rGraph[u][v] > 0) {
//            //                cout << u << " " << v << " " << rGraph[u][v] << " Found neighbor " << v << endl; sleep(1);
//            q.push(v);
//            parent[v] = u;
//            visited[v] = true;
//        }
//    }
//    }

//    cout << "before returning value " << visited[t] << endl;

    return visited[t];    // If we reached sink in BFS starting from source, then return true, else false
}

bool Peer::fordFulkerson(int debug, int msgType, int s, int ot) {
//    cout << "ff source = " << s << ", dest = " << ot << endl;

    // If message directly received from its original broadcaster
    if (s == ot || mapPathGraph[make_pair(msgType, s)][0][s][ot]) { // TODO: s == ot should not be necessary
        return true;
    }

    int V = (nodesNbr+1)*nodesNbr;

    for (int i = 0; i < V; i++) { // TODO: should not be necessary
        for (int j = 0; j < V; j++) {
            rgraph[i][j] = 0;
        }
    }

    for (int d = 0; d < nodesNbr; d++) {

        int t = nodesNbr * d + ot;
//        for (int i = 0; i < V; i++) {
//            for (int j = 0; j < V; j++) {
//                rgraph[i][j] = 0;
//            }
//        }

        // Copy graph into local matrix
        for (int d = 0; d < nodesNbr; d++) {
            for (int i = 0; i < nodesNbr; i++) {
                for (int j = 0; j < nodesNbr; j++) {
                    rgraph[d*nodesNbr + i][(d+1)*nodesNbr + j] = ((mapPathGraph[make_pair(msgType, s)][d][i][j])?1:0);
                    //                cout << d*nodesNbr+i << " " << (d+1)*nodesNbr+j << " " << rgraph[d*nodesNbr + i][(d+1)*nodesNbr + j] << " " << endl;
//                    cout << rgraph[d*nodesNbr + i][(d+1)*nodesNbr + j] << " ";
                }
//                cout << endl;
            }
//            cout << endl;
        }

        int parent[V];
        int max_flow = 0;

        // Augment the flow while there is path from source to sink
        int iter = 0;
        while (bfs(V, rgraph, s, t, parent)) {
//            cout << debug << ": loop " << iter++ << endl;
            // Find minimum residual capacity of the edges along the
            // path filled by BFS. Or we can say find the maximum flow
            // through the path found.
            int path_flow = INT_MAX;
            for (int v=t; v!=s; v=parent[v]) {
                int u = parent[v];
                //            cout << u << " " << v << " updating path flow with " << rgraph[u][v] << endl;
                path_flow = min(path_flow, rgraph[u][v]);
            }

            // update residual capacities of the edges and reverse edges
            // along the path
            int u;
            for (int v=t; v != s; v=parent[v]) {
                u = parent[v];
                rgraph[u][v] -= path_flow;
                rgraph[v][u] += path_flow;
            }

            // Add path flow to overall flow
            max_flow += path_flow;
        }

        //    cout << "free rgraph" << endl;

//        cout << "max flow = " << max_flow << endl;
        // Return the overall flow
        if (max_flow >= f+1) {
//            for (int i = 0; i < V; i++) {
//                delete[] rgraph[i];
//            }
//            delete[] rgraph;
//            cout << "f+1 paths between " << s << " and " << t << endl;
            return true;
        }
    }

//    for (int i = 0; i < V; i++) {
//        delete[] rgraph[i];
//    }
//    delete[] rgraph;
    return false;
}

bool Peer::fPlus1DisjointPaths(int source, int msgType) {
    return true;
    //// Search for f+1 disjoint paths between the echoOrReadySender and the local process using a Python script
//    ofstream argfile;
////    argfile.open("/home/jeremie/sim/BroadcastSign/simulations/args.txt");
//    argfile.open(outputPrefix + ".args");
//    argfile << selfId << "\n";
//    argfile << f << "\n";
//    argfile << source << "\n"; // ?Changed?
//    argfile << msgType << "\n";
//    argfile.close();
//
//    PyObject* pInt;
//    Py_Initialize();
//
//    //string cmd = "/home/jeremie/sim/BroadcastSign/simulations/stats/"+to_string(nodesNbr)+"_"+to_string(k)+"_"+to_string(f)+".py";
//    string cmd = outputPrefix + ".py";
//    //cout << "name of the disjoint path python script " << cmd << endl;
//    FILE * PythonScriptFile = fopen(cmd.c_str(), "r");
//    PyRun_SimpleFile(PythonScriptFile, cmd.c_str());
//    fclose(PythonScriptFile);
//    //    PyRun_SimpleString("print('Hello World from Embedded Python!!!')");
//
//    //    Py_Finalize(); // Should be executed only once!
//
////    ifstream resfile ("/home/jeremie/sim/BroadcastSign/simulations/res.txt");
//    //cout << "after the python script has been called" << endl;
//    ifstream resfile (outputPrefix + ".res");
//    int res;
//    resfile >> res;
//    resfile.close();
//    return (res == 1);
}

vector<int> Peer::getPathFromMessage(BriefPacket *x) {

    vector<int> path;
    path.push_back(x->getEchoOrReadySender()); // getBroadcasterId is used like getEchoOrReady
    for (int i = 0; i < x->getPathArraySize(); i++) {
        path.push_back(x->getPath(i));
    }
    if (x->getEchoOrReadySender() != x->getLinkSenderId()) {
        path.push_back(x->getLinkSenderId());
    }
    path.push_back(selfId);

//    cout << selfId << " received " << ((x->getMsgType()==ECHO)? "(E=": "(R=") << x->getEchoOrReadySender() << ") from " << x->getLinkSenderId() << " with path = [";
//    for (int i = 0; i < x->getPathArraySize(); i++) {
//        cout << x->getPath(i) << " ";
//    }
//    cout << "]" << endl;
//    cout << "Assembled path = [";
//    for (int i : path) {
//        cout << i << " ";
//    }
//    cout << "]" << endl;

    return path;
}

vector<int> Peer::getVectorFromInt(int id) {
    vector<int> vec;
    vec.push_back(id);
    return vec;
}

bool Peer::checkIfSubpathReceived(pair<int, int> xId, vector<int> path) {
    if (receivedPaths.find(xId) != receivedPaths.end()) {
        bitset<BITSET_MAX_SIZE> candidateBitset;
        for (int pid : path) candidateBitset.set(pid);
        for (bitset<BITSET_MAX_SIZE> knownBitset : receivedPaths[xId]) {
            bool isSubset = true;
            for (int i = 0; i < BITSET_MAX_SIZE; i++) {
                if (knownBitset.test(i) && !candidateBitset.test(i)) {
                    isSubset = false;
                    break;
                }
            }
            if (isSubset) {
//                if (xId.first == ECHO) {
//                    cout << "ECHO: ";
//                } else if (xId.first == READY) {
//                    cout << "READY: ";
//                } else if (xId.first == READY_ECHO) {
//                    cout << "READY_ECHO: ";
//                }
//                cout << " Received : " << pathbitset.to_string() << endl;
//                cout << " Had previously : " << b.to_string() << endl;
                return true;
            }
        }
    }
    return false;
}

void Peer::insertPath(pair<int, int> xId, vector<int> path) {

    bitset<BITSET_MAX_SIZE> pathbitset;
    for (int x : path) pathbitset.set(x);

    if (receivedPaths.find(xId) == receivedPaths.end()) {
        vector<bitset<BITSET_MAX_SIZE>> v;
        v.push_back(pathbitset);
        receivedPaths[xId] = v;
    } else {
        receivedPaths[xId].push_back(pathbitset);
    }
}

void Peer::BRACHADOLEV2receiveMessage(BriefPacket *x) {

    if (selfIsByzantine) return;

//    cout << "Receive message of type" << x->getMsgType() << " sent from " << x->getLinkSenderId() << endl;

    if (OPTIM_PAYLOAD_AVOID_IF_RECEIVED) {
        if (x->getIncludeData()) {
            rcvPayload = true;
            while (pendingMsgs.size() > 0) {
//                cout << selfId << " depiling" << endl;
                BriefPacket * cur = pendingMsgs.front();
                pendingMsgs.erase(pendingMsgs.begin());
                BRACHADOLEV2receiveMessage(cur);
                delete cur;
            }
        } else if (!x->getIncludeData() && !rcvPayload) {
//            cout << selfId << " piling" << endl;
            pendingMsgs.push_back(createBriefPacket(x));
        }

        rcvMsgFrom[x->getLinkSenderId()] = true;
//        if (!sentAck[x->getLinkSenderId()]) {
//            BriefPacket * bp = createBriefPacket(PAYLOAD_ACK);
//            sendTo(bp, getVectorFromInt(x->getLinkSenderId()));
//            sentAck[x->getLinkSenderId()] = true;
//        }
    }

    if (x->getMsgType() == ECHO) {
        BRACHADOLEV2receiveMessage_ECHO(x);
    } else if (x->getMsgType() == READY) {
        BRACHADOLEV2receiveMessage_READY(x);
    } else if (x->getMsgType() == READY_ECHO) {
        BRACHADOLEV2receiveMessage_READY_ECHO(x);
    } else if (x->getMsgType() == ECHO_ECHO) {
        BRACHADOLEV2receiveMessage_ECHO_ECHO(x);
    } else if (x->getMsgType() == PAYLOAD_ACK) {
        rcvMsgFrom[x->getLinkSenderId()] = true;
    } else {
        cout<<"BRACHADOLEV2receiveMessage: unsupported msg type "<< x->getMsgType()<<endl;
    }
//    cout << "About to return from receiveMessage" << endl;
}

void Peer::BRACHADOLEV2receiveMessage_ECHO(BriefPacket *x) {
    vector<int> emptyPath;

    int echoSender = x->getEchoOrReadySender();
    pair<int, int> xId = make_pair(ECHO, echoSender);
    vector<int> path = getPathFromMessage(x);     // Assemble received path into a vector

    // Study the embedded path
//    cout<<selfId<<" rcv ECHO "<<x->getEchoOrReadySender()<<" from "<<x->getLinkSenderId()<<endl;
    bool sourceEchoIsValidated = false;
    pair<int, int> xId0 = make_pair(ECHO, x->getBroadcasterId());
    if (OPTIM_SENDTOECHO && selfId != x->getBroadcasterId() &&
            !(x->getLinkSenderId() == x->getBroadcasterId() && x->getEchoOrReadySender() == x->getBroadcasterId())
            && !dolevDelivered[xId0]) {
//        cout << "Study embedded" << endl;
        path.insert(path.begin(), x->getBroadcasterId());

        bool studyMsg = !checkIfDolevDelivered(xId0);
        studyMsg = studyMsg && !checkIfEmptyPathReceivedFromOneProcessInPath(xId0, path);
        if (studyMsg) {
            createGraphIfDoesNotExist(xId0);
            insertPathIntoGraph(xId0, path);

            if (OPTIM_BITSET) {
                if (!checkIfSubpathReceived(xId0, path)) {
                    insertPath(xId0, path);
                } else {
                    studyMsg = false;
                }
            }
            if (studyMsg) {
//                printGraphBRACHADOLEV2(ECHO, x->getBroadcasterId());
//                sourceEchoIsValidated = fPlus1DisjointPaths(x->getBroadcasterId(), ECHO);
//                cout << "bf verif 0" << endl;
                sourceEchoIsValidated = fordFulkerson(0, ECHO, x->getBroadcasterId(), selfId);
//                cout << "VERIFICATION 0 = " << sourceEchoIsValidated << ", " << newSourceEchoIsValidated << endl;
//                sleep(2);

                if (sourceEchoIsValidated) {
//                    cout<<selfId<<" validates ECHO "<<x->getBroadcasterId()<<" ["<<simTime()<<"]"<<endl;
                    dolevDelivered[xId0] = true;
                    rcvECHO.insert(x->getBroadcasterId());
                    rcvECHO.insert(selfId);
                }
            }
        }
        path.erase(path.begin());
    }

    if (checkIfDolevDelivered(xId)) return;     // Check if the rcvd msg has already been delivered, if so ignore it // Optim 5, Bonomi
    if (checkIfEmptyPathReceivedFromOneProcessInPath(xId, path)) return;  // Check if an empty path was previously received for this message

    if (x->getPathArraySize() == 0) { // Receive empty path from neighbor, store this info // Optim 2, 3 Bonomi
        gotEmptyPathSetFrom[xId][x->getLinkSenderId()] = true;
    }

    createGraphIfDoesNotExist(xId);    // Create graph associated to the received msg if it doesn't exist
    insertPathIntoGraph(xId, path);    // Insert received path in the graph

    path.erase(path.begin());
    path.pop_back(); // remove selfId from path

    if (OPTIM_BITSET) {
        if (!checkIfSubpathReceived(xId, path)) {
            insertPath(xId, path);
        } else {
            return; // skip message
        }
    }

//    printGraphBRACHADOLEV2(ECHO, echoSender);     // Write the graph to file
//    bool msgIsValidated = fPlus1DisjointPaths(echoSender, ECHO); //(x->getEchoOrReadySender(), x->getMsgType());

//    cout << "bf verif 1" << endl;
    bool msgIsValidated = fordFulkerson(1, ECHO, echoSender, selfId);
//    if (msgIsValidated != newSourceEchoIsValidated) {
//        cout << "VERIFICATION 1 = " << msgIsValidated << ", " << newSourceEchoIsValidated << endl;
//        sleep(2);
//    }

    bool doCreateREADY_ECHO = false;
    bool doCreateECHO_ECHO = false;
    if (msgIsValidated) { // Received message has just been Dolev-delivered
//        cout<<selfId<<" validates ECHO "<<x->getEchoOrRefadySender()<<" ["<<simTime()<<"]"<<endl;
        rcvMsgFrom[x->getEchoOrReadySender()] = true;

        dolevDelivered[xId] = true; // Optim 2 Bonomi
        path.clear();

        rcvECHO.insert(echoSender);
        if (x->getBroadcasterId() == x->getLinkSenderId() || rcvECHO.size() >= f+1) {
//            if (!isInEcho) {
//                isInEcho = true;
//                doCreateECHO_ECHO = true;
//            }
            rcvECHO.insert(selfId);
        }

        if (rcvECHO.size() >= quorumSize) {
//            if (!isInReady) {
//                isInReady = true;
//                doCreateREADY_ECHO = true;
//            }
            rcvREADY.insert(selfId);
        }

        if (!delivered && rcvREADY.size() >= 2*f+1) {
            delivered = true;
            deliverCount++;
            deliverTime.push_back(simTime().dbl() * 1000 - startTime*1000);
            cout << deliverCount << ": " << selfId << " [BRACHADOLEV2] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
        }
    }

    /////////////////////////////////////////////
    // Dolev forwarding of the received message
    /////////////////////////////////////////////
    bool isInEcho = rcvECHO.find(x->getBroadcasterId())!=rcvECHO.end() || rcvECHO.size() >= f+1; // echo can be sent if directly received from sender, or if f+1 echos have been received
    bool isInReady = rcvREADY.size() >= f+1 || rcvECHO.size() >= quorumSize;
    if (!sentReady && isInReady) {
        if (OPTIM_BRACHA_OVERPROVISIONING && selfId > 2*f) {
            doCreateREADY_ECHO = false;
        } else {
            doCreateREADY_ECHO = OPTIM_ECHOTOREADY;
        }
    }
    if (!sentEcho && isInEcho) {
        if (OPTIM_BRACHA_OVERPROVISIONING && selfId > quorumSize + f - 1
                || (OPTIM_SENDTOECHO && x->getEchoOrReadySender() == x->getBroadcasterId() && x->getLinkSenderId() == x->getBroadcasterId())) {
            doCreateECHO_ECHO = false;
        } else {
            doCreateECHO_ECHO = OPTIM_ECHOTOECHO;
        }
    }

//    doCreateECHO_ECHO = false; // TODO: to remove to activate ECHO_ECHO messages
    if (!doCreateREADY_ECHO && !doCreateECHO_ECHO) {
//        cout << "\t" << selfId << " forwards the received ECHO message" << endl;
        bool isSendMsg = OPTIM_SENDTOECHO && x->getEchoOrReadySender() == x->getBroadcasterId() && x->getLinkSenderId() == x->getBroadcasterId(); // New_optim 1: do not relay SEND msgs, x->getMsgType() == ECHO &&
        //    bool selfShouldNotSend = OPTIM_BRACHA_OVERPROVISIONING && selfId > quorumSize + f - 1;  // Those not sending their own echo have to participate.
        bool isEchoToReady = OPTIM_ECHOTOREADY && doCreateREADY_ECHO;
        bool doNotRelayAfterDeliver = OPTIM_CUTECHOREADY_AFTER_DELIVER && (delivered && x->getMsgType() == ECHO); // Optim async: do not send ECHOs after you have delivered
        bool echoAfterReady = OPTIM_CUT_ECHO_AFTER_READY_RECEIVED && (x->getMsgType()==ECHO && rcvREADY.find(x->getEchoOrReadySender()) != rcvREADY.end());

        if (!isSendMsg && !isEchoToReady && !doNotRelayAfterDeliver && !echoAfterReady) { //!selfShouldNotSend &&
            vector<int> notToSelect = (dolevDelivered[xId])? emptyPath: path; // At this point, path contains the received path + linkSendingId
            vector<int> destinations;
            for (int j = 0; j < nodesNbr; j++) {         // Select the neighbors that will receive the message
                if (!connections[selfId][j]) continue;

                if( !(gotEmptyPathSetFrom[xId][j]) // && gotEmptyPathSetFrom[xId0][j])
                        && find(notToSelect.begin(), notToSelect.end(), j) == notToSelect.end()) {
                    if ( (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED && neighborsThatDelivered.find(j) != neighborsThatDelivered.end())
                            || (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && x->getMsgType()==ECHO && rcvREADY.find(j) != rcvREADY.end()) )
                        continue;
                    destinations.push_back(j);
                }
            }
            // DOLEV forwarding
            BriefPacket * bp = createBriefPacket(x->getMsgType(), x->getBroadcasterId(), x->getMsgId(), x->getEchoOrReadySender(), (dolevDelivered[xId])? emptyPath: path);
            sendTo(bp, destinations);
        }
    }

    /////////////////////////////////////////////
    // Generate new message
    /////////////////////////////////////////////
    if (!sentReady && isInReady) {
        cout << selfId << " is now ready! [" << simTime() << "]" << endl;
        sentReady = true;
        sentEcho = true;
        if (!OPTIM_BRACHA_OVERPROVISIONING || selfId <= 2*f) {
            if (doCreateREADY_ECHO) { // Optimisation: two messages in one
                BriefPacket * bp = createBriefPacket(READY_ECHO, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath, echoSender);
                sendTo(bp, getAllNeighborsExceptDeliveredAnd(echoSender)); // all neighbors that did not deliver and that are not the echo sender

                if (connections[selfId][echoSender]) { // If sender of echo is a neighbor, send her a READY and not a READY_ECHO
                    BriefPacket * bp2 = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
                    sendTo(bp2, getVectorFromInt(echoSender));
                }
            } else {
                BriefPacket * bp = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
                sendTo(bp, getAllNeighborsExceptDelivered());
            }
        } else {
            doCreateREADY_ECHO = false;
        }
    } else if (!sentEcho && isInEcho) {
//        if (x->getLinkSenderId() != x->getBroadcasterId()) {
//            cout << selfId << need for ECHO_ECHO" " << endl;
//        }
        sentEcho = true;
        if (!OPTIM_BRACHA_OVERPROVISIONING || selfId <= quorumSize + f - 1) {
            if (doCreateECHO_ECHO) {
                cout << "\t" << selfId << " creates ECHO_ECHO message" << endl;
                BriefPacket * bp = createBriefPacket(ECHO_ECHO, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath, echoSender);
                sendTo(bp, getAllNeighborsExceptDeliveredAnd(echoSender)); // all neighbors that did not deliver and that are not the echo sender

                if (connections[selfId][echoSender]) { // If sender of echo is a neighbor, send her a READY and not a READY_ECHO
                    BriefPacket * bp2 = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
                    sendTo(bp2, getVectorFromInt(echoSender));
                }
            } else {
                BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
                vector<int> diff;
                for (int j : getAllNeighborsExceptDelivered()) {
                    if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && rcvREADY.find(j) != rcvREADY.end())
                        continue;
                    diff.push_back(j);
                }
                sendTo(bp, diff);
            }
        }
    }
}

void Peer::BRACHADOLEV2receiveMessage_READY(BriefPacket *x) {
    vector<int> emptyPath;

    int linkSender = x->getLinkSenderId();
    int readySender = x->getEchoOrReadySender();

    pair<int, int> xId = make_pair(READY, readySender);
    vector<int> path = getPathFromMessage(x);     // Assemble received path into a vector

    if (OPTIM_BITSET) {
        if (!checkIfSubpathReceived(xId, path)) {
            insertPath(xId, path);
        } else {
            return; // skip message
        }
    }

    if (checkIfDolevDelivered(xId)) return;     // Check if the rcvd msg has already been delivered, if so ignore it // Optim 5, Bonomi
    if (checkIfEmptyPathReceivedFromOneProcessInPath(xId, path)) return;  // Check if an empty path was previously received for this message

    if (x->getPathArraySize() == 0) { // Receive empty path from neighbor, store this info // Optim 2, 3 Bonomi
        gotEmptyPathSetFrom[xId][linkSender] = true;

        if (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED || OPTIM_CUT_ECHO_TO_READY_NEIGHBORS) { // New optim
            if (neighborsToReadys.find(linkSender) == neighborsToReadys.end()) {
                neighborsToReadys[linkSender] = new set<int>();
            }
            neighborsToReadys[linkSender]->insert(readySender);
            if (neighborsToReadys[linkSender]->size() >= 2*f+1) {
                neighborsThatDelivered.insert(linkSender);
            }
        }
    }

    createGraphIfDoesNotExist(xId);    // Create graph associated to the received msg if it doesn't exist
    insertPathIntoGraph(xId, path);    // Insert received path in the graph

    path.erase(path.begin());
    path.pop_back(); // remove selfId from path

//    printGraphBRACHADOLEV2(READY, readySender);     // Write the graph to file
//    bool msgIsValidated = fPlus1DisjointPaths(readySender, READY); //x->getEchoOrReadySender(), x->getMsgType());
    bool msgIsValidated = fordFulkerson(2, READY, readySender, selfId);

    if (msgIsValidated) { // Received message has just been Dolev-delivered
        rcvMsgFrom[x->getEchoOrReadySender()] = true;

        dolevDelivered[xId] = true; // Optim 2 Bonomi
        path.clear();

        if (x->getMsgType() == READY) {
            rcvREADY.insert(readySender);
            if (rcvREADY.size() >= f+1) {
                isInReady = true;
                rcvREADY.insert(selfId);
            }
        }

        if (!delivered && rcvREADY.size() >= 2*f+1) {
            delivered = true;
            deliverCount++;
            deliverTime.push_back(simTime().dbl() * 1000 - startTime*1000);
            cout << deliverCount << ": " << selfId << " [BRACHADOLEV2] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
        }
    }

    /////////////////////////////////////////////
    // Dolev forwarding of the received message
    /////////////////////////////////////////////
    vector<int> notToSelect = (dolevDelivered[xId])? emptyPath: path; // At this point, path contains the received path + linkSendingId
    vector<int> destinations;
    for (int j = 0; j < nodesNbr; j++) {         // Select the neighbors that will receive the message
        if (connections[selfId][j] == 1 && !gotEmptyPathSetFrom[xId][j] && find(notToSelect.begin(), notToSelect.end(), j) == notToSelect.end()
                && j != x->getEchoOrReadySender() && j != x->getLinkSenderId()) {
            if (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED && neighborsThatDelivered.find(j) != neighborsThatDelivered.end())
                continue;
            if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && x->getMsgType()==ECHO && rcvREADY.find(j) != rcvREADY.end())
                continue;
            destinations.push_back(j);
        }
    }
    // DOLEV forwarding
    BriefPacket * bp = createBriefPacket(x->getMsgType(), x->getBroadcasterId(), x->getMsgId(), x->getEchoOrReadySender(), (dolevDelivered[xId])? emptyPath: path);
    sendTo(bp, destinations);

    /////////////////////////////////////////////
    // Generate new message
    /////////////////////////////////////////////
    bool isInEcho = x->getBroadcasterId() == linkSender || rcvECHO.size() >= f+1; // echo can be sent if directly received from sender, or if f+1 echos have been received
    bool isInReady = rcvREADY.size() >= f+1 || rcvECHO.size() >= quorumSize;

    if (!sentReady && isInReady) {
        cout << selfId << " is now ready!" << endl;
        sentReady = true;
        sentEcho = true;

        cout << "!!! Following the reception of a READY message (need for READY READY messages)!!!!" << endl;

        if (!OPTIM_BRACHA_OVERPROVISIONING || selfId <= 2*f) {
            BriefPacket * bp = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
            sendTo(bp, getAllNeighborsExceptDelivered());
        }

    } else if (!sentEcho && isInEcho) {
        sentEcho = true;

        if (!OPTIM_BRACHA_OVERPROVISIONING || selfId <= quorumSize + f - 1) {
            BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
            vector<int> diff;
            for (int j : getAllNeighborsExceptDelivered()) {
                if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && rcvREADY.find(j) != rcvREADY.end()) {
                    continue;
                }
                diff.push_back(j);
            }
            sendTo(bp, diff);
        }
    }
}

void Peer::BRACHADOLEV2receiveMessage_ECHO_ECHO(BriefPacket *x) {
//    cout << selfId << " receives an ECHO_ECHO message" << endl;

    vector<int> emptyPath;
    int linkSender = x->getLinkSenderId();

    int echoSender1 = x->getEchoOrReadySender();
    int echoSender2 = x->getAckedEchoForREADY_ECHO();

    pair<int, int> echoId1 = make_pair(ECHO, echoSender1);
    pair<int, int> echoId2 = make_pair(ECHO, echoSender2);

    vector<int> epath1 = getPathFromMessage(x);     // Assemble received path into a vector
    vector<int> epath2;
    epath2.push_back(echoSender2);
    for (int i : epath2) {
        epath2.push_back(i);
    }

    // Study the embedded source path
    bool sourceEchoIsValidated = false;
    pair<int, int> xId0 = make_pair(ECHO, x->getBroadcasterId());
    if (OPTIM_SENDTOECHO && selfId != x->getBroadcasterId() &&
            !(x->getLinkSenderId() == x->getBroadcasterId() && x->getEchoOrReadySender() == x->getBroadcasterId())
            && !dolevDelivered[xId0]) {
        epath2.insert(epath2.begin(), x->getBroadcasterId());

        bool studyMsg = !checkIfDolevDelivered(xId0);
        studyMsg = studyMsg && !checkIfEmptyPathReceivedFromOneProcessInPath(xId0, epath2);
        if (studyMsg) {
            createGraphIfDoesNotExist(xId0);
            insertPathIntoGraph(xId0, epath2);

            if (OPTIM_BITSET) {
                if (!checkIfSubpathReceived(xId0, epath2)) {
                    insertPath(xId0, epath2);
                } else {
                    studyMsg = false;
                }
            }
            if (studyMsg) {
//                printGraphBRACHADOLEV2(ECHO, x->getBroadcasterId());
//                sourceEchoIsValidated = fPlus1DisjointPaths(x->getBroadcasterId(), ECHO);
                sourceEchoIsValidated = fordFulkerson(3, ECHO, x->getBroadcasterId(), selfId);
                if (sourceEchoIsValidated) {
                    dolevDelivered[xId0] = true;
                    rcvECHO.insert(x->getBroadcasterId());
                    rcvECHO.insert(selfId);
                }
            }
        }
        epath2.erase(epath2.begin());
    }


    bool forwardEcho1 = !checkIfDolevDelivered(echoId1) && !checkIfEmptyPathReceivedFromOneProcessInPath(echoId1, epath1);
    bool forwardEcho2 = !checkIfDolevDelivered(echoId2) && !checkIfEmptyPathReceivedFromOneProcessInPath(echoId2, epath2);

    if (OPTIM_BITSET) {
        if (!checkIfSubpathReceived(echoId1, epath1)) {
            insertPath(echoId1, epath1);
        } else {
            forwardEcho1 = false;
        }
        if (!checkIfSubpathReceived(echoId2, epath2)) {
            insertPath(echoId2, epath2);
        } else {
            forwardEcho2 = false;
        }
    }

    if (!forwardEcho1 && !forwardEcho2) return;

    if (x->getPathArraySize() == 0) { // Receive empty path from neighbor, store this info // Optim 2, 3 Bonomi
        gotEmptyPathSetFrom[echoId1][linkSender] = true;
        gotEmptyPathSetFrom[echoId2][linkSender] = true;
    }

    createGraphIfDoesNotExist(echoId1);    // Create graph associated to the received msg if it doesn't exist
    insertPathIntoGraph(echoId1, epath1);    // Insert received path in the graph

    createGraphIfDoesNotExist(echoId2);
    insertPathIntoGraph(echoId2, epath2);

    epath1.erase(epath1.begin());
    epath1.pop_back(); // remove selfId from path

    epath2.erase(epath2.begin());
    epath2.pop_back(); // remove selfId from path

//    printGraphBRACHADOLEV2(ECHO, echoSender1);     // Write the graph to file
//    bool echo1IsValidated = fPlus1DisjointPaths(echoSender1, ECHO);
    bool echo1IsValidated = fordFulkerson(4, ECHO, echoSender1, selfId);

//    printGraphBRACHADOLEV2(ECHO, echoSender2);
//    bool echo2IsValidated = fPlus1DisjointPaths(echoSender2, ECHO);
    bool echo2IsValidated = fordFulkerson(5, ECHO, echoSender2, selfId);

    // TODO: continue from here

    if (echo1IsValidated) { // Received message has just been Dolev-delivered
        rcvMsgFrom[echoSender1] = true;
        dolevDelivered[echoId1] = true; // Optim 2 Bonomi

        rcvECHO.insert(echoSender1);
        if (x->getBroadcasterId() == linkSender || rcvECHO.size() >= f+1) {
            rcvECHO.insert(selfId);
        }
        if (rcvECHO.size() >= quorumSize) {
            if (!isInReady) {
                isInReady = true;
            }
            rcvREADY.insert(selfId);
        }
    }

    if (echo2IsValidated) { // Received message has just been Dolev-delivered
        rcvMsgFrom[echoSender2] = true;
        dolevDelivered[echoId2] = true; // Optim 2 Bonomi

        rcvECHO.insert(echoSender2);
        if (x->getBroadcasterId() == linkSender || rcvECHO.size() >= f+1) {
            rcvECHO.insert(selfId);
        }
        if (rcvECHO.size() >= quorumSize) {
            if (!isInReady) {
                isInReady = true;
            }
            rcvREADY.insert(selfId);
        }
    }

    if (!delivered && rcvREADY.size() >= 2*f+1) {
        delivered = true;
        deliverCount++;
        deliverTime.push_back(simTime().dbl() * 1000 - startTime*1000);
        cout << deliverCount << ": " << selfId << " [BRACHADOLEV2] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
    }


    bool isInEcho = x->getBroadcasterId() == linkSender || rcvECHO.size() >= f+1; // echo can be sent if directly received from sender, or if f+1 echos have been received
    bool isInReady = rcvREADY.size() >= f+1 || rcvECHO.size() >= quorumSize;

    /////////////////////////////////////////////
    // Dolev forwarding of the received READY_ECHO message
    /////////////////////////////////////////////
    //    cout << "Before dolev forwarding" << endl;
    vector<int> notToSelectEcho1 = (dolevDelivered[echoId1])? emptyPath: epath1; // At this point, path contains the received path + linkSendingId
    vector<int> notToSelectEcho2 = (dolevDelivered[echoId2])? emptyPath: epath2;

    vector<int> echo1Destinations;
    for (int j = 0; j < nodesNbr; j++) {         // Select the neighbors that will receive the message
        if (connections[selfId][j] == 1 && !gotEmptyPathSetFrom[echoId1][j]
            && find(notToSelectEcho1.begin(), notToSelectEcho1.end(), j) == notToSelectEcho1.end()) {
//            && j != x->getAckedEchoForREADY_ECHO() && j != x->getLinkSenderId()) {
            if (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED && neighborsThatDelivered.find(j) != neighborsThatDelivered.end())
                continue;
            if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && rcvREADY.find(j) != rcvREADY.end())
                continue;
            echo1Destinations.push_back(j);
        }
    }

    vector<int> echo2Destinations;
    for (int j = 0; j < nodesNbr; j++) {         // Select the neighbors that will receive the message
        if (connections[selfId][j] == 1 && !gotEmptyPathSetFrom[echoId2][j]
            && find(notToSelectEcho2.begin(), notToSelectEcho2.end(), j) == notToSelectEcho2.end()) {
//            && j != x->getAckedEchoForREADY_ECHO() && j != x->getLinkSenderId()) {
            if (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED && neighborsThatDelivered.find(j) != neighborsThatDelivered.end())
                continue;
            if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && rcvREADY.find(j) != rcvREADY.end())
                continue;
            echo2Destinations.push_back(j);
        }
    }

    sort(echo1Destinations.begin(), echo1Destinations.end());
    sort(echo2Destinations.begin(), echo2Destinations.end());

    vector<int> echo1AndEcho2Destinations;
    set_intersection(echo1Destinations.begin(), echo1Destinations.end(),
                     echo2Destinations.begin(), echo2Destinations.end(),
                     back_inserter(echo1AndEcho2Destinations));

    vector<int> echo1OnlyDestinations;
    set_difference(echo1Destinations.begin(), echo1Destinations.end(),
                     echo2Destinations.begin(), echo2Destinations.end(),
                     back_inserter(echo1OnlyDestinations));

    vector<int> echo2OnlyDestinations;
    set_difference(echo2Destinations.begin(), echo2Destinations.end(),
                   echo1Destinations.begin(), echo1Destinations.end(),
                   back_inserter(echo2OnlyDestinations));

    if (dolevDelivered[echoId1]) epath1 = emptyPath;
    if (dolevDelivered[echoId2]) epath2 = emptyPath;

    if (forwardEcho1 && forwardEcho2) {
        if (epath1 == epath2) {
            BriefPacket * bp = createBriefPacket(ECHO_ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender1, epath1, echoSender2);
            sendTo(bp, echo1AndEcho2Destinations);
        } else {
            BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender1, epath1);
            sendTo(bp, echo1AndEcho2Destinations);

            bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender2, epath2);
            sendTo(bp, echo1AndEcho2Destinations);
        }

        BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender1, epath1);
        sendTo(bp, echo1OnlyDestinations);

        bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender2, epath2);
        sendTo(bp, echo2OnlyDestinations);

    } else if (forwardEcho1 && !forwardEcho2) {
        BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender1, epath1);
        sendTo(bp, echo1Destinations);

    } else if (!forwardEcho1 && forwardEcho2) {
        BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender2, epath2);
        sendTo(bp, echo2Destinations);
    }

    if (!sentReady && isInReady) {
        cout << selfId << " is now ready!" << endl;
        sentReady = true;
        sentEcho = true;

        if (!OPTIM_BRACHA_OVERPROVISIONING || selfId <= 3*f) {
            cout << "!!! (2) Following the reception of a ECHO_ECHO message (need for READY_ECHO_ECHO messages)!!!!" << endl;
            cout << "Echo 1 was validated: " << echo1IsValidated << ", sender = " << echoSender1 << ", forward = " << forwardEcho1 << endl;
            cout << "Echo 2 was validated: " << echo2IsValidated << ", sender = " << echoSender2 << ", forward = " << forwardEcho2 << endl;
            BriefPacket * bp = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
            sendTo(bp, getAllNeighborsExceptDelivered());
        }
    } else if (!sentEcho && isInEcho) {
        cout << "!!! (2) Following the reception of a ECHO_ECHO message (need for ECHO_ECHO_ECHO messages)!!!!" << endl;
        cout << "Echo 1 was validated: " << echo1IsValidated << ", sender = " << echoSender1 << ", forward = " << forwardEcho1 << endl;
        cout << "Echo 2 was validated: " << echo2IsValidated << ", sender = " << echoSender2 << ", forward = " << forwardEcho2 << endl;
        sentEcho = true;
        BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
        vector<int> diff;
        for (int j : getAllNeighborsExceptDelivered()) {
            if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && rcvREADY.find(j) != rcvREADY.end())
                continue;
            diff.push_back(j);
        }
        sendTo(bp, diff);
    }
}

void Peer::BRACHADOLEV2receiveMessage_READY_ECHO(BriefPacket *x) {
    vector<int> emptyPath;
    int linkSender = x->getLinkSenderId();

    int readySender = x->getEchoOrReadySender();
    int echoSender = x->getAckedEchoForREADY_ECHO();

    pair<int, int> readyId = make_pair(READY, readySender);
    pair<int, int> echoId = make_pair(ECHO, echoSender);

    vector<int> rpath = getPathFromMessage(x);     // Assemble received path into a vector
    vector<int> epath;
    epath.push_back(readySender);
    for (int i : rpath) {
        epath.push_back(i);
    }

    bool forwardReady = !checkIfDolevDelivered(readyId) && !checkIfEmptyPathReceivedFromOneProcessInPath(readyId, rpath);
    bool forwardEcho = !checkIfDolevDelivered(echoId) && !checkIfEmptyPathReceivedFromOneProcessInPath(echoId, epath);

    if (OPTIM_BITSET) {
        if (!checkIfSubpathReceived(echoId, epath)) {
            insertPath(echoId, epath);
        } else {
            forwardEcho = false;
        }
        if (!checkIfSubpathReceived(readyId, rpath)) {
            insertPath(readyId, rpath);
        } else {
            forwardReady = false;
        }
    }

    if (!forwardReady && !forwardEcho) return;

    if (x->getPathArraySize() == 0) { // Receive empty path from neighbor, store this info // Optim 2, 3 Bonomi
        gotEmptyPathSetFrom[readyId][linkSender] = true;
        gotEmptyPathSetFrom[echoId][linkSender] = true;

        if (neighborsToReadys.find(linkSender) == neighborsToReadys.end()) {
            neighborsToReadys[linkSender] = new set<int>();
        }
        neighborsToReadys[linkSender]->insert(readySender);
        if (neighborsToReadys[linkSender]->size() >= 2*f+1) {
            neighborsThatDelivered.insert(linkSender);
        }
    }

    createGraphIfDoesNotExist(readyId);    // Create graph associated to the received msg if it doesn't exist
    insertPathIntoGraph(readyId, rpath);    // Insert received path in the graph

    createGraphIfDoesNotExist(echoId);
    insertPathIntoGraph(echoId, epath);

    rpath.erase(rpath.begin());
    rpath.pop_back(); // remove selfId from path

    epath.erase(epath.begin());
    epath.pop_back(); // remove selfId from path

//    printGraphBRACHADOLEV2(READY, readySender);     // Write the graph to file
//    bool readyIsValidated = fPlus1DisjointPaths(readySender, READY);
    bool readyIsValidated = fordFulkerson(6, READY, readySender, selfId);

//    printGraphBRACHADOLEV2(ECHO, echoSender);
//    bool echoIsValidated = fPlus1DisjointPaths(echoSender, ECHO);
    bool echoIsValidated = fordFulkerson(7, ECHO, echoSender, selfId);

    /////////////////////////////////////////////
    // Dolev forwarding of the received READY_ECHO message
    /////////////////////////////////////////////
//    cout << "Before dolev forwarding" << endl;
    vector<int> notToSelectEcho = (dolevDelivered[echoId])? emptyPath: epath; // At this point, path contains the received path + linkSendingId
    vector<int> notToSelectReady = (dolevDelivered[readyId])? emptyPath: rpath;

    vector<int> echoDestinations;
    for (int j = 0; j < nodesNbr; j++) {         // Select the neighbors that will receive the message
        if (connections[selfId][j] == 1 && !gotEmptyPathSetFrom[echoId][j]
                && find(notToSelectEcho.begin(), notToSelectEcho.end(), j) == notToSelectEcho.end()
                && j != x->getAckedEchoForREADY_ECHO() && j != x->getLinkSenderId()) {
            if (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED && neighborsThatDelivered.find(j) != neighborsThatDelivered.end())
                continue;
            if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && rcvREADY.find(j) != rcvREADY.end())
                continue;
            echoDestinations.push_back(j);
        }
    }

    vector<int> readyDestinations;
    for (int j = 0; j < nodesNbr; j++) {         // Select the neighbors that will receive the message
        if (connections[selfId][j] == 1 && !gotEmptyPathSetFrom[readyId][j]
                 && find(notToSelectReady.begin(), notToSelectReady.end(), j) == notToSelectReady.end()
                 && j != x->getEchoOrReadySender() && j != x->getLinkSenderId()) {
            if (OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED && neighborsThatDelivered.find(j) != neighborsThatDelivered.end())
                continue;
            readyDestinations.push_back(j);
        }
    }

    sort(echoDestinations.begin(), echoDestinations.end());
    sort(readyDestinations.begin(), readyDestinations.end());

    vector<int> echoAndReadyDestinations;
    set_intersection(echoDestinations.begin(), echoDestinations.end(),
                     readyDestinations.begin(), readyDestinations.end(),
                     back_inserter(echoAndReadyDestinations));

    vector<int> echoOnlyDestinations;
    set_difference(echoDestinations.begin(), echoDestinations.end(),
                     readyDestinations.begin(), readyDestinations.end(),
                     back_inserter(echoOnlyDestinations));

    vector<int> readyOnlyDestinations;
    set_difference(readyDestinations.begin(), readyDestinations.end(),
                   echoDestinations.begin(), echoDestinations.end(),
                   back_inserter(readyOnlyDestinations));

    if (dolevDelivered[echoId]) epath = emptyPath;
    if (dolevDelivered[readyId]) rpath = emptyPath;

    if (forwardEcho && forwardReady) {
        if (epath == rpath) {
            BriefPacket * bp = createBriefPacket(READY_ECHO, x->getBroadcasterId(), x->getMsgId(), readySender, rpath, echoSender);
            sendTo(bp, echoAndReadyDestinations);
        } else {
            BriefPacket * bp = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), readySender, rpath);
            sendTo(bp, echoAndReadyDestinations);

            bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender, epath);
            sendTo(bp, echoAndReadyDestinations);
        }

        BriefPacket * bp = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), readySender, rpath);
        sendTo(bp, readyOnlyDestinations);

        bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender, epath);
        sendTo(bp, echoOnlyDestinations);

    } else if (forwardEcho && !forwardReady) {
        BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), echoSender, epath);
        sendTo(bp, echoDestinations);

    } else if (!forwardEcho && forwardReady) {
        BriefPacket * bp = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), readySender, rpath);
        sendTo(bp, readyDestinations);
    }

    // Generating new message

    if (readyIsValidated) { // Received message has just been Dolev-delivered
        rcvMsgFrom[x->getEchoOrReadySender()] = true;
         dolevDelivered[readyId] = true; // Optim 2 Bonomi

         rcvREADY.insert(readySender);
         if (rcvREADY.size() >= f+1) {
             isInReady = true;
             rcvREADY.insert(selfId);
         }

         if (!delivered && rcvREADY.size() >= 2*f+1) {
             delivered = true;
             deliverCount++;
             deliverTime.push_back(simTime().dbl() * 1000 - startTime*1000);
             cout << deliverCount << ": " << selfId << " [BRACHADOLEV2] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
         }

         bool isInEcho = x->getBroadcasterId() == linkSender || rcvECHO.size() >= f+1; // echo can be sent if directly received from sender, or if f+1 echos have been received
         bool isInReady = rcvREADY.size() >= f+1 || rcvECHO.size() >= quorumSize;

         if (!sentReady && isInReady) {
             cout << selfId << " is now ready!" << endl;
             sentReady = true;
             sentEcho = true;

             cout << "!!! (1) Following the reception of a READY_ECHO message (need for READY_READY_READY messages)!!!!" << endl;
             cout << "Ready was validated" << endl;
             cout << "Echo to forward: " << forwardEcho << endl;

             if (!OPTIM_BRACHA_OVERPROVISIONING || selfId <= 3*f) {
                 BriefPacket * bp = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
                 sendTo(bp, getAllNeighborsExceptDelivered());
             }

         } else if (!sentEcho && isInEcho) {
             sentEcho = true;

             BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
             vector<int> diff;
             for (int j : getAllNeighborsExceptDelivered()) {
                 if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && rcvREADY.find(j) != rcvREADY.end()) {
                     continue;
                 }
                 diff.push_back(j);
             }
             sendTo(bp, diff);
         }
     }

    if (echoIsValidated) { // Received message has just been Dolev-delivered
        rcvMsgFrom[x->getAckedEchoForREADY_ECHO()] = true;
        dolevDelivered[echoId] = true; // Optim 2 Bonomi

        rcvECHO.insert(echoSender);
        if (x->getBroadcasterId() == linkSender || rcvECHO.size() >= f+1) {
            rcvECHO.insert(selfId);
        }
        if (rcvECHO.size() >= quorumSize) {
            if (!isInReady) {
                isInReady = true;
            }
            rcvREADY.insert(selfId);
        }

        if (!delivered && rcvREADY.size() >= 2*f+1) {
            delivered = true;
            deliverCount++;
            deliverTime.push_back(simTime().dbl() * 1000 - startTime*1000);
            cout << deliverCount << ": " << selfId << " [BRACHADOLEV2] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
        }

        bool isInEcho = x->getBroadcasterId() == linkSender || rcvECHO.size() >= f+1; // echo can be sent if directly received from sender, or if f+1 echos have been received
        bool isInReady = rcvREADY.size() >= f+1 || rcvECHO.size() >= quorumSize;

        if (!sentReady && isInReady) {
            cout << selfId << " is now ready!" << endl;
            sentReady = true;
            sentEcho = true;

            if (!OPTIM_BRACHA_OVERPROVISIONING || selfId <= 3*f) {
                cout << "!!! (2) Following the reception of a READY_ECHO message (need for READY_READY_ECHO messages)!!!!" << endl;

                BriefPacket * bp = createBriefPacket(READY, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
                sendTo(bp, getAllNeighborsExceptDelivered());
            }

        } else if (!sentEcho && isInEcho) {
            sentEcho = true;

            BriefPacket * bp = createBriefPacket(ECHO, x->getBroadcasterId(), x->getMsgId(), selfId, emptyPath);
            vector<int> diff;
            for (int j : getAllNeighborsExceptDelivered()) {
                if (OPTIM_CUT_ECHO_TO_READY_NEIGHBORS && rcvREADY.find(j) != rcvREADY.end()) {
                    continue;
                }
                diff.push_back(j);
            }
            sendTo(bp, diff);
        }
    }
}


#endif
}
