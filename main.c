//#define F_CPU 8000000UL
#include <stdint.h>
//#include <stdlib.h>
#define byte uint8_t
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "millis.h"
#include "PS2.h"
#include "ATARI.h"

extern signed int X_delta, Y_delta;
extern uint8_t phase_tic;

uint8_t parse_mouse() {
	struct move_pact packet = rec_move_pact();
	if(packet.error) return 1;

	PORTC = (PORTC & 0b1111) | ((~packet.buttons & 0b011) << 4); //set r&l button output
	DDRC = (DDRC & 0b1111) | ((packet.buttons & 0b111) << 4);   //open collector

	X_delta += packet.X;
	Y_delta += packet.Y;

	return 0;
}

void setup() {
	//DDRB: PS2 Clock, Data
	//DDRC: Atari MB, RB, LB, XA,XB, YA,YB
	PS2_setup();
	atari_setup();
}

int main() {
	setup();
	while (1) {
		while (PINB & (1 << ck)) { //wait for start of PS2 data
			if (phase_tic) output_phase(); //output Atari data
		}
		parse_mouse();
	}
}
