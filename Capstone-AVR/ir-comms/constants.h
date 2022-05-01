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

#define AVR_TIMING_OFFSET (16*8)

#define TRUE_START_BIT 67212

// OLD VALUES FROM microseconds
//#define START_BIT 5000/AVR_TIMING_OFFSET
//#define ZERO_BIT 700/AVR_TIMING_OFFSET
//#define ONE_BIT 1600/AVR_TIMING_OFFSET
//#define OFFSET 400/AVR_TIMING_OFFSET

#define START_BIT 43
#define ZERO_BIT 8
#define ONE_BIT 17
#define OFFSET 4

#define PACKET_DUPLICATES 3

#define BIT_CHECK_INTERVAL 10000

#define EE_ADDR_ID 0

//static uint8_t ROBOT_ID;



#endif