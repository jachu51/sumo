/*
 * systick.h
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "stm32f10x_conf.h"

#define PID_freq 50
#define SYS_freq 1000
#define INFO_freq 10
#define ADC_freq 1000
#define LCD_freq 10
#define BUTTONS_freq 50

void sysInit();
void sysDelayMs(unsigned int ms);
void sysDelayMsRet(unsigned int ms);
bool sysIfDelayExp();

uint8_t ftoa(float num, char* buffer, uint8_t dig, uint8_t dec);
uint8_t uitoa(uint32_t num, char* buffer, uint8_t base);
uint8_t atof(float* num, char* buffer);
uint8_t atoui(uint32_t* num, char* buffer);
uint32_t min(uint32_t a, uint32_t b);
uint8_t strcmpLen(const char* str1, const char* str2, uint16_t len);

inline int min(int a, int b){
	return a>b?b:a;
}

inline int max(int a, int b){
	return a<b?b:a;
}

extern "C" {

void SysTick_Handler(void);

}

#endif /* SYSTICK_H_ */
