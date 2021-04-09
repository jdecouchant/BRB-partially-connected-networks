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

    int V = nodesNbr*(nodesNbr+1);
    rgraph = new int*[V];
    for (int i = 0; i < V; i++) {
        rgraph[i] = new int[V];
    }
}

void Peer::BRACHADOLEVfinish() {
    numMsgTotal += this->numSentMsg;
    cout << "Peer " << selfId << " sent " << this->numSentMsg << " msgs and " << this->numBitsSent << " B (msgId+payload = " << (PAYLOAD_SIZE+PROCESS_ID_SIZE+PAYLOAD_ID_SIZE) << " B) "
            << numBitsSent/(PAYLOAD_SIZE+PROCESS_ID_SIZE+MSG_ID_SIZE) << "X" <<  endl;
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

    bp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PATH_LENGTH_SIZE + PAYLOAD_SIZE));

    bp->setLinkSenderId(selfId);

    sentEcho = true;
    rcvECHOv0.insert(selfId);
    dolevDelivered[make_pair(ECHO, selfId)] = true;
    return bp;
}

/* Returns true if there is a path from source 's' to sink 't' in
  residual graph. Also fills parent[] to store the path */
bool Peer::bfs(int V, int **rGraph, int s, int t, int * parent) {

    // Create a visited array and mark all vertices as not visited
    bool visited[V];
    for(int i=0;i<V;++i) {
        visited[i] = false;
    }

    // Create a queue, enqueue source vertex and mark source vertex as visited
    queue <int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    while (!q.empty()) {     // Standard BFS Loop
        int u = q.front();
        q.pop();

        for (int d = 0; d < nodesNbr; d++) {
            for (int i = 0; i < nodesNbr; i++) {
                int v = nodesNbr*d+i;
                if (visited[v]==false && rGraph[u][v] > 0) {
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;
                }

            }
        }
    }

    return visited[t];    // If we reached sink in BFS starting from source, then return true, else false
}

bool Peer::fordFulkerson(int debug, int msgType, int s, int ot) {

    // If message directly received from its original broadcaster
    if (s == ot || mapPathGraph[make_pair(msgType, s)][0][s][ot]) { // TODO: s == ot should not be necessary
        return true;
    }

    int V = (nodesNbr+1)*nodesNbr;

    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            rgraph[i][j] = 0;
        }
    }

    for (int d = 0; d < nodesNbr; d++) {

        int t = nodesNbr * d + ot;

        // Copy graph into local matrix
        for (int d = 0; d < nodesNbr; d++) {
            for (int i = 0; i < nodesNbr; i++) {
                for (int j = 0; j < nodesNbr; j++) {
                    rgraph[d*nodesNbr + i][(d+1)*nodesNbr + j] = ((mapPathGraph[make_pair(msgType, s)][d][i][j])?1:0);
                }
            }
        }

        int parent[V];
        int max_flow = 0;

        // Augment the flow while there is path from source to sink
        int iter = 0;
        while (bfs(V, rgraph, s, t, parent)) {
            // Find minimum residual capacity of the edges along the
            // path filled by BFS. Or we can say find the maximum flow
            // through the path found.
            int path_flow = INT_MAX;
            for (int v=t; v!=s; v=parent[v]) {
                int u = parent[v];
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

        // Return the overall flow
        if (max_flow >= f+1) {
            return true;
        }
    }

    return false;
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
        depth++;
    }
    while (depth < nodesNbr) {
        mapPathGraph[make_pair(x->getMsgType(), x->getBroadcasterId())][depth][selfId][selfId] = true;
        depth++;
    }

    bool msgIsValidated = fordFulkerson(0, x->getMsgType(), x->getBroadcasterId(), selfId);

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
                    PATH_LENGTH_SIZE + PROCESS_ID_SIZE * cp->getPathArraySize() + PAYLOAD_SIZE));
            sendTo(cp, diff);
        }
    }

    if (msgIsValidated) {
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
                        deliverTime.push_back(simTime().dbl() * 1000- startTime*1000);
                        cout << deliverCount << ": " << selfId << " [BRACHADOLEV] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
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
                    deliverTime.push_back(simTime().dbl() * 1000- startTime*1000);
                    cout << deliverCount << ": " << selfId << " [BRACHADOLEV] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
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

            cp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PATH_LENGTH_SIZE + PROCESS_ID_SIZE * cp->getPathArraySize() + PAYLOAD_SIZE));
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

            cp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PATH_LENGTH_SIZE + PROCESS_ID_SIZE * cp->getPathArraySize() + PAYLOAD_SIZE));
            sendTo(cp, neighbors);
        }
    }
}


#endif
}
