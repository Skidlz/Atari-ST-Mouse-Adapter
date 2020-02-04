/*
 * PS2.h
 *
 *  Created on: Jun 10, 2018
 *      Author: Zack
 */

#ifndef PS2_H_
#define PS2_H_

#define ck 1
#define da 0
#define X_SIG 4
#define Y_SIG 5
#define ACK 0xFA
#define RESET 0xFF
#define RESEND 0xFE

struct move_pact{
	uint8_t buttons;
	int X;
	int Y;
	uint8_t error;
};

uint8_t send_byte(int data);
int rec_byte(uint8_t retry);
uint8_t wait_l_c(int max_wait);
uint8_t PS2_setup();
struct move_pact rec_move_pact();

#endif /* PS2_H_ */
