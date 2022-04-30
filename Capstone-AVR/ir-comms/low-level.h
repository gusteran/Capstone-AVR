/*
 * low_level.h
 *
 * Created: 4/29/2022 7:55:39 PM
 *  Author: Gus
 */ 


#ifndef LOW_LEVEL_H_
#define LOW_LEVEL_H_

#include <stdlib.h>
#include <avr/io.h>

#define byte uint8_t

//#define uint64_t unsigned long long //May need to remove all uint64_ts

void setupIR();
void startIR();
void stopIR();

unsigned long micros();
void setupTimer();

void setupDCMotors();
void enableMotors();
void disableMotors();
void setMotorSpeeds(int left, int right);




#endif /* LOW-LEVEL_H_ */