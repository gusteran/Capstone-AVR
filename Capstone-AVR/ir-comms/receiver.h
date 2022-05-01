#pragma once


#include "bitcoder.h"
#include "constants.h"
#include "lock.h"
#include "packet.h"

enum ReceiverState {
    WAITING,
    READING,
    ERROR,
};

class Receiver {
   public:
    static Receiver * getInstance() {
        return &instance;
    }
    // Receiver(Receiver const &);
    // void operator=(Receiver const &);
    void printSignal();
    void setup();
    void loop();
    void handleIR();

    bool incomingPacket() { return state == READING; }
    bool hasPacket();
    Packet getPacket();

	static volatile uint32_t receiverTicks;

   private:
    static Receiver instance;
    Receiver() : packet() {}
    TransmissionLock * lock = TransmissionLock::getInstance();

    ReceiverState state = WAITING;

    Packet_Header lastHeader = {0, 0, 0, 0};
    int signal = 0;
    Packet packet;
    bool hasOutPacket = false;
    Packet outPacket;
    bool buttonPress = false;
    bool waitingForStart = true;
    unsigned long myTime = 0;

    int currentRead = false;
    int lastRead = false;
    #ifdef IR_TRANSMISSION_PRINTS
    int bitReceived = 0;
    #endif

    uint64_t lastResetTime = 0;

    void handleSignal(uint32_t time);
};