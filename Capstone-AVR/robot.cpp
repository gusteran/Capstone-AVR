#include "robot.h"

uint8_t Robot::ROBOT_ID;

void Robot::setupRobotID() {
    // Uncomment this line to set the id to the second number.
    // EEPROM.write(EE_ADDR_ID, 10);
    byte id = eeprom_read_byte(EE_ADDR_ID);
    ROBOT_ID = id;
}
void Robot::setup() {
    setupRobotID();
    transmitter.setup();
    receiver->setup();
	setupTimer();
	setupDCMotors();
	setupIR();
	stopIR();
}

void Robot::spin(int speed) {
    setMotorSpeeds(speed, -speed);
}

void Robot::move(int speed) {
    setMotorSpeeds(speed, speed);
}

inline void Robot::generatePacket() {
    RobotFields robotData = {};
    robotData.state = state;
	// TODO: Fix packet data for avr
    robotData.battery = 0;//readBatteryMillivolts();
    robotData.angle = 0;//turnAngle / turnAngle1;

    byte* data = (byte*)&robotData;
    transmitter.transmitPacket(data, sizeof(robotData), 2);
}

inline RobotFields Robot::decryptPacket(byte* message) {
    RobotFields* fields;
    switch ((PacketType)message[0]) {
        case FIELDS:
            fields = (RobotFields*)message;
            return *fields;
        default:
            return {};
    }
}

// to align with other robot
//  float otherAngle = 0;
RobotFields otherRobot;
int angle;
int desired = 90;
int diff;

const int TIME_BETWEEN_SPEAKING = 1000;
const int Kp = 25;

void Robot::stateMachine() {
    //if (lock->getState() == PASSIVE || transmitter.canCompute()) turnSensorUpdate();
    switch (state) {
        case SPINNING:
            // if (lock->getState() == PASSIVE) turnSensorUpdate();
            transmitter.loop();
            receiver->loop();
            // if (millis() > nextTime) {
            //     nextTime = millis() + 500;
            //     Serial.print("Angle ");
            //     Serial.println(turnAngle / turnAngle1);
            // }
            if (!transmitter.hasPacket() && millis() > nextTime) {
                generatePacket();
                nextTime = millis() + TIME_BETWEEN_SPEAKING * 2.5;
            }
            if (lock->getState() == PASSIVE) {
                spin((ROBOT_ID == 10) ? 100 : 130);
            } else {
                spin(0);
            }
            if (receiver->incomingPacket()) {
                setMotorSpeeds(0, 0);
                state = COMMUNICATING;
            }
            break;
        case COMMUNICATING:
            // if (lock->getState() == PASSIVE) turnSensorUpdate();
            transmitter.loop();
            receiver->loop();
            if (!transmitter.hasPacket() && millis() > nextTime) {
                generatePacket();
                nextTime = millis() + TIME_BETWEEN_SPEAKING;
            }
            if (receiver->hasPacket()) {
                recv_pack = receiver->getPacket();
                RobotFields fields = decryptPacket(recv_pack.getMessage());
                if (fields.state >= COMMUNICATING) {
                    state = ALIGNING;
                    otherRobot = fields;
                }
            }
            break;
        case ALIGNING:
            // TODO: update angle
            diff = angle - desired;
            spin((diff > 0) ? 120 : -120);
            // spin((diff * Kp > 120) ? 120 : (diff * Kp < -120) ? -120
            //                                                   : diff * Kp);
            if (millis() > nextTime) {
                nextTime = millis() + 500;
            }
            if (diff < 2 && diff > -2) {
                spin(0);
                state = PREPARE_TO_MOVE;
            }
            break;
        case PREPARE_TO_MOVE:
            transmitter.loop();
            receiver->loop();
            if (!transmitter.hasPacket() && millis() > nextTime) {
                generatePacket();
                nextTime = millis() + TIME_BETWEEN_SPEAKING;
            }
            if (receiver->hasPacket()) {
                recv_pack = receiver->getPacket();
                RobotFields fields = decryptPacket(recv_pack.getMessage());
                if (fields.state >= PREPARE_TO_MOVE) {
                    state = MOVING;
                    otherRobot = fields;
                    move(FORWARD_SPEED);
                    nextTime = millis() + 2000;
                }
            }
            break;
        case MOVING:
            // turnSensorUpdate();
            if (millis() > nextTime) {
                setMotorSpeeds(0, 0);
                state = FINISH;
            }
            break;
        case FINISH:
            transmitter.loop();
            receiver->loop();
            if (!transmitter.hasPacket() && millis() > nextTime) {
                generatePacket();
                nextTime = millis() + TIME_BETWEEN_SPEAKING;
            }
            break;
    }
}

bool runningMotors = false;
int motorSpeed = 225;
byte * message = (byte *) "Right";
byte fullPacket[32] = "This string is the entire buff!";
bool send = true;

void Robot::loop() {
	if(send && millis() > 2000 && !transmitter.hasPacket()){
		transmitter.transmitPacket(fullPacket, sizeof(fullPacket));
		send = false;
	}
    transmitter.loop();
#ifdef RECEIVER_ENABLE
if (runningMotors && millis() > nextTime) {
	setMotorSpeeds(0, 0);
	runningMotors = false;
}
    receiver->loop();
    if (receiver->hasPacket()) {
        recv_pack = receiver->getPacket();
        if (recv_pack.getMessageString()[0] == 'L') {
            // move(FORWARD_SPEED);
            setMotorSpeeds(motorSpeed, 0);
            nextTime = millis() + 500;
            runningMotors = true;
        } else if (recv_pack.getMessageString()[0] == 'R') {
			setMotorSpeeds(0, motorSpeed);
            nextTime = millis() + 500;
            runningMotors = true;
        } else if (recv_pack.getMessageString()[0] == 'F') {
			setMotorSpeeds(motorSpeed, motorSpeed);
			nextTime = millis() + 500;
			runningMotors = true;
        }
    }
#endif
}
