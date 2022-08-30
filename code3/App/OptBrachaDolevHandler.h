#ifndef OPT_BRACHA_DOLEV_HANDLER_H
#define OPT_BRACHA_DOLEV_HANDLER_H

#include "Handler.h"

#include <queue>
#include <bitset>

class OptBrachaDolevHandler: public Handler {
    
private:
    
    std::map<bId, int> numSends; 
    std::map<bId, int> numSendInits;
    
    std::map<bId, int> numEchos;
    std::map<bId, int> numEchoInits; 
    std::map<bId, int> numEchoLocalIds; 
    
    std::map<bId, int> numReadys;
    std::map<bId, int> numReadyInits; 
    std::map<bId, int> numReadyLocalIds; 
    
    std::map<bId, int> numEchoEchos;
    std::map<bId, int> numEchoEchoInits; 
    std::map<bId, int> numEchoEchoLocalIds; 
    
    std::map<bId, int> numReadyEchos;
    std::map<bId, int> numReadyEchoInits; 
    std::map<bId, int> numReadyEchoLocalIds; 
    
    std::map<bId, int> bytesSends; 
    std::map<bId, int> bytesSendInits;
    
    std::map<bId, int> bytesEchos;
    std::map<bId, int> bytesEchoInits; 
    std::map<bId, int> bytesEchoLocalIds; 
    
    std::map<bId, int> bytesReadys;
    std::map<bId, int> bytesReadyInits; 
    std::map<bId, int> bytesReadyLocalIds;
    
    std::map<bId, int> bytesEchoEchos;
    std::map<bId, int> bytesEchoEchoInits; 
    std::map<bId, int> bytesEchoEchoLocalIds; 
    
    std::map<bId, int> bytesReadyEchos;
    std::map<bId, int> bytesReadyEchoInits; 
    std::map<bId, int> bytesReadyEchoLocalIds; 
    
    bool MOD_1_BONOMI = true; // delivers if rcvd from source
    bool MOD_2_BONOMI = true; // upon delivers, send empty path
    bool MOD_3_BONOMI = true; // relays content only to neighbors that haven't delivered
    bool MOD_4_BONOMI = true; // if empty path from q, then ignore paths that contain q
    bool MOD_5_BONOMI = true; // stop forwarding/analyzing a content after delivery
    
    bool MBD_1 = false; // Associate payload to ID to avoid resending + delete echo or ready sender with single hop
    bool MBD_2 = false; // Single-hop Send msgs
    bool MBD_3 = false; // Echo_Echo messages 
    bool MBD_4 = false; // Ready_Echo messages 
    bool MBD_5 = false; // Optimized Send messages 
    bool MBD_6 = false; // Ignore Echos rcvd after corresponding Ready delivered 
    bool MBD_7 = false; // Ignore all Echos rcvd after delivering content 
    bool MBD_8 = false; // Do not send Echos to Ready neighbor  
    bool MBD_9 = false; // Do not send anything to neighbor that delivered 
    bool MBD_10 = false; // Ignore msgs whose path is a superpath of known path 
    bool MBD_11 = false; // Bracha overprovisioning - to improve in send_echo and send_ready 
    bool MBD_12 = false; // Process sends its new Echo to only 2f+1 other processes

    // number of broadcasts to start
    unsigned int numBcasts = 10; // default value
    
    // time to sleep between broadcasts (in microseconds)
    unsigned int sleepTime = 1000000; // 1ms // default value
    
    int minBidMeasure = 0; // default value
    int maxBidMeasure = 1000; // default value
    
    int nextLocalId = 0;
    void ifNeededCreateLocalId(bId broadId, localInfo li);
    
    const std::vector<PID> emptyPath;
    
    std::map<bId, std::pair<std::chrono::milliseconds::rep, bool>> deliveredBroad; // Broadcasts that are over
    
    std::string statsfilename = "./log/";
    
    // For Bracha-Dolev: store the ids of the nodes that sent Echos or Readys
    std::map<bId, std::set<PID>> rcvEchos; // cleaned
    std::map<bId, std::set<PID>> rcvReadys; // cleaned
    
    // For MBD_9
    std::map<bId, std::map<PID, std::set<PID>>> readysWithEmptyPaths; // cleaned
    std::map<bId, std::set<PID>> neighborsToAvoid;  // cleaned
    
    // For MBD_1 (localId)
    
    std::map<bId, int> bidToLocalId; // bid -> local Id // cleaned
    std::map<int, std::map<int, localInfo>> pidToLocalInfo; // PID -> localId -> {broadcaster, broadcastId, payloadSize, val} // cleaned
    
    // TODO: max number of supported nodes is 100 right now, should raise an alert if more nodes at init time
    std::map<int, std::bitset <100>> pidsThatKnowLocalId;
    
    std::map<PID, std::map<int, std::queue<EchoLocalId>>> pendingEchoLocalId;
    std::map<PID, std::map<int, std::queue<EchoLocalIdSingleHop>>> pendingEchoLocalIdSingleHop;
    
    std::map<PID, std::map<int, std::queue<ReadyLocalId>>> pendingReadyLocalId;
    std::map<PID, std::map<int, std::queue<ReadyLocalIdSingleHop>>> pendingReadyLocalIdSingleHop;
    std::map<PID, std::map<int, std::queue<EchoEchoLocalId>>> pendingEchoEchoLocalId;
    std::map<PID, std::map<int, std::queue<ReadyEchoLocalId>>> pendingReadyEchoLocalId;
    
    void consumeAllPending(const PID linkSenderId, const int localId, const PeerNet::conn_t &conn); 
    
     // DEBUG functions 
    inline void checkPath(Echo msg); 
    inline void checkPath(Ready msg);

    bool precomputeSets = true;
    
    // For each (senderId, broadcastId) store the paths that are received per node (neighbour)
    // Solution 1 
    std::map<struct mId, std::vector<std::string> *> pathsBitmasks; // an array of vector of paths (one per possible node in the network) per broadcast id
    bool msgCanBeDelivered(struct mId msgId, PID senderId);
    
    // Solution 2: computation of all non-intersecting computations as they arrive
    std::map<struct mId, std::vector<std::pair<long long, int>> *> allNonIntersectingSets; 
    bool recomputeAllNonIntersectingSets(struct mId msgId, std::string newPath); 
    
    std::set<struct mId> deliveredMsgs; // Echo/Ready messages that have been Dolev-delivered

    std::map<struct mId, std::set<int>> neighborsThatDelivered;

    // thread to send messages
    std::thread send_thread;

    void send_multiple_broadcasts();
    void printStats();
    bool firstWrite = true;

    std::chrono::milliseconds::rep getCurrentTime(); 
    int countDelivers = 0;
    int writingIntervals = 100;
    void printBroadcastTime(PID broadcasterId, int broadcastId, bool firstBroadcastWrite);
    void printDeliverTime(PID broadcasterId, int broadcastId); 
    
    bool isSelectedToSendEchos(PID broadcasterId);
    bool isSelectedToSendReadys(PID broadcasterId);
    
    void init_broadcast();
    
    void clean_after_deliver(bId broadId);
    
    void handle_send(Send msg, const PeerNet::conn_t &conn);
    void handle_sendInit(SendInit msg, const PeerNet::conn_t &conn); // directly calls echoInit_2
                         
    bool send_echo(struct mId msgId, const char * val);
    
    void handle_echo(Echo msg, const PeerNet::conn_t &conn);
    void handle_echoInit(EchoInit msg, const PeerNet::conn_t &conn);
    void handle_echoLocalId(EchoLocalId msg, const PeerNet::conn_t &conn);
    
    void handle_echoInit_2(const unsigned int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const int &localId, const PID &echoSender, const std::vector<PID> msgPath, const unsigned int &sizeMsg, bool doStats);
    
    void handle_echoSingleHop(EchoSingleHop msg, const PeerNet::conn_t &conn);
    void handle_echoInitSingleHop(EchoInitSingleHop msg, const PeerNet::conn_t &conn);
    void handle_echoLocalIdSingleHop(EchoLocalIdSingleHop msg, const PeerNet::conn_t &conn);
    
    void handle_echo_2(const unsigned int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &echoSender, const std::vector<PID> &msgPath, const unsigned int &sizeMsg, const bool doStats);
    
    bool send_ready(struct mId msgId, const char * val);
    
    void handle_ready(Ready msg, const PeerNet::conn_t &conn);
    void handle_readyInit(ReadyInit msg, const PeerNet::conn_t &conn);
    void handle_readyLocalId(ReadyLocalId msg, const PeerNet::conn_t &conn);
    
    void handle_readySingleHop(ReadySingleHop msg, const PeerNet::conn_t &conn);
    void handle_readyInitSingleHop(ReadyInitSingleHop msg, const PeerNet::conn_t &conn);
    void handle_readyLocalIdSingleHop(ReadyLocalIdSingleHop msg, const PeerNet::conn_t &conn);
    
    void handle_ready_2(const int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &readySender, const std::vector<PID> msgPath, const unsigned int &sizeMsg, const bool doStats);
    
    void send_echoEcho(struct mId msgId, const char * val, std::vector<PID> path);
    
    void handle_echoEcho(EchoEcho msg, const PeerNet::conn_t &conn);
    
    void handle_echoEcho_2(const int &payloadSize, const char * val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &firstEchoSender, const PID &secondEchoSender, const std::vector<PID> msgPath, const unsigned int &sizeMsg, const bool doStats);
    
    
    void handle_echoEchoInit(EchoEchoInit msg, const PeerNet::conn_t &conn);
    void handle_echoEchoLocalId(EchoEchoLocalId msg, const PeerNet::conn_t &conn);
    
    void send_readyEcho(struct mId msgId, const char * val, std::vector<PID> path);
    
    void handle_readyEcho(ReadyEcho msg, const PeerNet::conn_t &conn);
    
    void handle_readyEcho_2(const int &payloadSize, const char * val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &firstEchoSender, const PID &secondReadySender, const std::vector<PID> msgPath, const unsigned int &sizeMsg, bool doStats);
    
    void handle_readyEchoInit(ReadyEchoInit msg, const PeerNet::conn_t &conn);
    void handle_readyEchoLocalId(ReadyEchoLocalId msg, const PeerNet::conn_t &conn);

public:    
    OptBrachaDolevHandler(PID id, PeerNet::Config pconf, Nodes nodes, std::string topofilename, int payloadSize, bool mod1, bool mod2, bool mod3, bool mod4, bool mod5, unsigned int numBcasts, unsigned int sleepTime, int minBidMeasure, int maxBidMeasure, bool MBD_1, bool MBD_2, bool MBD_3, bool MBD_4, bool MBD_5, bool MBD_6, bool MBD_7, bool MBD_8, bool MBD_9, bool MBD_10, bool MBD_11, bool MBD_12, int writingIntervals);

};

#endif
