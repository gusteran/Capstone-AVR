/*
 * main.c
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
static volatile unsigned long microseconds;
volatile bool LED_ON = false;


#define IR_LED PB7

// TODO: Implement the micros function for myself
inline static unsigned long micros() { return microseconds; }

inline void setLED() {
    PORTB |= 1 << IR_LED;
}

inline void unsetLED() {
    PORTB &= ~(1 << IR_LED);
}
inline void stopIR() {
    TCCR2 = 0x00;
}

inline void startIR() {
    // Sets to CTC
    TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS20);
}

inline void setupIR() {
    //cli();
    OCR2 = 110;
    DDRB = (1 << IR_LED);
    TCCR2 = (1 << WGM21) | (1 << COM20) | (1 << CS20);
    //TIMSK = (1 << OCIE2);
    TCNT2 = 0;
}

inline void setupTimer() {
	OCR0 = 100;
	TCNT0 = 0;
	TCCR0 = (1 << WGM01) | (1 << CS01);
	TIMSK = (1 << TOIE0);
}

ISR(TIMER0_OVF_vect) {
	//cli();
    PORTB ^= 1 << IR_LED;
	//microseconds++;
	//sei();
}

ISR(TIMER0_COMP_vect) {
	PORTB ^= 1 << IR_LED;
	//microseconds++;
}

int main(void) {
	microseconds = 0;
	unsetLED();
	_delay_ms(100);
	setLED();
	_delay_ms(500);
	unsetLED();
    setupIR();
    setupTimer();
	_delay_ms(500);
	stopIR();
	sei();
	DDRB = (1 << IR_LED);
    while (1) {
		//cli();
		//if(microseconds > 10){
			//PORTB ^= (1 << IR_LED);
			//microseconds = 0;
		//}
		//sei();
    }
    return 1;
}
