/*
 * lineDet.cpp
 *
 *  Created on: 17-03-2014
 *      Author: jachu
 */

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "lineDet.h"

volatile bool lineDetBuffer[4][LINE_DET_NSAMP_MEAN];
volatile bool lineDetStatus[4];

void lineDetInit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = lineDetPins[0] | lineDetPins[1];
	//gpioInit.GPIO_Pin = GPIO_Pin_12;
	gpioInit.GPIO_Mode = GPIO_Mode_IPU;
	gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(lineDetPorts[0], &gpioInit);
	gpioInit.GPIO_Pin = lineDetPins[2] | lineDetPins[3];
	GPIO_Init(lineDetPorts[2], &gpioInit);
	for(int i = 0; i < 4; i++){
		lineDetStatus[i] = false;
		for(int s = 0; s < LINE_DET_NSAMP_MEAN; s++){
			lineDetBuffer[i][s] = false;
		}
	}
}

void lineDetSys(){
	static uint8_t pos = 0;

	for(int i = 0; i < 4; i++){
		lineDetBuffer[i][pos] = GPIO_ReadInputDataBit(lineDetPorts[i], lineDetPins[i]);
	}
	for(int i = 0; i < 4; i++){
		uint8_t cnt = 0;
		for(int s = 0; s < LINE_DET_NSAMP_MEAN; s++){
			cnt += (lineDetBuffer[i][s] == true ? 1 : 0);
		}
		if(cnt >= LINE_DET_NSAMP_MEAN/2){
			lineDetStatus[i] = true;
		}
		else{
			lineDetStatus[i] = false;
		}
	}

	pos++;
	pos %= LINE_DET_NSAMP_MEAN;
}

bool lineDetCheck(Detector det){
	return lineDetStatus[det];
}
