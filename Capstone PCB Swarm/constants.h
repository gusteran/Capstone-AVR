#ifndef GUS_CONSTANTS
#define GUS_CONSTANTS

#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define IR_TRANSMISSION_PRINTS

#define IR_LED PB7

#define INTERVAL 150

#define COUNT 10
#define PACKET_SIZE 32
// #define BUF_SIZE (8 * PACKET_SIZE)
#define BYTE_SIZE 8

#define RECV_PIN 22
#define RECV_ANALOG A4
#define SEND_PIN 6

#define START_BIT 5000
#define ZERO_BIT 700
#define ONE_BIT 1600
#define OFFSET 400

#define PACKET_DUPLICATES 3

#define BIT_CHECK_INTERVAL 10000

#define EE_ADDR_ID 0

// static uint8_t ROBOT_ID;

// const int START_PACKET[PACKET_SIZE] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}; //{1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0};
// const int END_PACKET[PACKET_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   //{0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1}

// unsigned long nextTime = 0;

#endif