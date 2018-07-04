/*****************************************************************************
  HeartRate.ino

 Copyright (c) 2016 ROHM Co.,Ltd.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
******************************************************************************/
#include <Arduino.h>

extern "C" {
#include "utility/twi.h"  // from Wire library
#include <bh1790main.h>
}


volatile bool timer_flg;

ISR (TIMER1_COMPA_vect){
	timer_flg = true;
}

// Setup Timer1 for interrupt at 35 Hz @ 16MHz
void timerSetup(void){
	TCCR1A = 0;     //TCCR1A register to 0
	TCCR1B = 0;     //TCCR1B register to 0
	OCR1A = 445; //compare match register to desired timer count
	//turn on CTC mode, CS10 and CS12 bits for 1024 prescaler
	TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12);
	TIMSK1 |= (1 << OCIE1A); //enable timer compare interrupt
}

int main(){
	/*
		SETUP PHASE
	*/
	uint8_t  mbpm     = 0U;

	timer_flg = false;
	
	init(); //Init 328P
	twi_init(); //Init TWI
	twi_setFrequency(400000); //Set TWI clock
	Serial.begin(9600);
	bh1790_init();
	timerSetup();
	//TODO data to UART
	while(1){

		if (timer_flg) {
			mbpm = get_bh1790_HR();
			Serial.println(mbpm, DEC);
			timer_flg = false;
		}
	}
	
}

