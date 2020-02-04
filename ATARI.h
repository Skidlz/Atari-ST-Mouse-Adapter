/*
 * ATARI.h
 *
 *  Created on: Jun 10, 2018
 *      Author: Zack
 */

#ifndef ATARI_H_
#define ATARI_H_

//extern uint8_t phase_tic; //flag to move phase
extern signed int X_delta, Y_delta;
signed int X_phase, Y_phase;
void output_phase();
void atari_setup();

#endif /* ATARI_H_ */
