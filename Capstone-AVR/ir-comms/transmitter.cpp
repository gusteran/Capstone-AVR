#include "transmitter.h"

//byte hello[14] = "Hello, world!";
//char bString[20] = "That's the B button";
//byte fullPacket[32] = "This string is the entire buff!";

volatile uint32_t Transmitter::transmitterTicks = 0;

Transmitter::Transmitter() : packet() {
}

void Transmitter::setup() {
    state = WAITING_FOR_PACKET;
    //packet.init(hello, sizeof(hello));
    lock = TransmissionLock::getInstance();
}

// TODO: Fix this being a blocking function or remove it.
void Transmitter::startBit() {
    transmitTime = START_BIT + micros();
    startIR();
    while (transmitTime > micros())
        ;
    transmitTime = START_BIT + micros();
    stopIR();
    while (transmitTime > micros())
        ;
}

void Transmitter::transmitPacket(byte * message, uint32_t size){
    transmitPacket(message, size, PACKET_DUPLICATES);
}
void Transmitter::transmitPacket(byte * message, uint32_t size, uint16_t transmitTimes){
    packet.init(message, size);
    hasFreshPacket = true;
    packetDuplicates = transmitTimes;
}

bool Transmitter::hasPacket(){
    return hasFreshPacket || packetDuplicates > 0;
}

void Transmitter::loop() {
    switch (state) {
        case WAITING_FOR_PACKET:
            if (hasPacket()) {
                state = START;
            }
            break;
        case START:
            if (!hasPacket()) {
                state = WAITING_FOR_PACKET;
                hasFreshPacket = false;
                break;
            }
            if (!lock->startBlast()) return;
            transmitTime = START_BIT;
            lowTime = ONE_BIT; // temp to check start bit timing
            state = TRANSMIT;
            break;
        case SET_DELAY:
            if (packet.getNextBit()) {
                transmitTime = ONE_BIT;
                // lowTime = ZERO_BIT;
                // Serial.print("t1 ");
            } else {
                transmitTime = ZERO_BIT;
                // lowTime = ONE_BIT;
                // Serial.print("t0 ");
            }
            lowTime = ZERO_BIT;
            state = TRANSMIT;
        case TRANSMIT:
            startIR();
            //nextTransmitTime = micros() + transmitTime;
			transmitterTicks = transmitTime;
            state = WAIT;
            nextState = FALL;
            break;
        case WAIT:
            if (transmitterTicks < 2) {
                stopIR();
                state = nextState;
            }
            break;
        case FALL:
            //nextTransmitTime = micros() + lowTime;
			transmitterTicks = lowTime;
            state = WAIT;
            nextState = packet.atEnd() ? END : SET_DELAY;
            break;
        case END:
            packetDuplicates--;
            transmitterTicks = (packetDuplicates > 0) ? START_BIT : START_BIT * 3;
            packet.resetBit();
            hasFreshPacket = false;
            lock->endEmitting();
            state = WAIT;
            nextState = START;
            break;
    }
}
