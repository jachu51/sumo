/*
 * led.c
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */
#include "led.h"
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

void ledInit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = LED1 | LED2 | LED3 | LED4;
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &gpioInit);
}

void ledSet(uint16_t mask){
	GPIO_ResetBits(GPIOB, mask);
}

void ledReset(uint16_t mask){
	GPIO_SetBits(GPIOB, mask);
}

void ledToggle(uint16_t mask){
	uint16_t state = GPIO_ReadOutputData(GPIOB);
	state ^= (0xffff & mask);
	GPIO_Write(GPIOB, state);
}

