#ifndef __INET_MYPEERCLI_H
#define __INET_MYPEERCLI_H

#include "inet/common/INETDefs.h"

#include "inet/applications/base/ApplicationBase.h"
#include "inet/common/packet/Packet.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"

#include "packets_m.h"
#include <vector>
#include <set>
#include <map>

#include "base.h"

#include <random>
#include <bitset>

#include <chrono>

using namespace std;
using namespace std::chrono;

namespace inet {

class Peer : public ApplicationBase, public UdpSocket::ICallback {

public:
    static map<L3Address, int> addrToId;

    cMessage **processLinkTimers; // An array of timers, one per possible destination (instantiated only for the neighbors)
    vector< pair<Packet *, SimTime> > *pendingMsgsPerNeighbors;

    static vector<SimTime> deliverTime;
    static int deliverCount;

    static bool **connections;

    static int numFinish;
    static string outputPrefix;

    static double PAYLOAD_SIZE; // In bits
    static long numMsgTotal;
    static long numBitsTotal;
    static int nTransmissionsPayload;
    static long int cumulatedProcessingTime; // Store time that is lost processing messages that do not lead to msg emission

protected:
    int roundId;

//    cPar *startTimePar;
    simtime_t startTime;

    int msgId = 0;

    // state
    UdpSocket socket;
    cMessage *roundEvent;
    int localPort = -1;

    int nodesNbr;
    int f;
    int k; // connectivity

    cPar *byzStringPar = nullptr;

    vector<L3Address> nodesAddr; // store the IP addresses of all clients*
    int selfId;
    bool selfIsByzantine = false;

    int quorumSize;

    omnetpp::cTopology *topo;

    // Common
    bool delivered = false;

    // Stats
    int numSentMsg = 0;
    int numBitsSent = 0;

    cPar * outputPrefixPar = nullptr;

    int debugId = 0;

    bool delay = true;
    std::chrono::time_point<std::chrono::high_resolution_clock> timeMsgReception; // Set when a message is received
    std::default_random_engine generator;
    std::normal_distribution<double> **distribution; //(5.0,2.0);

private:

    // TODO: Currently, all these options are read in BrachaDolev2.cc, while they should be in Peer.cc
    bool OPTIM_SENDTOECHO = false;
    bool OPTIM_ECHOTOECHO = false;
    bool OPTIM_ECHOTOREADY = false;
    bool OPTIM_CUTECHOREADY_AFTER_DELIVER = false;
    bool OPTIM_CUT_ECHO_AFTER_READY_RECEIVED = false;
    bool OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED = false;
    bool OPTIM_CUT_ECHO_TO_READY_NEIGHBORS = false;
    bool OPTIM_BRACHA_OVERPROVISIONING = false;
    bool OPTIM_BITSET = false;
    bool OPTIM_MESSAGE_FIELDS = false;

    bool OPTIM_PAYLOAD_AVOID_IF_RECEIVED = false;
    bool OPTIM_SEND_2f1 = false;

    bool ASYNC = false; // Decide whether messages are sent asynchronously or not

public:
    Peer() { }
    virtual ~Peer() { }//cancelAndDelete(this->roundEvent); }

protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void finish() override;

    static bool sortbysec(const pair<Packet *, SimTime> &a, const pair<Packet *, SimTime> &b);
    virtual void handleMessageWhenUp(cMessage *msg) override;

    // ApplicationBase:
    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

    virtual void socketDataArrived(UdpSocket *socket, Packet *packet) override;
    virtual void socketErrorArrived(UdpSocket *socket, Indication *indication) override;
    virtual void socketClosed(UdpSocket *socket) override;

    virtual int getNextNodeToDst(int srcNodeID, int dstNodeID);
    virtual int getNextGateToDst(int srcNodeID, int dstNodeID);
    virtual void initializeTopo();
    virtual vector<int> getAllNeighbors();

    // Dolev functions

    virtual void sendTo(BriefPacket * bp, vector<int> ids );
    virtual void receiveECHO_or_READY(BriefPacket *x);
    virtual void receiveStream(Packet *msg); // Receive from socket and call adequate receive function

    bool * rcvMsgFrom; // To keep track of the nodes that sent a given payload, to avoid sending it again
    bool * sentAck;

    virtual void deleteBriefPacket(BriefPacket *bp);

#ifdef DOLEV
protected:
    bool *gotEmptyPathSetFrom;
    bool ***pathGraph;

    virtual void printGraph(); // DOLEV

    virtual void DOLEVinitialize();
    virtual void DOLEVfinish();

    virtual BriefPacket * DOLEVfirstMessage();
    virtual void DOLEVreceiveMessage(BriefPacket *x);
#endif

#ifdef BRACHA
protected:
    bool sentEcho = false;
    bool sentReady = false;
    map<int, set<int>> rcvECHO;
    map<int, set<int>> rcvREADY;

    virtual void BRACHAinitialize();
    virtual void BRACHAfinish();

    virtual BriefPacket * BRACHAfirstMessage();
    virtual void BRACHAreceiveMessage(BriefPacket *x);
#endif

#ifdef BRACHADOLEV // Direct combination between Bracha and Dolev's algorithm
protected:
    bool sentEcho = false;
    bool sentReady = false;
    map<pair<int, int>, bool> dolevDelivered;
    map<pair<int, int>, bool *> gotEmptyPathSetFrom;
    set<int> rcvECHOv0;
    set<int> rcvREADYv0;
    map<pair<int,int>, bool ***> mapPathGraph;

    virtual void BRACHADOLEVinitialize();
    virtual void BRACHADOLEVfinish();

    virtual BriefPacket * BRACHADOLEVfirstMessage();

    virtual void printGraphBrachaDolev(int msgType, int senderId); // BRACHADOLEV
    virtual bool BRACHADOLEVreceiveMessage(BriefPacket *x); // true if message discarded
#endif

#ifdef BRACHACPA // Direct combination between Bracha and CPA's algorithm
protected:
    bool sentEcho = false;
    bool sentReady = false;

    map<pair<int, int>, set<int>*> heardNodesPerMsg; // to collect for each message (e.g., (ECHO, p_0)) the number of neighbors who sent it

    set<pair<int, int>> validatedEchos;
    set<pair<int, int>> validatedReadys;
    set<int> validatedEchosOrReadys;

    virtual void BRACHACPAinitialize();
    virtual void BRACHACPAfinish();

    virtual BriefPacket * BRACHACPAfirstMessage();
    BriefPacket * BRACHACPAselfEcho(int broadcasterId, int msgId);
    BriefPacket * BRACHACPAselfReady(int broadcasterId, int msgId);

    virtual void BRACHACPAreceiveMessage(BriefPacket *x, bool studyEmbedded);
#endif

#ifdef BRACHADOLEV2 // Direct combination between Bracha and Dolev's algorithm
protected:
    bool sentEcho = false;

    bool isInReady = false;

    bool sentReady = false;

    bool rcvPayload = false;
    vector<BriefPacket *> pendingMsgs;

    map<pair<int, int>, bool> dolevDelivered;
    map<pair<int, int>, bool *> gotEmptyPathSetFrom;
    set<int> rcvECHO;
    set<int> rcvREADY;
    map<pair<int,int>, bool ***> mapPathGraph;
    map<int, set<int> *> neighborsToReadys;
    set<int> neighborsThatDelivered;

    map< pair<int, int>, vector< bitset<BITSET_MAX_SIZE>> > receivedPaths;

    virtual vector<int> getAllNeighborsExceptDelivered();
    virtual vector<int> getAllNeighborsExceptDeliveredAnd(int nodeId);

    BriefPacket * createBriefPacket(int msgType);
    BriefPacket * createBriefPacket(int msgType, int broadcasterId, int msgId, int echoOrReadySender, vector<int> path);
    BriefPacket * createBriefPacket(int msgType, int broadcasterId, int msgId, int echoOrReadySender, vector<int> path, int ackedEchoForREADY_ECHO); // For READY_ECHO messages
    BriefPacket * createBriefPacket(BriefPacket * x);
    virtual void BRACHADOLEV2initialize();
    virtual void BRACHADOLEV2finish();

    virtual BriefPacket * BRACHADOLEV2firstMessage();

    virtual void createGraphIfDoesNotExist(pair<int, int> xId);
    virtual void insertPathIntoGraph(pair<int, int> xId, vector<int> path);
    virtual bool checkIfEmptyPathReceivedFromOneProcessInPath(pair<int, int> xId, vector<int> path);
    virtual bool checkIfDolevDelivered(pair<int, int> xId);
    virtual bool fPlus1DisjointPaths(int source, int msgType);
    virtual vector<int> getPathFromMessage(BriefPacket *x);
    virtual vector<int> getVectorFromInt(int id);

    virtual bool bfs(int V, int **rGraph, int s, int t, int * parent);
    int **rgraph;
    virtual bool fordFulkerson(int debug, int msgType, int s, int t);

    virtual bool checkIfSubpathReceived(pair<int, int> xId, vector<int> path);
    virtual void insertPath(pair<int, int> xId, vector<int> path);

    virtual void printGraphBRACHADOLEV2(int msgType, int senderId); // BRACHADOLEV

    virtual void BRACHADOLEV2receiveMessage(BriefPacket *x);

    virtual void BRACHADOLEV2receiveMessage_ECHO(BriefPacket *x);
    virtual void BRACHADOLEV2receiveMessage_READY(BriefPacket *x);
    virtual void BRACHADOLEV2receiveMessage_READY_ECHO(BriefPacket *x);
    virtual void BRACHADOLEV2receiveMessage_ECHO_ECHO(BriefPacket *x);

#endif

#ifdef V1
protected:
    // V1 (Hybrid where information gathered during send is used)
    //    map<int, set<int>> rcvECHO;

    bool sentEcho = false;
    bool sentReady = false;
    map<pair<int, int>, bool> dolevDelivered;
    map<pair<int, int>, bool *> gotEmptyPathSetFrom;
    set<int> rcvECHOv0;
    set<int> rcvREADYv0;
    map<pair<int,int>, bool ***> mapPathGraph; // For each ack message (pi, pj) -> graph, means pj have reliably heard from pi having the message

    bool *sentAck; //To remember which acks were sent already
    int nSentAck = 0;

    bool **hasHeardFrom; // To keep track of when it is possible to deliver a message
    bool matrixDelivered = false;

    virtual void V1initMapPathGraph(int ackedId, int broadcasterId);

    virtual bool V1canDeliver();

    virtual void V1initialize();
    virtual void V1finish();

    virtual BriefPacket * V1firstMessage();

    virtual void printGraphV1(int msgType, int senderId);
    virtual void V1receiveMessage(BriefPacket *x);
#endif

#ifdef V2
protected:

    bool sentEcho = false;
    bool sentReady = false;
    map<pair<int, int>, bool> dolevDelivered;
    map<pair<int, int>, bool *> gotEmptyPathSetFrom;
    set<int> rcvECHOv0;
    set<int> rcvREADYv0;
    map<pair<int,int>, bool ***> mapPathGraph; // For each ack message (pi, pj) -> graph, means pj have reliably heard from pi having the message

    bool *sentAck; //To remember which acks were sent already
    int nSentAck = 0;

    bool **hasHeardFrom; // To keep track of when it is possible to deliver a message
    bool matrixDelivered = false;

    virtual void V2initMapPathGraph(int ackedId, int broadcasterId);

    virtual bool V2canDeliver();

    virtual void V2initialize();
    virtual void V2finish();

    virtual BriefPacket * V2firstMessage();

    virtual void printGraphV2(int msgType, int senderId);
    virtual void V2receiveMessage(BriefPacket *x);
#endif

};

} // namespace inet

#endif

