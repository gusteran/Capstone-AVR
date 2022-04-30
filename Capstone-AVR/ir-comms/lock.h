#ifndef TRANSMISSION_LOCK
#define TRANSMISSION_LOCK
#include "constants.h"

enum CommState {
    PASSIVE,
    ACTIVE_LISTENING,
    START_BLAST,
    CRASH,
    TALK,
};

class TransmissionLock {
   public:
    static TransmissionLock * getInstance() {
        if (TransmissionLock::instance == nullptr) {
            instance = new TransmissionLock();
        }
        return instance;
    }

    /**
     * Indicates that the robot has completed receiving the packet.
     */
    void endListening();

    /**
     * Indicates that the robot has stop emitting communications.
     */
    void endEmitting();

    /**
     * Alerts the lock that a communication is being received.
     */
    bool receiveComms();

    /**
     * Tries to begin broadcasting. Returns true if
     * the robot can begin communicating.
     */
    bool startBlast();

    CommState getState() { return commState; }

   private:
    static TransmissionLock* instance;
    TransmissionLock(){
        commState = CommState::PASSIVE;
        lockTime = 0;
    }
    CommState commState;
    uint64_t lockTime;
    bool holdForReceive();

};

#endif