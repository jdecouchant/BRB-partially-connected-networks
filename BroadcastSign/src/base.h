#ifndef BASE_H
#define BASE_H

//const bool MAINTAIN_WITNESSED_BM = false;
/*** Message types ***/
//const int REQUEST = 8;
//const int KEY_RQST  = 1;
//const int KEY_RESP = 2;
//const int SERVE     = 3;
//const int ACK_SERVE     = 4;
//const int ACK_NODE_WITN    = 5;
//const int ACK_WITN_WITN = 6;
//const int SHARE_CRYPTO = 7;


const int SEND = 0;
const int ECHO = 1;
const int READY = 2;
const int READY_ECHO = 3; // A Ready with an embedded Echo
const int ECHO_ECHO = 4; // An Echo with an embedded Echo
const int PAYLOAD_ACK = 5;

//#define BRACHA
//#define DOLEV
//#define BRACHADOLEV  // Direct Hybrid between the two protocols (v0)

//#define BRACHACPA
//#define OPTIM_SENDTOECHO false
//#define OPTIM_SENDTOECHO true

#define BRACHADOLEV2 // Our improved BRACHADOLEV

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

/*** Constants of the streaming protocol ***/

//const int RTE = 8; //10 // Number of rounds between the release of an update and its expiration
//const int UPD_NBR_PER_ROUND = 40; // Updates number released by the source at each round
// Rounds duration, and the size of video chunks are specified in the .ini file


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
//const double PAYLOAD_SIZE = 1024.0;

//const int ROUND_ID_SIZE     = 13; // 2^13 rounds = more than 2 hours
//const int SENDER_ID_SIZE    = 18; // 2^18 = up to 262000 peers simultaneously
//const int RECEIVER_ID_SIZE  = 18;
//const int MSG_TYPE_SIZE     = 3; // up to 8 types of messages
//const int UPDATES_ID_SIZE   = 18;
//const int ENCRYPTION_SIZE   = 1024; //1024;
//const int SIGN_SIZE         = ENCRYPTION_SIZE; // Size of a signature with RSA (1024 bits)
//const int HASH_SIZE         = 160; // Size of a hash in bits with SHA-1 (as in PeerReview)
//const int KEY_SIZE          = 512; //512;
//const int COUNT_SIZE        = 32;
//const int PRIME_NUMBER_SIZE = KEY_SIZE;

#endif
