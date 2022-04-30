/*
 * main.cpp
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
static volatile unsigned long ticks = 0;
static volatile unsigned long kiloTicks = 0;
const int TICKS_TO_MICROSECONDS = 15;
volatile bool LED_ON = false;

#define IR_LED PB7

// TODO: Implement the micros function for myself
unsigned long micros() { return TICKS_TO_MICROSECONDS * (ticks + 1000 * kiloTicks); }

void setLED() {
    PORTB |= 1 << IR_LED;
}

void unsetLED() {
    PORTB &= ~(1 << IR_LED);
}
void stopIR() {
    TCCR2 = 0x00;
}

void startIR() {
    // Sets to CTC
    TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS20);
}

inline void setupIR() {
    // cli();
    OCR2 = 110;
    DDRB = (1 << IR_LED);
    TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS20);
    // TIMSK = (1 << OCIE2);
    TCNT2 = 0;
}

void setupTimer() {
    OCR0 = 10;
    TCNT0 = 0;
    TCCR0 = (1 << WGM01) | (1 << CS00);
    TIMSK = (1 << OCIE0);
}

// ISR(TIMER0_OVF_vect) {
////cli();
// PORTB |= 1 << IR_LED;
////microseconds++;
////sei();
//}

ISR(TIMER0_COMP_vect) {
    // PORTB ^= 1 << IR_LED;
    ticks++;
    if (ticks >= 1000) {
        ticks = 0;
        kiloTicks++;
    }
}

// define a drive variable for the output compare register to turn on when reached
byte drive = 0;

#define LEFT_DC PE3
#define RIGHT_DC PE4

#define LEFT_MOTOR PE6
#define RIGHT_MOTOR PE7

#define MOTOR_ENABLE PG3

void setupDC() {
    // pre scaler set to (1)
    TCCR3B = (1 << WGM32) | (1 << CS30);

    // pwm ramp up only mode (count TCNT3 up to OCR3A then stop); 8-bit
    TCCR3A = (1 << WGM30) | (1 << COM3A1) | (1 << COM3B1);

    // define counter pre-set value (this is what we start counting up from)
    TCNT3 = 0;

    // define value to count up to (no pre scaler used so can be from 1 to 254)
    OCR3A = 200;
    OCR3B = 200;

    // the closer TCNT3 is set to 200, the closer we are to 100% duty cycle

    // set pins 5 and 6 (PE3 and PE4) only as output
    // DDRE= 0b00011000;
    DDRE |= (1 << LEFT_DC);
    DDRE |= (1 << RIGHT_DC);
}

void enableMotors() {
    // PORTE |= (1 << LEFT_MOTOR );
    // PORTE |= (1 << RIGHT_MOTOR);
    PORTG |= (1 << MOTOR_ENABLE);
}

void disableMotors() {
    // PORTE &= ~(1 << LEFT_MOTOR );
    // PORTE &= ~(1 << RIGHT_MOTOR);
    PORTG &= ~(1 << MOTOR_ENABLE);
}

void setMotorSpeeds(int left, int right) {
    left = (left < 0) ? 0 : (left > 255) ? 255
                                         : left;
    right = (left < 0) ? 0 : (right > 255) ? 255
                                           : right;
    OCR3A = left;
    OCR3B = right;
}

int main(void) {
    unsigned long nextTime = 0;
    //unsetLED();
    //_delay_ms(100);
    //setLED();
    //_delay_ms(500);
    //unsetLED();
    //_delay_ms(1000);
    //setupIR();
    //_delay_ms(500);
    //stopIR();
    sei();
    DDRB = (1 << IR_LED);
    setupTimer();
    setupDC();
    // int speed = 150;
    bool waiting = false;
    unsigned long delay = 1000000;
    while (1) {
        if (waiting && micros() > nextTime) {
            waiting = false;
        } else {
            PORTB ^= (1 << IR_LED);
            nextTime = micros() + delay;
        }

        // cli();
        // if(microseconds > 10){
        // PORTB ^= (1 << IR_LED);
        // microseconds = 0;
        //}
        // sei();

        // Motor Test
        // enableMotors();
        // setMotorSpeeds(speed, speed);
        //_delay_ms(2000);
        // disableMotors();
        // speed += 15;
        //_delay_ms(500);
    }
    return 1;
}
