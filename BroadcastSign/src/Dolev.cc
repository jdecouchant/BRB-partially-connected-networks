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
#ifdef DOLEV

void Peer::DOLEVinitialize() {
    gotEmptyPathSetFrom = new bool[nodesNbr];
    for (int i = 0; i < nodesNbr; i++) {
        gotEmptyPathSetFrom[i] = false;
    }
    mapPathGraph = (bool ***) new bool*[nodesNbr]; // Depth
    for (int d = 0; d < nodesNbr; d++) {
        mapPathGraph[d] = new bool*[nodesNbr];
        for (int i = 0; i < nodesNbr; i++) {
            mapPathGraph[d][i] = new bool[nodesNbr];
            for (int j = 0; j < nodesNbr; j++) {
                mapPathGraph[d][i][j] = false;
            }
        }
    }

    int V = nodesNbr*(nodesNbr+1);
    rgraph = new int*[V];
    for (int i = 0; i < V; i++) {
        rgraph[i] = new int[V];
    }
}

void Peer::DOLEVfinish() {
    delete[] gotEmptyPathSetFrom;
    for (int d = 0; d < nodesNbr; d++) {
        for (int i = 0; i < nodesNbr; i++) {
            delete mapPathGraph[d][i];
        }
        delete mapPathGraph[d];
    }
    delete[] mapPathGraph;
    cout << "Peer " << selfId << " sent " << this->numSentMsg << " msgs" << endl;
}

BriefPacket * Peer::DOLEVfirstMessage() {
    BriefPacket * bp = new BriefPacket();
    bp->setLinkSenderId(selfId);
    bp->setBroadcasterId(selfId);
    bp->setMsgId(msgId);

    bp->setMsgType(ECHO);
    bp->setPathArraySize(0);
    for (int depth = 0; depth < nodesNbr; depth++) {
        mapPathGraph[depth][selfId][selfId] = true;
    }
    if (!delivered) {
        delivered = true;
        deliverCount++;
        deliverTime.push_back(simTime().dbl() * 1000- startTime*1000);
        cout << deliverCount << ": " << selfId << " [DOLEV] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
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
                gfile << ((mapPathGraph[d][i][j])?1:0) << " ";
            }
            gfile << endl;
        }
        gfile << endl;
    }
    gfile.close();
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

    return visited[t];    // If we reached sink in BFS starting from source, then return true, else false
}

bool Peer::fordFulkerson(int debug, int s, int ot) {

    // If message directly received from its original broadcaster
    if (s == ot || mapPathGraph[0][s][ot]) { // TODO: s == ot should not be necessary
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
                    rgraph[d*nodesNbr + i][(d+1)*nodesNbr + j] = ((mapPathGraph[d][i][j])?1:0);
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

        // Return the overall flow
        if (max_flow >= f+1) {
            return true;
        }
    }

    return false;
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
        gotEmptyPathSetFrom[x->getLinkSenderId()] = true;
    }

    bool containsSelf = (x->getLinkSenderId() == selfId); // Should not happen
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
    path.push_back(x->getLinkSenderId());
    path.push_back(selfId);

    if (x->getLinkSenderId() != x->getBroadcasterId() && x->getPathArraySize()==0) { // Optim 2 Bonomi
        path.insert(path.begin(), x->getBroadcasterId());
    }

    int depth = 0;
    for (int i = 0; i < path.size()-1; i++) {
        int start = path[i];
        int next = path[i+1];
        mapPathGraph[depth][start][next] = true;
        depth++;
    }
    while (depth < nodesNbr) {
        mapPathGraph[depth][selfId][selfId] = true;
        depth++;
    }

    path.pop_back();

    bool msgIsValidated = fordFulkerson(0, x->getBroadcasterId(), selfId);
    if (msgIsValidated) {
        if (!delivered) {
            delivered = true;
            deliverCount++;
            deliverTime.push_back(simTime().dbl() * 1000- startTime*1000);
            cout << deliverCount << ": " << selfId << " [DOLEV] DELIVERS after " << simTime().dbl() * 1000 - startTime*1000 << "ms" <<  endl;
            path.clear(); // Optim 2 Bonomi
        }
    }

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
        cp->setLinkSenderId(selfId);
        cp->setBroadcasterId(x->getBroadcasterId());
        cp->setMsgId(x->getMsgId());
        cp->setMsgType(ECHO);
        cp->setPathArraySize(path.size());
        for (int i = 0; i < path.size(); i++) {
            cp->setPath(i, path[i]);
        }
        sendTo(cp, diff);
    }
}

#endif
}
