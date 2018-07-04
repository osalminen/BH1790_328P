/*
 * bh1790main.c
 *
 * Created: 2.7.2018 9:54:12
 *  Author: h17177
 */ 
#include <BH1790GLC.h>
#include <hr_bh1790.h>

#define SAMPLING_CNT_32HZ (32)
static volatile uint8_t    bh1790_cnt_freq = 0;

void bh1790_init(){
	uint16_t ret16 = hr_bh1790_Init(); //Init sensor
	if(ret16 == ERROR_NONE){
		ret16 = hr_bh1790_StartMeasure();
		if (ret16 != ERROR_NONE){
			//Error handling for start measure
		}
		} else {
		//Error handling for init
	}
}
uint8_t  bpm     = 0U;
uint16_t get_bh1790_HR(){
	uint8_t  wearing = 0U;
	uint16_t ret16 = hr_bh1790_Calc(bh1790_cnt_freq);
	if (ret16 == ERROR_NONE) {
		bh1790_cnt_freq++;
		if (bh1790_cnt_freq >= SAMPLING_CNT_32HZ) {
			bh1790_cnt_freq = 0;
			hr_bh1790_GetData(&bpm, &wearing);
			return bpm;
			//Serial.print(bpm, DEC);
			//Serial.print(F(","));
			//Serial.println(wearing, DEC);
		}
		} else {
		//Serial.println(F("Error: hr_bh1790_Calc function"));
		//Serial.print(F("ret16 = "));
		//Serial.println(ret16, HEX);
	}
}