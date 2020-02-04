/*
 * PS2.c
 *
 *  Created on: Jun 10, 2018
 *      Author: Zack
 */
//#define F_CPU 8000000UL
#include <stdint.h>
#define byte uint8_t
#include <avr/io.h>
#include <util/delay.h>
#include "millis.h"
#include "ATARI.h"
#include "PS2.h"

uint8_t read_data = 0;
extern uint8_t phase_tic;

uint8_t wait_l_c(int max_wait) { //wait for low clock
	unsigned long start_ts = millis();
	while (PINB & (1 << ck)) if ((millis() - start_ts) > max_wait) return 1; //error, timeout
	return 0; //didn't time out
}

uint8_t wait_h_c(int max_wait) { //wait for high clock
	unsigned long start_ts = millis();
	while (~PINB & (1 << ck)) {
		if ((millis() - start_ts) > max_wait) return 1; //error, timeout
		if (phase_tic) output_phase(); //output Atari data
	}
	return 0; //didn't time out
}

uint8_t write_bit(uint8_t data) {
	uint8_t error = 0;
	error |= wait_l_c(2); //wait for low clock
	PORTB = (PORTB & ~(1 << da)) | (data & 1); //set data
	error |= wait_h_c(2); //wait for high clock
	return error;
}

uint8_t send_byte(int data) {
	uint8_t error = 0;
	uint8_t parity = 1;
	DDRB = (1 << ck) | (1 << da); //clock, data outputs
	PORTB = (0 << ck) | (1 << da); //hold clock low, inhibit
	_delay_us(150);
	DDRB &= ~(1 << ck); //set clock to input
	PORTB = (1 << ck) | (0 << da); //request to send

	for (int i = 0; i < 8; i++) {
		if (write_bit((data >> i) & 1)) return -1; //write LSB of data, exit on error
		parity ^= ((data >> i) & 1);
	}
	error |= write_bit(parity & 1); //write parity
	error |= write_bit(1); //write stop

	DDRB &= ~((1 << ck) | (1 << da)); //clock, data inputs
	PORTB = (1 << ck) | (1 << da); //pullup
	unsigned long start_ts = millis();
	const int max_wait = 2; //ms
	while (PINB & (1 << da)) if ((millis() - start_ts) > max_wait) return 1; //wait for low data
	error |= wait_l_c(10); //wait for low clock
	start_ts = millis();
	//wait for high clock & data
	while ((PINB & ((1 << ck) | (1 << da))) != ((1 << ck) | (1 << da)))
		if ((millis() - start_ts) > max_wait)
			return 1;
	return error;
}

int rec_byte(uint8_t retry) {
	if(retry == 0)send_byte(RESEND); //request resend

	int read_data = 0;
	uint8_t error = 0;
	uint8_t parity = 1;
	uint8_t data_bit;
	//TIMSK &= ~(1 << OCIE1A); //disable interrupts
	//if(wait_l_c(50))return -1;//wait up to 50ms for a uint8_t, return error on timeout
	unsigned long start_ts = millis();
	while (PINB & (1 << ck)) {
		if ((millis() - start_ts) > 50) return -1; //error, timeout
		if (phase_tic) output_phase();
	}

	for (int bits = 0; bits < 9; bits++) {
		error |= wait_l_c(2); //spin while clock high
		data_bit = PINB & (1 << da);
		//TIMSK |= (1 << OCIE1A); //enable compare interrupt
		read_data >>= 1;
		read_data |= data_bit << 7;
		parity ^= data_bit;
		error |= wait_h_c(2); //spin while clock low
		//TIMSK &= ~(1 << OCIE1A); //disable interrupts
		if (error) return (retry) ? rec_byte(0):-1; //retry or error out
	}
	error |= wait_l_c(2); //spin for parity
	if (parity != (PINB & (1 << da)))
		error = 1; //check parity bit
	//TIMSK |= (1 << OCIE1A); //enable compare interrupt
	error |= wait_h_c(2); //spin while clock low
	//TIMSK &= ~(1 << OCIE1A); //disable interrupts
	error |= wait_l_c(2); //spin stop bit
	//TIMSK |= (1 << OCIE1A); //enable compare interrupt
	error |= wait_h_c(2); //spin while clock low

	return (error) ? ((retry) ? rec_byte(0):-1) : read_data; //return error/retry or data
}

uint8_t PS2_setup(){
	DDRB |= (1<<da) | (1<<ck); //clock, data
	PORTB |= (1<<da); //data high
	PORTB &= ~(1<<ck); //hold clock low, inhibit
	uint8_t error = 0;
	_delay_ms(500); //wait for mouse/Atari to start up

	send_byte(RESET); //reset
	error |= (rec_byte(1) != ACK); //read ack

	wait_l_c(1000); //wait up to 1 second for reset
	rec_byte(1); //AA?
	error |= (rec_byte(1) != 0); //read device ID 0=mouse

	/*send_byte(0xf3); //set sample rate
	 error |= (rec_byte() != ACK); //read ack
	 send_byte(100); //set sample rate 100/sec
	 error |= (rec_byte() != ACK); //read ack
	 */
	send_byte(0xE8); //set res
	error |= (rec_byte(1) != ACK); //read ack
	send_byte(0x03); //set res 8count/mm
	error |= (rec_byte(1) != ACK); //read ack

	send_byte(0xE7); //set scale 2:1
	error |= (rec_byte(1) != ACK); //read ack


	send_byte(0xf4); //enable
	error |= (rec_byte(1) != ACK); //read ack

	return error;
}

struct move_pact rec_move_pact(){
	struct move_pact packet = {0,0,0,0};

	int mouse_byte = rec_byte(1);
	if (mouse_byte == -1) {
		packet.error = 1;
		return packet;
	}

	//Y overflow,  X overflow,  Y sign bit,  X sign bit,  Always 1,  Middle Btn,  Right Btn, Left Btn
	if (mouse_byte & 0b00001000) { //may be the starting uint8_t of a data packet
		packet.buttons = mouse_byte & 0b111;

		packet.X = rec_byte(1);
		if (packet.X == -1) {
			packet.error = 1;
			return packet;
		}

		packet.Y = rec_byte(1);
		if (packet.Y == -1 ){
			packet.error = 1;
			return packet;
		}

		if (mouse_byte & (1 << X_SIG)) packet.X |= 0xff00; //x sign bit
		if (mouse_byte & (1 << Y_SIG)) packet.Y |= 0xff00; //y sign bit
	}
	return packet;
}
