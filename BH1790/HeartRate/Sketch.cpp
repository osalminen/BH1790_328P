/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

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
#include <Wire.h>
#include <BH1790GLC.h>
#include <FlexiTimer2.h>
#include <Arduino.h>

extern "C" {
#include <hr_bh1790.h>
#include "utility/twi.h"  // from Wire library
}
#define SAMPLING_CNT_32HZ   (32)

BH1790GLC bh1790glc;
volatile bool timer_flg;
void timer_isr(void);

static volatile uint8_t    bh1790_cnt_freq = 0;

// Setup Timer1 for interrupt at 35 Hz @ 16MHz
void timerSetup(void){
	TCCR1A = 0;     //TCCR1A register to 0
	TCCR1B = 0;     //TCCR1B register to 0
	OCR1A = 445; //compare match register to desired timer count
	//turn on CTC mode, CS10 and CS12 bits for 1024 prescaler
	TCCR1B |= (1 << WGM12) | (1 << CS10) | (1 << CS12);
	TIMSK1 |= (1 << OCIE1A); //enable timer compare interrupt
}

ISR (TIMER1_COMPA_vect){
	timer_flg = true;
	//Maybe init measurement here too...
}


int main(){
	/*
		SETUP PHASE
	*/
	uint16_t ret16 = ERROR_NONE;
	uint8_t  bpm     = 0U;
	uint8_t  wearing = 0U;

	timer_flg = false;
	
	init(); //Init 328P
	twi_init(); //Init TWI
	twi_setFrequency(400000); //Set TWI clock
	Serial.begin(9600);
	bh1790_cnt_freq = 0; //Keeping trach of sensor sampling
	ret16 = hr_bh1790_Init(); //Init sensor
	if(ret16 == ERROR_NONE){
		timerSetup(); //Init 35Hz timer
		ret16 = hr_bh1790_StartMeasure();
		if (ret16 != ERROR_NONE){
			//Error handling for start measure
		}
	} else {
		//Error handling for init
	}
	//TODO data to UART
	while(1){

		if (timer_flg) {
			ret16 = hr_bh1790_Calc(bh1790_cnt_freq);
			if (ret16 == ERROR_NONE) {
				bh1790_cnt_freq++;
				if (bh1790_cnt_freq >= SAMPLING_CNT_32HZ) {
					bh1790_cnt_freq = 0;
					hr_bh1790_GetData(&bpm, &wearing);
					Serial.print(bpm, DEC);
					Serial.print(F(","));
					Serial.println(wearing, DEC);
				}
				} else {
				Serial.println(F("Error: hr_bh1790_Calc function"));
				Serial.print(F("ret16 = "));
				Serial.println(ret16, HEX);
			}
			timer_flg = false;
		}
	}
	
}

/*
int8_t bh1790_Write(uint8_t adr, uint8_t *data, uint8_t size)
{
  byte   rc  = 0;
  int8_t ret = 0;
  
  rc = bh1790glc.write(adr, data, size);
  if (rc == 0) {
    ret = BH1790_RC_OK;
  } else {
    ret = BH1790_RC_I2C_ERR;
  }

  return (ret);
}

int8_t bh1790_Read(uint8_t adr, uint8_t *data, uint8_t size)
{
  byte   rc  = 0;
  int8_t ret = 0;

  rc = bh1790glc.read(adr, data, size);
  if (rc == 0) {
    ret = BH1790_RC_OK;
  } else {
    ret = BH1790_RC_I2C_ERR;
  }
  
  return (ret);
}
*/

