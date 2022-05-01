#include "receiver.h"

Receiver Receiver::instance;
volatile uint32_t Receiver::receiverTicks = 0;

void Receiver::setup() {
	DDRF &= ~(1 << FRONT_IR);
 DDRF &= ~(1 << BACK_IR);
DDRF &= ~(1 << LEFT_IR);
DDRF &= ~(1 << RIGHT_IR);
    lock = TransmissionLock::getInstance();
}

bool Receiver::hasPacket() {
    Packet_Header header = outPacket.getHeader();
    bool out = hasOutPacket &&
            //    outPacket.checkPacketValid() &&
               (header.packetNumber != lastHeader.packetNumber || header.source != lastHeader.source);
    lastHeader = header;
    hasOutPacket = false;
    return out;
}

Packet Receiver::getPacket() {
    hasOutPacket = false;
    return outPacket;
}

void Receiver::loop() {
	// TODO: Check that the IR reading works
    currentRead = PINF & (1 << FRONT_IR);

    if (lastRead != currentRead) {
        uint32_t time = receiverTicks;
        receiverTicks = 0;

        bool falling = currentRead;

        if (falling && lock->receiveComms()) {
            handleSignal(time);
        }

        lastRead = currentRead;
    } else if (lock->getState() == ACTIVE_LISTENING && receiverTicks > START_BIT * 3) {
        lock->endListening();
        state = WAITING;
    }
}

void Receiver::handleSignal(uint32_t time) {
    if (time > START_BIT - OFFSET * 4 && time < START_BIT + OFFSET) {
        state = READING;
        packet = Packet();
        return;
    }
    switch (state) {
        case WAITING:
            // if (time > START_BIT - OFFSET && time < START_BIT + OFFSET) {
            //     state = READING;
            //     packet = Packet();
            // } else {
            //     // state = ERROR;
            // }
            break;
        case READING:
            // Reads the bit from the signal.
            if (time > ZERO_BIT - OFFSET && time < ZERO_BIT + OFFSET) {
                packet.setNextBit(0);
            } else if (time > ONE_BIT - OFFSET && time < ONE_BIT + OFFSET) {
                packet.setNextBit(1);
            } 
            else {
                state = ERROR;
                // state = WAITING;
            }

            // Prints the buffer when the buffer is full.
            if (packet.atEnd()) {
                outPacket = packet;
                hasOutPacket = true;
                packet.resetBit();
                lock->endListening();
                state = WAITING;
            }

            break;
        case ERROR:
            state = WAITING;
            // lock->endListening();
            break;
    }
}

