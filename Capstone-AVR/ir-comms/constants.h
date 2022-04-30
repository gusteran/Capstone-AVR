#ifndef GUS_CONSTANTS
#define GUS_CONSTANTS

#include "low-level.h"
#include <stdlib.h>

// #define TRANSMITTER
#ifndef TRANSMITTER
#define RECEIVER
#endif
// #define IR_TRANSMISSION_PRINTS

#define INTERVAL 150

#define COUNT 10
#define PACKET_SIZE 32
// #define BUF_SIZE (8 * PACKET_SIZE)
#define BYTE_SIZE 8

#define FRONT_IR PF0
#define BACK_IR PF1
#define LEFT_IR PF6
#define RIGHT_IR PF7

#define RECV_PIN FRONT_IR

#define START_BIT 5000
#define ZERO_BIT 700
#define ONE_BIT 1600
#define OFFSET 400

#define PACKET_DUPLICATES 3

#define BIT_CHECK_INTERVAL 10000

#define EE_ADDR_ID 0



#endif