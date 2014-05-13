/*
 * sys.c
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */
#include "sys.h"
#include "misc.h"
#include "motor.h"
#include "ctrl.h"
#include "led.h"
#include "adc.h"
#include "lcd.h"
#include "buttons.h"
#include "lineDet.h"

volatile unsigned int del;
volatile bool lcdEnable;

void sysInit(){
	lcdEnable = false;
	ledInit();
	lineDetInit();
	//ctrlInit();
	motorInit(20, 2, 0, 64*19);
	adcInit();
	buttonsInit();
	SysTick_Config(SystemCoreClock / SYS_freq);
	LcdInit();
	lcdEnable = true;
}

void sysDelayMs(unsigned int ms){
	del = ms;
	while(del > 0);
}

void sysDelayMsRet(unsigned int ms){
	del = ms;
}

bool sysIfDelayExp(){
	return del == 0;
}

uint8_t ftoa(float num, char* buffer, uint8_t dig, uint8_t dec){
	uint8_t pos = 0;
	if(num < 0){
		buffer[pos++] = '-';
		num = -num;
	}
	pos += uitoa(num, buffer + pos, 10);
	int decDig = min(dig - pos, dec);
	if(decDig > 0){
		buffer[pos++] = '.';
	}
	for(uint8_t i = 0; i < decDig; i++){
		num *= 10;
		buffer[pos++] = (char)((((int)num) % 10) + 0x30);
	}
	return pos;
}

uint8_t uitoa(uint32_t num, char* buffer, uint8_t base){
	uint8_t pos = 0;
	if(num == 0){
		buffer[pos++] = '0';
		return pos;
	}
	while(num > 0){
		buffer[pos++] = (char)(num % base + 0x30);
		num /= base;
	}
	for(uint8_t i = 0; i < pos/2; i++){
		char tmp = buffer[i];
		buffer[i] = buffer[pos - i - 1];
		buffer[pos - i - 1] = tmp;
	}
	return pos;
}

uint8_t atof(float* num, char* buffer){
	uint8_t pos = 0;
	uint32_t tmp;
	int8_t mul = 1;
	if(buffer[pos] == '-'){
		mul = -1;
		pos++;
	}
	pos += atoui(&tmp, buffer + pos);
	*num = tmp;
	if(buffer[pos] == '.'){
		pos++;
		uint32_t pow = 1;
		while(0x30 <= buffer[pos] && buffer[pos] < 0x3a){
			pow *= 10;
			*num += (float)(buffer[pos++] - 0x30)/pow;
		}
	}
	*num *= mul;
	return pos;
}

uint8_t atoui(uint32_t* num, char* buffer){
	uint8_t pos = 0;
	*num = 0;
	while(0x30 <= buffer[pos] && buffer[pos] < 0x3a){
		*num *= 10;
		*num += (buffer[pos++] - 0x30);
	}
	return pos;
}

uint32_t min(uint32_t a, uint32_t b){
	return a <= b ? a : b;
}


uint8_t strcmpLen(const char* str1, const char* str2, uint16_t len){
	for(uint16_t i = 0; i < len; i++){
		if(str1[i] != str2[i]){
			return 1;
		}
	}
	return 0;
}

extern "C" {

void SysTick_Handler(void){
	if(del != 0){
		del--;
	}
	static uint16_t cnt = 0;
	/*if(cnt % (uint16_t)(SYS_freq/INFO_freq) == 0){
		ctrlSendInfo();
	}*/
	if(cnt % (uint16_t)(SYS_freq/PID_freq) == 1){
		motorPID(Left);
		motorPID(Right);
	}
	if(cnt % (uint16_t)(SYS_freq/ADC_freq) == 0){
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	}
	if(cnt % (uint16_t)(SYS_freq/BUTTONS_freq) == 3){
		buttonsSys();
	}
	if(cnt % (uint16_t)(SYS_freq/PID_freq) == 4 && lcdEnable == true){
		LcdUpdate();
	}

	cnt++;
	cnt %= SYS_freq;
}

}
