#include <iostream>


#include "Message.h"

#include "Handler.h"
#include "DolevHandler.h"
#include "BrachaDolevHandler.h"
#include "OptBrachaDolevHandler.h"
#include "BrachaCPAHandler.h"
#include "OptBrachaCPAHandler.h"
#include "Nodes.h"

#include <string>
#include <iostream>

// ./server nodeId topologyFile

int main(int argc, char const *argv[]) {

  unsigned int myid = 0;
  if (argc > 1) { sscanf(argv[1], "%d", &myid); }

  // read nodes' locations from config file
  std::string addrFile = "addresses";
  Nodes nodes(addrFile);

  // first line contains #nodes and #faulty nodes
  // the other lines describe bidirectional links
  std::string topofilename("topology.txt");
  if (argc > 2) {
      topofilename = std::string(argv[2]);
  }
  
  int payloadSize = 10; 
  if (argc > 3) {
      payloadSize = atoi(argv[3]);
  }

  bool MOD1 = false;
  bool MOD2 = false;
  bool MOD3 = false;
  bool MOD4 = false;
  bool MOD5 = false;
  
  if (argc > 4) { 
    if (!strcmp(argv[4], "True")) {
        MOD1 = true;
    }
  }
  if (argc > 5) { 
    if (!strcmp(argv[5], "True")) {
        MOD2 = true;
    }
  }
  if (argc > 6) { 
    if (!strcmp(argv[6], "True")) {
        MOD3 = true;
    }
  }
  if (argc > 7) { 
    if (!strcmp(argv[7], "True")) {
        MOD4 = true;
    }
  }
  if (argc > 8) { 
    if (!strcmp(argv[8], "True")) {
        MOD5 = true;
    }
  }
  
  // 'Handler' handler type should not be used
  //   Handler handler(myid, pconfig, topofilename);

  int choice = 2;
  if (argc > 9) {
      choice = atoi(argv[9]);
  }

  unsigned int numBcasts = 1000;
  if (argc > 10) {
    numBcasts = atoi(argv[10]);
  }

  unsigned int sleepTime = 1000000; // in microsec
  if (argc > 11) {
    sleepTime = atoi(argv[11]);
  }

  int minBidMeasure = 0;
  int maxBidMeasure = 1000;
  if (argc > 12) {minBidMeasure = atoi(argv[12]);}
  if (argc > 13) {maxBidMeasure = atoi(argv[13]);}

  bool MBD_1 = false;
  bool MBD_2 = false;
  bool MBD_3 = false;
  bool MBD_4 = false;
  bool MBD_5 = false;
  bool MBD_6 = false;
  bool MBD_7 = false;
  bool MBD_8 = false;
  bool MBD_9 = false;
  bool MBD_10 = false;
  bool MBD_11 = false;
  bool MBD_12 = false;

  int writingIntervals = 100;

  if (argc > 14) {
    if (!strcmp(argv[14], "True")) {
        MBD_1 = true;
    }
    if (!strcmp(argv[15], "True")) {
        MBD_2 = true;
    }
    if (!strcmp(argv[16], "True")) {
        MBD_3 = true;
    }
    if (!strcmp(argv[17], "True")) {
        MBD_4 = true;
    }
    if (!strcmp(argv[18], "True")) {
        MBD_5 = true;
    }
    if (!strcmp(argv[19], "True")) {
        MBD_6 = true;
    }
    if (!strcmp(argv[20], "True")) {
        MBD_7 = true;
    }
    if (!strcmp(argv[21], "True")) {
        MBD_8 = true;
    }
    if (!strcmp(argv[22], "True")) {
        MBD_9 = true;
    }
    if (!strcmp(argv[23], "True")) {
        MBD_10 = true;
    }
    if (!strcmp(argv[24], "True")) {
        MBD_11 = true;
    }
    if (!strcmp(argv[25], "True")) {
        MBD_12 = true;
    }
  }
  if (argc > 26) {
    writingIntervals = atoi(argv[26]);
  }

  // Initializing handler
  unsigned int size = std::max({sizeof(Dolev),
                                sizeof(Send),
                                sizeof(SendInit),
                                sizeof(Echo),
                                sizeof(EchoInit),
                                sizeof(EchoLocalId),
                                sizeof(EchoSingleHop),
                                sizeof(EchoInitSingleHop),
                                sizeof(EchoLocalIdSingleHop),
                                sizeof(Ready),
                                sizeof(ReadyInit),
                                sizeof(ReadyLocalId),
                                sizeof(ReadySingleHop),
                                sizeof(ReadyInitSingleHop),
                                sizeof(ReadyLocalIdSingleHop),
                                sizeof(ReadyEcho),
                                sizeof(ReadyEchoInit),
                                sizeof(ReadyEchoLocalId),
                                sizeof(EchoEcho),
                                sizeof(EchoEchoInit),
                                sizeof(EchoEchoLocalId)});
  //                                sizeof(EchoCPA),
  //                                sizeof(ReadyCPA)});

  size += sizeof(char)*payloadSize;

  PeerNet::Config pconfig;
  pconfig.max_msg_size(2*size);


    switch (choice) {
        case 1: { /** Dolev with previous optimizations **/
            DolevHandler c1(myid, pconfig, nodes, topofilename, true, true, true, true, true);
            //   DolevHandler dolev(myid, pconfig, topofilename, false, false, false, false, false);
        } break;
        case 2: {/** Sota Bracha-Dolev with optimizations **/
            BrachaDolevHandler c2(myid, pconfig, nodes, topofilename, payloadSize, false, false, false, false, false);
            //    BrachaDolevHandler dolev(myid, pconfig, topofilename, true, true, true, true, true);
        } break;
        case 3: { /** Optimized Bracha-Dolev with new optimizations **/
            std::cout << KYEL << "[id=" << myid << "] " << topofilename << KNRM << std::endl;
            std::cout << KYEL << "Options: " << myid<<" "<<MOD1<<MOD2<<MOD3<<MOD4<<MOD5<<" " <<numBcasts<<" "<<sleepTime<<" "<<minBidMeasure<<" "<<maxBidMeasure<<" "<<MBD_1<<MBD_2<<MBD_3<<MBD_4<<MBD_5<<MBD_6<<MBD_7<<MBD_8<<MBD_9<<MBD_10<<MBD_11<<MBD_12<<" "<<writingIntervals<<std::endl;
            OptBrachaDolevHandler c3(myid, pconfig, nodes, topofilename, payloadSize, MOD1, MOD2, MOD3, MOD4, MOD5, numBcasts, sleepTime, minBidMeasure, maxBidMeasure, MBD_1, MBD_2, MBD_3, MBD_4, MBD_5, MBD_6, MBD_7, MBD_8, MBD_9, MBD_10, MBD_11, MBD_12, writingIntervals);
        } break;
        case 4: {/** Sota Bracha-CPA **/
            std::string topofilenameCPA("topologyCPA.txt");
            BrachaCPAHandler c4(myid, pconfig, nodes, topofilename, true, true, true, true, true);
        } break;
        case 5: {/** Optimized Bracha-CPA with new optimizations **/
            OptBrachaCPAHandler c5(myid, pconfig, nodes, topofilename, true, true, true, true, true);
        } break;
    }

  return 0;
};
