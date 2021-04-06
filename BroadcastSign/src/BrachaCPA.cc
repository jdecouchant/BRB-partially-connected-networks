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
#ifdef BRACHACPA

void Peer::BRACHACPAinitialize() {
    rcvMsgFrom = new bool[nodesNbr];
    for (int i = 0; i < nodesNbr; i++) {
        rcvMsgFrom[i] = false;
    }
}

void Peer::BRACHACPAfinish() {
    cout << "Peer " << selfId << " sent " << this->numSentMsg << " msgs and " << this->numBytesSent << " B (msgId+payload = " << (msgSize+PROCESS_ID_SIZE+MSG_ID_SIZE) << " B) "
            << numBytesSent/(msgSize+PROCESS_ID_SIZE+MSG_ID_SIZE) << "X" <<  endl;
    delete[] rcvMsgFrom;
}

BriefPacket * Peer::BRACHACPAfirstMessage() {
    BriefPacket * bp = new BriefPacket();
    bp->setMsgType(ECHO);
    bp->setBroadcasterId(selfId);
    bp->setMsgId(msgId);

    bp->setEchoOrReadySender(selfId); // Not counted in the chunk length

    bp->setChunkLength(B(MSG_TYPE_SIZE + (PROCESS_ID_SIZE + MSG_ID_SIZE))); // + PAYLOAD_SIZE

    bp->setLinkSenderId(selfId);

    sentEcho = true;
    validatedEchos.insert(make_pair(ECHO, selfId));

    return bp;
}

BriefPacket * Peer::BRACHACPAselfEcho(int broadcasterId, int msgId) {
    BriefPacket * bp = new BriefPacket();
    bp->setMsgType(ECHO);
    bp->setBroadcasterId(broadcasterId);
    bp->setMsgId(msgId);
    bp->setEchoOrReadySender(selfId);
    bp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PROCESS_ID_SIZE)); // + PAYLOAD_SIZE));
    bp->setLinkSenderId(selfId);
    return bp;
}

BriefPacket * Peer::BRACHACPAselfReady(int broadcasterId, int msgId) {
    BriefPacket * bp = new BriefPacket();
    bp->setMsgType(READY);
    bp->setBroadcasterId(broadcasterId);
    bp->setMsgId(msgId);
    bp->setEchoOrReadySender(selfId);
    bp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PROCESS_ID_SIZE)); // + PAYLOAD_SIZE));
    bp->setLinkSenderId(selfId);
    return bp;
}


void Peer::BRACHACPAreceiveMessage(BriefPacket *x, bool studyEmbedded) {

    // DONE: do not send to the neighbors that already have a message
    // DONE: send to echo optimization has been implemented, option OPTIM_SENDTOECHO
    // IN PROGRESS: echo to ready optimization
    // TODO: get rid of the studyEmbedded that is used to extract the sender's echo
    // TODO: messages can be transformed like in Bracha/Dolev during transitions to new phase
    // TODO: use options for the transitions

    if (selfIsByzantine) return;

    // Optim
    if (OPTIM_SENDTOECHO && studyEmbedded && selfId != x->getBroadcasterId() && x->getMsgType() == ECHO && x->getBroadcasterId() != x->getLinkSenderId()) {

        pair<int, int> xId2 = make_pair(x->getMsgType(), x->getBroadcasterId());
        bool wasNotValidated2 = heardNodesPerMsg.find(xId2) == heardNodesPerMsg.end()
                                            or (heardNodesPerMsg[xId2]->find(x->getBroadcasterId()) == heardNodesPerMsg[xId2]->end()
                                                    and heardNodesPerMsg[xId2]->size() < f+1); // Order is important
        // Receive the echo from the source
        // and later receive the echo from the sender
        if (wasNotValidated2) {
            BriefPacket * bp = new BriefPacket();
            bp->setMsgType(ECHO);
            bp->setBroadcasterId(x->getBroadcasterId());
            bp->setMsgId(x->getMsgId());
            bp->setEchoOrReadySender(x->getBroadcasterId());
            bp->setLinkSenderId(x->getLinkSenderId());

            if (heardNodesPerMsg.find(xId2) != heardNodesPerMsg.end()) {
                heardNodesPerMsg[xId2]->insert(x->getLinkSenderId());
            } else {
                set<int> * s = new set<int>;
                s->insert(x->getLinkSenderId());
                heardNodesPerMsg[xId2] = s;
            }

            BRACHACPAreceiveMessage(bp, false);
        }
    }


    rcvMsgFrom[x->getLinkSenderId()] = true;
    pair<int, int> xId = make_pair(x->getMsgType(), x->getEchoOrReadySender());

    bool wasNotValidated = heardNodesPerMsg.find(xId) == heardNodesPerMsg.end()
                                    or (heardNodesPerMsg[xId]->find(x->getEchoOrReadySender()) == heardNodesPerMsg[xId]->end()
                                            and heardNodesPerMsg[xId]->size() < f+1); // Order is important

    if (!wasNotValidated) return;

    //    cout << selfId << " received message (" << x->getMsgType()<<", "<<x->getEchoOrReadySender()<<"): " << ((wasNotValidated)? "Not validated": "Validated") << endl;

    //    cout << "Inserting " << x->getSenderId() << endl;
    // Update the list of nodes that sent that particular message
    if (heardNodesPerMsg.find(xId) != heardNodesPerMsg.end()) {
        //        cout << "case 1 " << endl;
        heardNodesPerMsg[xId]->insert(x->getLinkSenderId());
    } else {
        //        cout << "case 2 " << endl;
        set<int> * s = new set<int>;
        s->insert(x->getLinkSenderId());
        heardNodesPerMsg[xId] = s;
        //heardNodesPerMsg[make_pair(x->getMsgType(), x->getEchoOrReadySender())]->insert(x->getSenderId());
    }

    //    cout << x->getBroadcasterId() << endl;
    //    if (heardNodesPerMsg[make_pair(x->getMsgType(), x->getEchoOrReadySender())]->find(x->getBroadcasterId())
    //            != heardNodesPerMsg[make_pair(x->getMsgType(), x->getEchoOrReadySender())]->end()) {
    //        cout << "Found " << x->getBroadcasterId() << " in heardNodesPerMsg" << endl;
    //    } else {
    //        cout << "Not found" << endl;
    //    }

    bool isValidated = x->getEchoOrReadySender() == x->getLinkSenderId()
                                    or heardNodesPerMsg[make_pair(x->getMsgType(), x->getEchoOrReadySender())]->size() >= f+1;
    //    cout << selfId << " message (" << x->getMsgType()<<", "<<x->getEchoOrReadySender()<<") is now: " << ((isValidated)? "Validated": "Not validated") << endl;

    //    bool test = heardNodesPerMsg.find(make_pair(x->getMsgType(), x->getEchoOrReadySender())) == heardNodesPerMsg.end()
    //            or heardNodesPerMsg[make_pair(x->getMsgType(), x->getEchoOrReadySender())]->size() < f+1
    //            or heardNodesPerMsg[make_pair(x->getMsgType(), x->getEchoOrReadySender())]->find(x->getBroadcasterId())
    //            == heardNodesPerMsg[make_pair(x->getMsgType(), x->getEchoOrReadySender())]->end();
    //    cout << selfId << " received message (" << x->getMsgType()<<", "<<x->getEchoOrReadySender()<<"): " << ((test)? "Not validated": "Validated") << endl;

    // Check if the msg can be validated
    if (isValidated && wasNotValidated) {
        if (x->getMsgType() == ECHO) {
            validatedEchos.insert(make_pair(ECHO, x->getEchoOrReadySender()));
        } else if (x->getMsgType() == READY){
            validatedReadys.insert(make_pair(READY, x->getEchoOrReadySender()));
        }
        validatedEchosOrReadys.insert(x->getEchoOrReadySender());

        if (not (OPTIM_SENDTOECHO and MSG_TYPE_SIZE == ECHO and x->getLinkSenderId() == x->getBroadcasterId())) { // Do not forward the send message
            //        cout << selfId << " Fowarding a message (" << x->getMsgType()<<", "<<x->getEchoOrReadySender()<<") following validation" << endl;
            BriefPacket * bp = new BriefPacket();
            bp->setMsgType(x->getMsgType());
            bp->setBroadcasterId(x->getBroadcasterId());
            bp->setMsgId(msgId);
            bp->setEchoOrReadySender(x->getEchoOrReadySender());
            bp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + PROCESS_ID_SIZE));
            bp->setLinkSenderId(selfId);

            //cout << selfId << " forwarding (" << x->getMsgType() << ", "<<x->getEchoOrReadySender()<<")" << endl;
            vector<int> diff;
            vector<int> allNeighbors = getAllNeighbors();

            for (int j = 0; j < allNeighbors.size(); j++) {
                if (allNeighbors[j]!=x->getLinkSenderId() && heardNodesPerMsg[xId]->find(allNeighbors[j]) == heardNodesPerMsg[xId]->end()) {
                    diff.push_back(allNeighbors[j]);
                }
            }
            sendTo(bp, diff);
        }
    }

    bool isReady = (validatedEchos.size() >= quorumSize) || (validatedReadys.size() >= f+1);

    // Check if a new message should be sent
    if (!sentReady && isReady) {
//        cout << "Entering ready" << endl;
        BriefPacket * cp = BRACHACPAselfReady(x->getBroadcasterId(), x->getMsgId());
        //cp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + ((x->getBroadcasterId() == x->getSenderId())? 0: PROCESS_ID_SIZE) +
        //                    PATH_SIZE + PROCESS_ID_SIZE * cp->getPathArraySize() + PAYLOAD_SIZE));
        sendTo(cp, getAllNeighbors());
        cout << selfId << " sending ready" << endl;
        //        cout << selfId << " Sending own ready ("  << cp->getMsgType()<<", "<<cp->getEchoOrReadySender() << ")" << endl;
        sentReady = true;
        sentEcho = true;
        validatedReadys.insert(make_pair(READY, selfId));
    } else if (!sentEcho) {
        //        if (x->getBroadcasterId() == x->getSenderId()
        if (validatedEchosOrReadys.size() >= f+1 or
                (heardNodesPerMsg.find(make_pair(ECHO, x->getBroadcasterId())) != heardNodesPerMsg.end() and
                (heardNodesPerMsg[make_pair(ECHO, x->getBroadcasterId())]->find(x->getBroadcasterId()) != heardNodesPerMsg[make_pair(ECHO, x->getBroadcasterId())]->end()
                or heardNodesPerMsg[make_pair(ECHO, x->getBroadcasterId())]->size() >= f+1))) { // rcv echo from its initial sender -> forward
            validatedEchos.insert(make_pair(ECHO, selfId));
            BriefPacket * cp = BRACHACPAselfEcho(x->getBroadcasterId(), x->getMsgId());
            cout << selfId << " sending echo" << endl;
            //cp->setChunkLength(B(MSG_TYPE_SIZE + PROCESS_ID_SIZE + MSG_ID_SIZE + ((x->getBroadcasterId() == x->getSenderId())? 0: PROCESS_ID_SIZE) +
            //                    PATH_SIZE + PROCESS_ID_SIZE * cp->getPathArraySize() + PAYLOAD_SIZE));
            sendTo(cp, getAllNeighbors());
            sentEcho = true;
        }
    }

    if (!delivered and validatedReadys.size() >= quorumSize) {
        delivered = true;
        deliverCount++;
        deliverTime.push_back(simTime().dbl() * 1000- roundDuration*1000);
        cout << deliverCount << ": " << selfId << " [BRACHACPA] DELIVERS after " << simTime().dbl() * 1000 - roundDuration*1000 << "ms" <<  endl;
    }

}


#endif
}
