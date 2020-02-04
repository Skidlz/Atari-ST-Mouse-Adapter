/*
 * ATARI.c
 *
 *  Created on: Jun 10, 2018
 *      Author: Zack
 */
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "millis.h"
#include "ATARI.h"
#include "PS2.h"

uint8_t phase_tic = 0;
const uint8_t quad_phases[] = { 0b00, 0b01, 0b11, 0b10 };

signed int X_delta, Y_delta = 0;

ISR(TIMER1_COMPA_vect) {
	phase_tic = 1; //set flag
	static uint8_t sub_ms = 0;
	if (sub_ms++ > 2) { //2.5khz/2 = .8ms
		timer1_millis++;
		sub_ms = 0;
	}
}

void output_phase() {
	uint8_t output = quad_phases[X_phase] | (quad_phases[Y_phase] << 2);
	PORTC = (PORTC & 0b11110000) | output;
	DDRC = (DDRC & 0b11110000) | ~output; //open collector
	if (X_delta > 0) {
		if (++X_phase > 3) X_phase = 0;
		X_delta--; //remove one click
	} else if (X_delta < 0) {
		if (--X_phase < 0) X_phase = 3;
		X_delta++; //remove one click
	}

	if (Y_delta / 4 > 0) {
		if (++Y_phase > 3) Y_phase = 0;
		Y_delta--; //remove one click
	} else if (Y_delta < 0) {
		if (--Y_phase < 0) Y_phase = 3;
		Y_delta++; //remove one click
	}
	phase_tic = 0;
}

void atari_setup(){
	//DDRC: MB, RB, LB, XA,XB, YA,YB
	DDRC = 0; //must be input for Atari startup

	TCCR1A = 0; //timer setup
	TCNT1 = 0;

	TCCR0 |= (1 << CS01) | (1 << CS00); // Clock/64

	OCR1A = F_CPU/256/2500; //2.5KHz
	TCCR1B = (1 << WGM12) | (1 << CS12); // CTC /256

	TIMSK |= (1 << OCIE1A); //enable compare interrupt (atmega8)

	millis_setup();
}
