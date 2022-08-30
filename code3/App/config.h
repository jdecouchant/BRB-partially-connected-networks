#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define DEBUG false

// Used for salticidae
#define HDR_START 0x0
#define HDR_PING  0x1
#define HDR_REPLY 0x2

#define HDR_DOLEV 0x3

// Used to distinguish messages
// 20 msg types =
#define HDR_SEND_INIT 0x0
#define HDR_SEND 0x1

#define HDR_ECHO 0x2
#define HDR_ECHO_INIT 0x3
#define HDR_ECHO_LOCALID 0x4

#define HDR_READY 0x5
#define HDR_READY_INIT 0x6
#define HDR_READY_LOCALID 0x7

#define HDR_ECHO_SINGLEHOP 0x8
#define HDR_ECHO_INIT_SINGLEHOP 0x9
#define HDR_ECHO_LOCALID_SINGLEHOP 0x10

#define HDR_READY_SINGLEHOP 0x11
#define HDR_READY_INIT_SINGLEHOP 0x12
#define HDR_READY_LOCALID_SINGLEHOP 0x13

#define HDR_ECHO_ECHO 0x14
#define HDR_ECHO_ECHO_INIT 0x15
#define HDR_ECHO_ECHO_LOCALID 0x16

#define HDR_READY_ECHO 0x17
#define HDR_READY_ECHO_INIT 0x18
#define HDR_READY_ECHO_LOCALID 0x19

#define HDR_ECHO_CPA 0x0
#define HDR_READY_CPA 0x1


typedef unsigned int PID; // process ids
typedef unsigned int VAL; // payload


// ----------------------------------------
// Colors
// ------

#define KNRM  "\x1B[0m"

// default background & different foreground colors
#define KRED  "\x1B[49m\x1B[31m"
#define KGRN  "\x1B[49m\x1B[32m"
#define KYEL  "\x1B[49m\x1B[33m"
#define KBLU  "\x1B[49m\x1B[34m"
#define KMAG  "\x1B[49m\x1B[35m"
#define KCYN  "\x1B[49m\x1B[36m"
#define KWHT  "\x1B[49m\x1B[37m"

// default background & different (light) foreground colors
#define KLRED  "\x1B[49m\x1B[91m"
#define KLGRN  "\x1B[49m\x1B[92m"
#define KLYEL  "\x1B[49m\x1B[93m"
#define KLBLU  "\x1B[49m\x1B[94m"
#define KLMAG  "\x1B[49m\x1B[95m"
#define KLCYN  "\x1B[49m\x1B[96m"
#define KLWHT  "\x1B[49m\x1B[97m"

// diferent background colors & white foreground
#define KBRED  "\x1B[41m\x1B[37m"
#define KBGRN  "\x1B[42m\x1B[37m"
#define KBYEL  "\x1B[43m\x1B[37m"
#define KBBLU  "\x1B[44m\x1B[37m"
#define KBMAG  "\x1B[45m\x1B[37m"
#define KBCYN  "\x1B[46m\x1B[37m"
#define KBWHT  "\x1B[47m\x1B[30m"

#endif
