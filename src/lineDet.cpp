/*
 * lineDet.cpp
 *
 *  Created on: 17-03-2014
 *      Author: jachu
 */

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "lineDet.h"

void lineDetInit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = lineDetPins[0] | lineDetPins[1];
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(lineDetPorts[0], &gpioInit);
	gpioInit.GPIO_Pin = lineDetPins[2] | lineDetPins[3];
	GPIO_Init(lineDetPorts[2], &gpioInit);
}

bool lineDetCheck(Detector det){
	return GPIO_ReadInputDataBit(lineDetPorts[det], lineDetPins[det]);
}
