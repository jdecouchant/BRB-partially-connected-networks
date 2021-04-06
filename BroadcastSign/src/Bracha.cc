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
#ifdef BRACHA

void Peer::BRACHAinitialize() {
}

void Peer::BRACHAfinish() {
    cout << "Peer " << selfId << " sent " << this->numSentMsg << " msgs" << endl;
}

BriefPacket * Peer::BRACHAfirstMessage() {
    BriefPacket * bp = new BriefPacket();
    bp->setSenderId(selfId);
    bp->setBroadcasterId(selfId);
    bp->setMsgId(msgId);
    bp->setMsgType(ECHO);
    return bp;
}

void Peer::BRACHAreceiveMessage(BriefPacket *x) {

    if (rcvECHO.find(x->getBroadcasterId()) == rcvECHO.end()) {
        set<int> s;
        rcvECHO[x->getBroadcasterId()] = s;
    }
    if (rcvREADY.find(x->getBroadcasterId()) == rcvREADY.end()) {
        set<int> s;
        rcvREADY[x->getBroadcasterId()] = s;
    }

    if (x->getMsgType() == ECHO) {
        rcvECHO[x->getBroadcasterId()].insert(x->getSenderId());
        rcvECHO[x->getBroadcasterId()].insert(selfId);
        if (rcvECHO[x->getBroadcasterId()].size() >= quorumSize) {
            rcvREADY[x->getBroadcasterId()].insert(selfId);
            if (rcvREADY[x->getBroadcasterId()].size() > 2*f) {
                if (!delivered) {
                    delivered = true;
                    deliverCount++;
                    deliverTime.push_back(simTime().dbl() * 1000- roundDuration*1000);
                    cout << deliverCount << ": " << selfId << " [BRACHA] DELIVERS after " << simTime().dbl() * 1000 - roundDuration*1000 << "ms" <<  endl;
                }
            }
        }
    } else if (x->getMsgType() == READY) {
        rcvREADY[x->getBroadcasterId()].insert(x->getSenderId());
        if (rcvREADY[x->getBroadcasterId()].size() > f) {
            rcvREADY[x->getBroadcasterId()].insert(selfId);
        }
        if (rcvREADY[x->getBroadcasterId()].size() > 2*f) {
            if (!delivered) {
                delivered = true;
                deliverCount++;
                deliverTime.push_back(simTime().dbl() * 1000- roundDuration*1000);
                cout << deliverCount << ": " << selfId << " [BRACHA] DELIVERS after " << simTime().dbl() * 1000 - roundDuration*1000 << "ms" <<  endl;
            }
        }
    }

    //    cout << selfId << " has " << rcvECHO[x->getBroadcasterId()].size() << " echos, and "
    //            << rcvREADY[x->getBroadcasterId()].size() << " readys" << endl;

    bool isReady = rcvREADY[x->getBroadcasterId()].size() > f || rcvECHO[x->getBroadcasterId()].size() >= quorumSize;

    if (!sentReady && isReady) {
        sentReady = true;
        BriefPacket * cp = new BriefPacket();
        cp->setSenderId(selfId);
        cp->setBroadcasterId(x->getBroadcasterId());
        cp->setMsgId(x->getMsgId());
        cp->setMsgType(READY);
        vector<int> allNodes;
        for (int i = 0; i < nodesNbr; i++) {
            allNodes.push_back(i);
        }
        sendTo(cp, allNodes);
    } else if (!sentEcho) {
        sentEcho = true;
        BriefPacket * cp = new BriefPacket();
        cp->setSenderId(selfId);
        cp->setBroadcasterId(x->getBroadcasterId());
        cp->setMsgId(x->getMsgId());
        cp->setMsgType(ECHO);
        vector<int> allNodes;
        for (int i = 0; i < nodesNbr; i++) {
            allNodes.push_back(i);
        }
        sendTo(cp, allNodes);
    }
}


#endif
}
