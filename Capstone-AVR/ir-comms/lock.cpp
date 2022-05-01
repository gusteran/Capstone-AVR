#include "lock.h"

TransmissionLock TransmissionLock::instance;

void TransmissionLock::endListening() {
    if (commState == ACTIVE_LISTENING) {
        commState = PASSIVE;
    }
}

void TransmissionLock::endEmitting() {
    if (commState == TALK) {
        commState = PASSIVE;
    }
}

bool TransmissionLock::receiveComms() {
    if(commState == ACTIVE_LISTENING) {
        return true;
    }
    if (commState == PASSIVE) {
        commState = ACTIVE_LISTENING;
        return true;
    } 
    // if (state == TALK) {
    //     crash();
    //     return false;
    // }
    return false;
}

bool TransmissionLock::startBlast() {
    if (commState == PASSIVE){
        commState = TALK;
        return true;
    }
    return false;
}
