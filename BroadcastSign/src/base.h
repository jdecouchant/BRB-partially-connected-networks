#ifndef BASE_H
#define BASE_H

// Message types
const int SEND = 0;
const int ECHO = 1;
const int READY = 2;
const int READY_ECHO = 3; // A Ready with an embedded Echo
const int ECHO_ECHO = 4; // An Echo with an embedded Echo
const int PAYLOAD_ACK = 5;

// Define the protocol that will be used

//#define BRACHA
//#define DOLEV
//#define BRACHADOLEV  // Direct combination of the two protocols
#define BRACHADOLEV2 // Our improved BRACHADOLEV

//#define BRACHACPA // Experimental - not necessary
//#define OPTIM_SENDTOECHO false
//#define OPTIM_SENDTOECHO true

// All modifications of BRACHADOLEV2 that can be activated

#define BITSET_MAX_SIZE 100
//#define OPTIM_SENDTOECHO true
//#define OPTIM_ECHOTOECHO false
//#define OPTIM_ECHOTOREADY true
//#define OPTIM_CUTECHOREADY_AFTER_DELIVER true
//#define OPTIM_CUT_ECHO_AFTER_READY_RECEIVED true
//#define OPTIM_AVOID_NEIGHBORS_THAT_DELIVERED true
//#define OPTIM_CUT_ECHO_TO_READY_NEIGHBORS true
//#define OPTIM_BRACHA_OVERPROVISIONING true
#define OPTIM_BRACHA_2 true
//#define OPTIM_EXPERIMENTAL_READY_ECHO false
//#define OPTIM_MESSAGE_FIELDS true
//#define OPTIM_PAYLOAD_NONE true
//#define OPTIM_PAYLOAD_AVOID_IF_RECEIVED false
//#define OPTIM_PAYLOAD_REQUEST false
//#define OPTIM_SEND_2f1 false

#define OPTIM_DOLEV_1 true
#define OPTIM_DOLEV_2 true
#define OPTIM_DOLEV_3 true
#define OPTIM_DOLEV_4 true

/*** Size of log components in bits ***/

const double MSG_TYPE_SIZE = 3.0;
const double PAYLOAD_PRESENCE_BIT = 1.0;
const double MSG_ID_BIT = 1.0;
const double LOCAL_ID_BIT = 1.0;
const double SELF_MSG_BIT = 1.0;

const double PROCESS_ID_SIZE = 10.0; // 2^10 = 1024
const double MSG_ID_SIZE = 32.0;

const double LOCAL_ID_SIZE = 32.0; // decided between two nodes

const double PATH_LENGTH_SIZE = 10.0; // At most the path contains all processes
// + PATH

const double PAYLOAD_ID_SIZE = 8.0; // in bits, to indicate the number of bytes that the payload contains


#endif
