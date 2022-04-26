/*
 * main.c
 *
 * Created: 4/21/2022 3:02:31 PM
 *  Author: Gus
 */

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
//#include <xc.h>

// There is no byte in c so I use uint8_t
#define byte uint8_t
static volatile unsigned long microseconds;
volatile bool LED_ON = false;

// constants.h
#ifndef GUS_CONSTANTS
#define GUS_CONSTANTS

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
static uint8_t ROBOT_ID = 1;

#endif

//#ifndef IR_BLASTER
//#define IR_BLASTER

// TODO: Implement the micros function for myself
inline static unsigned long micros() { return microseconds; }

inline void setLED() {
    PORTB |= 1 << IR_LED;
}

inline void unsetLED() {
    PORTB &= ~(1 << IR_LED);
}
inline void stopIR() {
    TCCR2 = 0x00;
}

inline void startIR() {
    // Sets to CTC
    TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS20);
}

inline void setupIR() {
    //cli();
    OCR2 = 110;
    DDRB = (1 << IR_LED);
    TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS20);
    //TIMSK = (1 << OCIE2);
    TCNT2 = 0;
}

inline void setupTimer() {
	OCR0 = 100;
	TCNT0 = 0;
	TCCR0 = (1 << WGM01) | (1 << CS01);
	TIMSK = (1 << TOIE0);
}

ISR(TIMER0_OVF_vect) {
	//cli();
    PORTB ^= 1 << IR_LED;
	//microseconds++;
	//sei();
}

ISR(TIMER0_COMP_vect) {
	PORTB ^= 1 << IR_LED;
	//microseconds++;
}
//#endif

#ifndef TRANSMISSION_LOCK
#define TRANSMISSION_LOCK
#include "constants.h"

enum CommState {
    PASSIVE,
    ACTIVE_LISTENING,
    START_BLAST,
    CRASH,
    TALK,
} commState;

// CommState commState = CommState::PASSIVE;

/**
 * Indicates that the robot has completed receiving the packet.
 */
inline void endListening() {
    if (commState == ACTIVE_LISTENING) {
        commState = PASSIVE;
    }
}

/**
 * Indicates that the robot has stop emitting communications.
 */
inline void endEmitting() {
    if (commState == TALK) {
        commState = PASSIVE;
    }
}

/**
 * Alerts the lock that a communication is being received.
 */
inline bool receiveComms() {
    if (commState == ACTIVE_LISTENING) {
        return true;
    }
    if (commState == PASSIVE) {
        commState = ACTIVE_LISTENING;
        return true;
    }
    return false;
}

/**
 * Tries to begin broadcasting. Returns true if
 * the robot can begin communicating.
 */
inline bool startBlast() {
    if (commState == PASSIVE) {
        commState = TALK;
        return true;
    }
    return false;
}

#endif

// Packet

#ifndef PACKET
#define PACKET
typedef struct packet_header {
    uint8_t source;
    uint8_t destination;
    uint8_t packetNumber;
    uint8_t checksum;
} Packet_Header;

typedef struct packet {
    Packet_Header packetHeader;
    byte packetMessage[PACKET_SIZE];
    uint32_t packetSize;
    uint32_t bitIndex;
} Packet;

static uint8_t packetIndex = 0;

uint32_t getFullSize() { return PACKET_SIZE * BYTE_SIZE + sizeof(Packet_Header) * BYTE_SIZE; }

static uint8_t generateChecksum(Packet* packet) {
    uint8_t sum = 0;
    for (int i = 0; i < PACKET_SIZE * BYTE_SIZE && i < packet->packetSize * BYTE_SIZE; i++) {
        sum += (packet->packetMessage[i / BYTE_SIZE] >> (i % BYTE_SIZE)) & 1;
    }
    return sum;
}

void initPacketHelper(Packet* packet, uint8_t destination, byte data[], uint32_t sizeData) {
    // TODO: Initialize ROBOT_ID
    // if (ROBOT_ID == 0) eeprom_read_byte(&ROBOT_ID);
    for (uint16_t i = 0; i < sizeData && i < PACKET_SIZE; i++) {
        packet->packetMessage[i] = (i < sizeData) ? data[i] : 0;
    }
    if (sizeData > PACKET_SIZE) {
        sizeData = PACKET_SIZE;
    }
    packet->bitIndex = 0;
    packet->packetSize = sizeData * BYTE_SIZE + sizeof(Packet_Header) * BYTE_SIZE;  // bytes to bits
    uint8_t checksum = generateChecksum(packet);
    Packet_Header header = {.source = ROBOT_ID, .destination = destination, .packetNumber = packetIndex++, .checksum = checksum};
    packet->packetHeader = header;
}

void initPacket(Packet* packet, byte data[], uint32_t sizeData) {
    initPacketHelper(packet, -1, data, sizeData);
}

bool getBit(Packet* packet, int index) {
    if (index < 0 || index >= getFullSize()) {
        return false;
    } else if (index < BYTE_SIZE * 1) {
        return (packet->packetHeader.source >> (index % BYTE_SIZE)) & 1;
    } else if (index < BYTE_SIZE * 2) {
        return (packet->packetHeader.destination >> (index % BYTE_SIZE)) & 1;
    } else if (index < BYTE_SIZE * 3) {
        return (packet->packetHeader.packetNumber >> (index % BYTE_SIZE)) & 1;
    } else if (index < BYTE_SIZE * 4) {
        return (packet->packetHeader.checksum >> (index % BYTE_SIZE)) & 1;
    } else {
        index -= BYTE_SIZE * 4;
        return (packet->packetMessage[index / BYTE_SIZE] >> (index % BYTE_SIZE)) & 1;
    }
    byte pack = packet->packetMessage[index / BYTE_SIZE];
    int bit = index % BYTE_SIZE;
    return (pack >> bit) & 1;
}
bool getNextBit(Packet* packet) { return getBit(packet, packet->bitIndex++); }

void setBit(Packet* packet, int index, bool bit) {
    if (index < 0 || index >= getFullSize()) {
        // TODO: Add out of bounds error
    } else if (index < BYTE_SIZE * 1) {
        packet->packetHeader.source |= bit << (index % BYTE_SIZE);
    } else if (index < BYTE_SIZE * 2) {
        index -= BYTE_SIZE * 1;
        packet->packetHeader.destination |= bit << (index % BYTE_SIZE);
    } else if (index < BYTE_SIZE * 3) {
        index -= BYTE_SIZE * 2;
        packet->packetHeader.packetNumber |= bit << (index % BYTE_SIZE);
    } else if (index < BYTE_SIZE * 4) {
        index -= BYTE_SIZE * 3;
        packet->packetHeader.checksum |= bit << (index % BYTE_SIZE);
    } else {
        index -= BYTE_SIZE * 4;
        packet->packetMessage[index / BYTE_SIZE] |= bit << (index % BYTE_SIZE);
    }
}
void setNextBit(Packet* packet, bool bit) { setBit(packet, packet->bitIndex++, bit); }

bool atEnd(Packet* packet) { return packet->bitIndex >= getFullSize(); }
void resetBit(Packet* packet) { packet->bitIndex = 0; }

bool checkPacketValid(Packet* packet) {
    return packet->packetHeader.checksum == generateChecksum(packet);
}
byte* getMessage(Packet* packet) { return packet->packetMessage; }
Packet_Header getHeader(Packet* packet) { return packet->packetHeader; }

#endif

// Transmitter
// #ifndef TRANSMITTER
// #define TRANSMITTER

// enum TransmitState {
//     WAITING_FOR_PACKET,
//     START,
//     SET_DELAY,
//     TRANSMIT,
//     WAIT,
//     FALL,
//     END,
// };

// void setup();
// void loop();
// void startBit();

// bool canCompute() { return state == WAIT && nextTransmitTime > micros() + 1100; }

// void transmitPacket(byte* message, uint32_t size = PACKET_SIZE);
// void transmitPacket(byte* message, uint32_t size, uint16_t transmitTimes);
// bool hasPacket();

// void enableTransmit() { blockTransmit = false; }
// void disableTransmit() {
//     blockTransmit = true;
//     state = START;
// }

// bool hasFreshPacket = false;
// int16_t packetDuplicates = 0;

// TransmissionLock* lock = TransmissionLock::getInstance();
// TransmitState state;
// TransmitState nextState;
// Packet packet;
// Zumo32U4IRPulses::Direction direction;
// uint16_t brightness = 100;
// /** The default frequency is 16000000 / (420 + 1) = 38.005 kHz */
// const uint16_t period = 420;

// uint32_t nextTransmitTime = 0;
// uint32_t transmitTime = 0;
// uint32_t lowTime = 0;
// void checkButtons();

// bool blockTransmit = false;

// uint64_t lastResetTime = 0;

// // Test or debug variables
// bool testSignalStrengths = false;

// #endif

int main(void) {
	microseconds = 0;
	unsetLED();
	_delay_ms(100);
	setLED();
	_delay_ms(500);
	unsetLED();
    setupIR();
    setupTimer();
	_delay_ms(500);
	stopIR();
	sei();
	DDRB = (1 << IR_LED);
    while (1) {
		//cli();
		//if(microseconds > 10){
			//PORTB ^= (1 << IR_LED);
			//microseconds = 0;
		//}
		//sei();
    }
    return 1;
}
