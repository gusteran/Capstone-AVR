#pragma once


// #include "bitcoder.h"
#include "constants.h"
#include "packet.h"
#include "lock.h"

enum TransmitState {
    WAITING_FOR_PACKET,
    START,
    SET_DELAY,
    TRANSMIT,
    WAIT,
    FALL,
    END,
};

class Transmitter {
   public:
    Transmitter();
    void setup();
    void loop();
    void startBit();

    bool canCompute() { return state == WAIT && nextTransmitTime > micros() + 1100; }

    void transmitPacket(byte * message, uint32_t size = PACKET_SIZE);
    void transmitPacket(byte * message, uint32_t size, uint16_t transmitTimes);
    bool hasPacket();

    void enableTransmit() { blockTransmit = false; }
    void disableTransmit() {
        blockTransmit = true;
        state = START;
    }

   private:
    bool hasFreshPacket = false;
    int16_t packetDuplicates = 0;

    TransmissionLock * lock = TransmissionLock::getInstance();
    TransmitState state;
    TransmitState nextState;
    Packet packet;

    uint32_t nextTransmitTime = 0;
    uint32_t transmitTime = 0;
    uint32_t lowTime = 0;
    bool blockTransmit = false;

    uint64_t lastResetTime = 0;

    // Test or debug variables
    bool testSignalStrengths = false;
};