#ifndef MSG_H
#define MSG_H

#include "salticidae/msg.h"
#include "salticidae/stream.h"
#include "config.h"
#include <set>

struct Dolev {
  
  static const uint8_t opcode = HDR_DOLEV;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  PID broadcasterId; 
  int broadcastId; 
  
  int pathLen;
  std::vector<PID> path; 
  
  Dolev(const int &payloadSize, const char * val, const PID &linkSenderId, const int &broadcasterId, const int &broadcastId, const std::vector<PID> path): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  Dolev(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize; 
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct Send {
  
  static const uint8_t opcode = HDR_SEND;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
//   PID broadcasterId; 
  int broadcastId;
  
  // Local Id for this broadcast
//   int localId;
  
  // Id the sender of the ECHO/READY message 
//   PID echoSender;
  
//   int pathLen;
//   std::vector<PID> path; 
  
  Send(const int &payloadSize, const char *val, const PID &linkSenderId, const int &broadcastId): payloadSize(payloadSize),  linkSenderId(linkSenderId), broadcastId(broadcastId) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcastId;
  }
  
  Send(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcastId; 
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcastId)); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcastId; 
  }
};

struct SendInit {
  
  static const uint8_t opcode = HDR_SEND_INIT;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
//   PID broadcasterId; 
  int broadcastId;
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
//   PID echoSender;
  
//   int pathLen;
//   std::vector<PID> path; 
  
  SendInit(const int &payloadSize, const char *val, const PID &linkSenderId, const int &broadcastId, const int &localId): payloadSize(payloadSize),  linkSenderId(linkSenderId), broadcastId(broadcastId), localId(localId) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcastId << localId;
  }
  
  SendInit(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcastId >> localId; 
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcastId)+sizeof(localId)); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcastId << localId; 
  }
};

struct Echo {
  
  static const uint8_t opcode = HDR_ECHO;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId;
  
  // Id the sender of the ECHO/READY message 
  PID echoSender;
  
  int pathLen;
  std::vector<PID> path; 
  
  Echo(const int &payloadSize, const char *val, const PID &linkSenderId, const int &broadcasterId, const int &broadcastId, const int &echoSender, const std::vector<PID> &path): payloadSize(payloadSize),  linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), echoSender(echoSender) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << echoSender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  Echo(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> echoSender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(echoSender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << echoSender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};    

struct EchoInit {
  
  static const uint8_t opcode = HDR_ECHO_INIT;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId;
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
  PID echoSender;
  
  int pathLen;
  std::vector<PID> path; 
  
  EchoInit(const int &payloadSize, const char *val, const PID &linkSenderId, const int &broadcasterId, const int &broadcastId, const int &localId, const PID &echoSender, const std::vector<PID> &path): payloadSize(payloadSize),  linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), localId(localId), echoSender(echoSender) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << localId << echoSender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  EchoInit(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> localId >> echoSender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(localId)+sizeof(echoSender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << localId << echoSender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct EchoLocalId {
  
  static const uint8_t opcode = HDR_ECHO_LOCALID;
  salticidae::DataStream serialized;
  
//   int payloadSize; 
//   char * val;
  
  PID linkSenderId;

  // Id the broadcast 
//   PID broadcasterId; 
//   int broadcastId;
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
  PID echoSender;
  
  int pathLen;
  std::vector<PID> path; 
  
  EchoLocalId(const PID &linkSenderId, const int &localId, const int &echoSender, const std::vector<PID> &path): linkSenderId(linkSenderId), localId(localId), echoSender(echoSender) { 
      
      serialized << linkSenderId << localId << echoSender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  EchoLocalId(salticidae::DataStream &&s) { 
      s >> linkSenderId >> localId >> echoSender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)
//       +sizeof(linkSenderId)
      +sizeof(localId)+sizeof(echoSender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << linkSenderId << localId << echoSender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct EchoSingleHop {
  
  static const uint8_t opcode = HDR_ECHO_SINGLEHOP;
  salticidae::DataStream serialized;
  
  unsigned int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId;
  
  // Id the sender of the ECHO/READY message 
//   PID echoSender;
  
  EchoSingleHop (const int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId): payloadSize(payloadSize),  linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId;
  }
  
  EchoSingleHop(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId;
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId;
  }
};    

struct EchoInitSingleHop {
  
  static const uint8_t opcode = HDR_ECHO_INIT_SINGLEHOP;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId;
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
//   PID echoSender;
  
  EchoInitSingleHop(const int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const int &localId): payloadSize(payloadSize),  linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), localId(localId) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << localId;
  }
  
  EchoInitSingleHop(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> localId;
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(localId)); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << localId;
  }
};

struct EchoLocalIdSingleHop {
  
  static const uint8_t opcode = HDR_ECHO_LOCALID_SINGLEHOP;
  salticidae::DataStream serialized;
  
//   int payloadSize; 
//   char * val;
  
  PID linkSenderId;

  // Id the broadcast 
//   PID broadcasterId; 
//   int broadcastId;
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
//   PID echoSender;
  
  EchoLocalIdSingleHop(const PID &linkSenderId, const int &localId): linkSenderId(linkSenderId), localId(localId) { 
      
      serialized << linkSenderId << localId;
  }
  
  EchoLocalIdSingleHop(salticidae::DataStream &&s) { 
      s >> linkSenderId >> localId;
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)
//       +sizeof(linkSenderId)
      +sizeof(localId)); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << linkSenderId << localId;
  }
};

struct Ready {
  
  static const uint8_t opcode = HDR_READY;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val; 
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  // Id the sender of the ECHO/READY message 
  PID readySender;
  
  int pathLen;
  std::vector<PID> path; 
  
  Ready(const int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &readySender, const std::vector<PID> &path): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), readySender(readySender) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << readySender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  Ready(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> readySender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(readySender)+sizeof(pathLen)+sizeof(PID)*path.size());}
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize; 
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << readySender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct ReadyInit {
  
  static const uint8_t opcode = HDR_READY_INIT;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val; 
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
  PID readySender;
  
  int pathLen;
  std::vector<PID> path; 
  
  ReadyInit(const int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const int &localId, const int &readySender, const std::vector<PID> &path): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), localId(localId), readySender(readySender) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << localId << readySender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  ReadyInit(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> localId >> readySender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(localId)+sizeof(readySender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize; 
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << localId << readySender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct ReadyLocalId {
  
  static const uint8_t opcode = HDR_READY_LOCALID;
  salticidae::DataStream serialized;
  
//   int payloadSize; 
//   char * val; 
  
  PID linkSenderId;

  // Id the broadcast 
//   PID broadcasterId; 
//   int broadcastId; 
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
  PID readySender;
  
  int pathLen;
  std::vector<PID> path; 
  
  ReadyLocalId(const PID &linkSenderId, const int &localId, const PID &readySender, const std::vector<PID> &path): linkSenderId(linkSenderId), localId(localId), readySender(readySender) { 

      serialized << linkSenderId << localId << readySender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  ReadyLocalId(salticidae::DataStream &&s) { 
      s >> linkSenderId >> localId >> readySender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }

  unsigned int sizeMsg() { return (sizeof(uint8_t)
//       +sizeof(linkSenderId)
      +sizeof(localId)+sizeof(readySender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << linkSenderId << localId << readySender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct ReadySingleHop {
  
  static const uint8_t opcode = HDR_READY_SINGLEHOP;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val; 
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  // Id the sender of the ECHO/READY message 
//   PID readySender;
  
  ReadySingleHop(const int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId;
  }
  
  ReadySingleHop(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId;
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId));}
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize; 
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId;
  }
};

struct ReadyInitSingleHop {
  
  static const uint8_t opcode = HDR_READY_INIT_SINGLEHOP;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val; 
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
//   PID readySender;
  
  ReadyInitSingleHop(const int &payloadSize, const char *val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const int &localId): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), localId(localId) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        serialized << val[i];
        this->val[i] = val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << localId;
  }
  
  ReadyInitSingleHop(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
        s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> localId;
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(localId)); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize; 
      for (int i = 0; i < payloadSize; i++) {
        s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << localId;
  }
};

struct ReadyLocalIdSingleHop {
  
  static const uint8_t opcode = HDR_READY_LOCALID_SINGLEHOP;
  salticidae::DataStream serialized;
  
//   int payloadSize; 
//   char * val; 
  
  PID linkSenderId;

  // Id the broadcast 
//   PID broadcasterId; 
//   int broadcastId; 
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
//   PID readySender;
  
  ReadyLocalIdSingleHop(const PID &linkSenderId, const int &localId): linkSenderId(linkSenderId), localId(localId) { 

      serialized << linkSenderId << localId;
  }
  
  ReadyLocalIdSingleHop(salticidae::DataStream &&s) { 
      s >> linkSenderId >> localId;
  }

  unsigned int sizeMsg() { return (sizeof(uint8_t)
//       +sizeof(linkSenderId)
      +sizeof(localId)); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << linkSenderId << localId;
  }
};

struct ReadyEcho {
  
  static const uint8_t opcode = HDR_READY_ECHO;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  // Id the sender of the ECHO/READY message 
  PID firstEchoSender; 
  PID secondReadySender; 
  
  int pathLen;
  std::vector<PID> path; 
  
  ReadyEcho(const int &payloadSize, const char * val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &firstEchoSender, const PID &secondReadySender, const std::vector<PID> &path): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId),  firstEchoSender(firstEchoSender), secondReadySender(secondReadySender) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
          this->val[i] = val[i];
          serialized << val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << firstEchoSender << secondReadySender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  ReadyEcho(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
          s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> firstEchoSender >> secondReadySender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(firstEchoSender)+sizeof(secondReadySender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
          s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << firstEchoSender << secondReadySender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct ReadyEchoInit {
  
  static const uint8_t opcode = HDR_READY_ECHO_INIT;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
  PID firstEchoSender; 
  PID secondReadySender;
  
  int pathLen;
  std::vector<PID> path; 
  
  ReadyEchoInit(const int &payloadSize, const char * val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const int &localId, const PID &firstEchoSender, const PID &secondReadySender, const std::vector<PID> &path): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), localId(localId), firstEchoSender(firstEchoSender), secondReadySender(secondReadySender) { 
      
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
          this->val[i] = val[i];
          serialized << val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << localId << firstEchoSender << secondReadySender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  ReadyEchoInit(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
          s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> localId >> firstEchoSender >> secondReadySender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(localId)+sizeof(firstEchoSender)+sizeof(secondReadySender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
          s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << localId << firstEchoSender << secondReadySender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct ReadyEchoLocalId {
  
  static const uint8_t opcode = HDR_READY_ECHO_LOCALID;
  salticidae::DataStream serialized;
  
//   int payloadSize; 
//   char * val;
  
  PID linkSenderId;

  // Id the broadcast 
//   PID broadcasterId; 
//   int broadcastId; 
  
  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
  PID firstEchoSender; 
  PID secondReadySender;
  
  int pathLen;
  std::vector<PID> path; 
  
  ReadyEchoLocalId(const PID &linkSenderId, const int &localId, const PID &firstEchoSender, const PID &secondReadySender, const std::vector<PID> &path): linkSenderId(linkSenderId), localId(localId), firstEchoSender(firstEchoSender), secondReadySender(secondReadySender) { 
      serialized << linkSenderId << localId << firstEchoSender << secondReadySender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]);
      }
  }
  
  ReadyEchoLocalId(salticidae::DataStream &&s) { 
      s >> linkSenderId >> localId>> firstEchoSender >> secondReadySender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }

  unsigned int sizeMsg() { return (sizeof(uint8_t)
//       +sizeof(linkSenderId)
      +sizeof(localId)+sizeof(firstEchoSender)+sizeof(secondReadySender)+sizeof(pathLen)+sizeof(PID)*path.size()); }

  void serialize(salticidae::DataStream &s) const { 
      s << linkSenderId << localId << firstEchoSender << secondReadySender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct EchoEcho {
  
  static const uint8_t opcode = HDR_ECHO_ECHO;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  // Id the sender of the ECHO/READY message 
  PID firstEchoSender;
  PID secondEchoSender; 
  
  int pathLen;
  std::vector<PID> path; 
  
  EchoEcho(const int &payloadSize, const char * val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const PID &firstEchoSender, const PID &secondEchoSender, const std::vector<PID> &path): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), firstEchoSender(firstEchoSender), secondEchoSender(secondEchoSender) { 
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
          this->val[i] = val[i];
          serialized << val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << firstEchoSender << secondEchoSender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]); 
      }
  }
  
  EchoEcho(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
          s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> firstEchoSender >> secondEchoSender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(firstEchoSender)+sizeof(secondEchoSender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
          s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << firstEchoSender << secondEchoSender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};


struct EchoEchoInit {
  
  static const uint8_t opcode = HDR_ECHO_ECHO_INIT;
  salticidae::DataStream serialized;
  
  int payloadSize; 
  char * val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 

  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
  PID firstEchoSender;
  PID secondEchoSender; 
  
  int pathLen;
  std::vector<PID> path; 
  
  EchoEchoInit(const int &payloadSize, const char * val, const PID &linkSenderId, const PID &broadcasterId, const int &broadcastId, const int &localId, const PID &firstEchoSender, const PID &secondEchoSender, const std::vector<PID> &path): payloadSize(payloadSize), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), localId(localId),firstEchoSender(firstEchoSender), secondEchoSender(secondEchoSender) { 
      serialized << payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
          this->val[i] = val[i];
          serialized << val[i];
      }
      serialized << linkSenderId << broadcasterId << broadcastId << localId << firstEchoSender << secondEchoSender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]); // TODO: missing everywhere else
      }
  }
  
  EchoEchoInit(salticidae::DataStream &&s) { 
      s >> payloadSize; 
      this->val = new char[payloadSize];
      for (int i = 0; i < payloadSize; i++) {
          s >> val[i];
      }
      s >> linkSenderId >> broadcasterId >> broadcastId >> localId >> firstEchoSender >> secondEchoSender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)+sizeof(payloadSize)+sizeof(char)*payloadSize
//       +sizeof(linkSenderId)
      +sizeof(broadcasterId)+sizeof(broadcastId)+sizeof(localId)+sizeof(firstEchoSender)+sizeof(secondEchoSender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << payloadSize;
      for (int i = 0; i < payloadSize; i++) {
          s << val[i];
      }
      s << linkSenderId << broadcasterId << broadcastId << localId << firstEchoSender << secondEchoSender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct EchoEchoLocalId {
  
  static const uint8_t opcode = HDR_ECHO_ECHO_LOCALID;
  salticidae::DataStream serialized;
  
//   int payloadSize; 
//   char * val;
  
  PID linkSenderId;

  // Id the broadcast 
//   PID broadcasterId; 
//   int broadcastId; 

  // Local Id for this broadcast
  int localId;
  
  // Id the sender of the ECHO/READY message 
  PID firstEchoSender;
  PID secondEchoSender; 
  
  int pathLen;
  std::vector<PID> path; 
  
  EchoEchoLocalId(const PID &linkSenderId, const int &localId, const PID &firstEchoSender, const PID &secondEchoSender, const std::vector<PID> &path): linkSenderId(linkSenderId), localId(localId),firstEchoSender(firstEchoSender), secondEchoSender(secondEchoSender) { 
      serialized << linkSenderId << localId << firstEchoSender << secondEchoSender;
      pathLen = path.size();
      serialized << pathLen;
      for (int i = 0; i < pathLen; i++) {
          serialized << path[i];
          this->path.push_back(path[i]); 
      }
  }

  EchoEchoLocalId(salticidae::DataStream &&s) { 
      s >> linkSenderId >> localId >> firstEchoSender >> secondEchoSender >> pathLen; 
      for (int i = 0; i < pathLen; i++) {
          PID x;
          s >> x; 
          path.push_back(x); 
      }
  }
  
  unsigned int sizeMsg() { return (sizeof(uint8_t)
//       +sizeof(linkSenderId)
      +sizeof(localId)+sizeof(firstEchoSender)+sizeof(secondEchoSender)+sizeof(pathLen)+sizeof(PID)*path.size()); }
  
  void serialize(salticidae::DataStream &s) const { 
      s << linkSenderId << localId << firstEchoSender << secondEchoSender << pathLen; 
      for (int i = 0; i < pathLen; i++) {
          s << path[i];
      }
  }
};

struct EchoCPA {
  
  static const uint8_t opcode = HDR_ECHO_CPA;
  salticidae::DataStream serialized;
  
  VAL val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  PID echoSender;
  
  
  EchoCPA(const VAL &val, const PID &linkSenderId, const int &broadcasterId, const int &broadcastId, const int &echoSender): val(val), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), echoSender(echoSender) { 
      serialized << val << linkSenderId << broadcasterId << broadcastId << echoSender;
  }
  
  EchoCPA(salticidae::DataStream &&s) { 
      s >> val >> linkSenderId >> broadcasterId >> broadcastId >> echoSender; 
  }
  
  unsigned int sizeMsg() { return (sizeof(VAL) + sizeof(PID) + sizeof(PID) + sizeof(int) + sizeof(int)); } // TODO: to revise in the end
  
  void serialize(salticidae::DataStream &s) const { 
      s << val << linkSenderId << broadcasterId << broadcastId << echoSender; 
  }
};

struct ReadyCPA {
  
  static const uint8_t opcode = HDR_READY_CPA;
  salticidae::DataStream serialized;
  
  VAL val;
  
  PID linkSenderId;

  // Id the broadcast 
  PID broadcasterId; 
  int broadcastId; 
  
  PID readySender;
  
  ReadyCPA(const VAL &val, const PID &linkSenderId, const int &broadcasterId, const int &broadcastId, const int &readySender): val(val), linkSenderId(linkSenderId), broadcasterId(broadcasterId), broadcastId(broadcastId), readySender(readySender) { 
      serialized << val << linkSenderId << broadcasterId << broadcastId << readySender;
  }
  
  ReadyCPA(salticidae::DataStream &&s) { 
      s >> val >> linkSenderId >> broadcasterId >> broadcastId >> readySender; 
  }

  unsigned int sizeMsg() { return (sizeof(VAL) + sizeof(PID) + sizeof(PID) + sizeof(int) + sizeof(int)); } // TODO: to revise in the end
  
  void serialize(salticidae::DataStream &s) const { 
      s << val << linkSenderId << broadcasterId << broadcastId << readySender; 
  }
};




#endif
