#pragma once

#include <avr/eeprom.h>
#include <util/delay.h>

#include "ir-comms/receiver.h"
#include "ir-comms/transmitter.h"
#include "ir-comms/constants.h"

enum RobotState : uint8_t {
    SPINNING,
    COMMUNICATING,
    ALIGNING,
    PREPARE_TO_MOVE,
    MOVING,
    FINISH,
};

enum PacketType : uint8_t {
    FIELDS,
    STRING_MESSAGE,
};

struct RobotFields {
    PacketType packetType = FIELDS;
    RobotState state;
    uint16_t battery;
    uint16_t angle;
};

class Robot {
   public:
    void setupRobotID();
    void setup();
    void loop();
    void stateMachine();
    void spin(int speed);
    void move(int speed);
    static uint8_t ROBOT_ID;

   private:
    inline void generatePacket();
    inline RobotFields decryptPacket(byte *message);
	
    Receiver *receiver = Receiver::getInstance();
    Transmitter transmitter;
    RobotState state = SPINNING;
    // RobotState nextState = SPINNING;

    TransmissionLock *lock;
    bool receiving = true;
    Packet recv_pack;
    uint64_t nextTime = 0;
    int FORWARD_SPEED = 150;
    int SPIN_SPEED = 1000;

};