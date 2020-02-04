/*
 * millis.c
 *
 *  Created on: Oct 20, 2014
 *      Author: Owner
 */
// http://www.adnbr.co.uk/articles/counting-milliseconds

#include <util/atomic.h>
#include <avr/interrupt.h>

// Calculate the value needed for
// the CTC match value in OCR1A.
#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8)

volatile unsigned long timer1_millis;
/*
ISR (TIMER1_COMPA_vect) {
    timer1_millis++;
}
*/

unsigned long millis () {
    unsigned long millis_return;
    // Ensure this cannot be disrupted
    ATOMIC_BLOCK(ATOMIC_FORCEON){
        millis_return = timer1_millis;
    }
    return millis_return;
}

void millis_setup(){
	/*
    TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, Clock/8
    OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
    OCR1AL = CTC_MATCH_OVERFLOW;
    //TIMSK1 |= (1 << OCIE1A); // Enable the compare match interrupt
    TIMSK |= (1 << OCIE1A); // Enable the compare match interrupt
    */
}
