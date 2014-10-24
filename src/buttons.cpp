/*
 * buttons.c
 *
 * Created: 2011-05-18 22:54:01
 *  Author: Jachu
 */
#include "stm32f10x_gpio.h"
#include "sys.h"
#include "buttons.h"

#define THRESHOLD 5
#define THRESHOLD_HOLD 50

volatile uint8_t isPushed[BUTTONS_NUM];
volatile uint8_t isHeld[BUTTONS_NUM];
volatile uint8_t cntPushed[BUTTONS_NUM];
volatile uint8_t wasHeld[BUTTONS_NUM];
//const uint8_t pinNum[BUTTONS_NUM] = {0, 1, 2, 3};
//void (*handlersPushed[BUTTONS_NUM])(void) = {handleButton1Pushed, handleButton2Pushed, handleButton3Pushed, handleButton4Pushed};
//void (*handlersHeld[BUTTONS_NUM])(void) = {handleButton1Held, handleButton2Held, handleButton3Held, handleButton4Held};

void buttonsInit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Mode = GPIO_Mode_IPU;
	gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
	gpioInit.GPIO_Pin = buttonsPins[0] | buttonsPins[1] | buttonsPins[2] | buttonsPins[3];
	GPIO_Init(BUTTONS_PORT, &gpioInit);


	for(int i = 0; i < BUTTONS_NUM; i++){
		cntPushed[i] = 0;
		isPushed[i] = 0;
		isHeld[i] = 0;
		wasHeld[i] = 0;
	}
}

void buttonsSys(){
	for(int i = 0; i < BUTTONS_NUM; i++){
		if(!GPIO_ReadInputDataBit(BUTTONS_PORT, buttonsPins[i])){
			cntPushed[i]++;
		}
		else{
			if(cntPushed[i] > THRESHOLD && wasHeld[i] == 0){
				isPushed[i] = 1;
			}
			wasHeld[i] = 0;
			cntPushed[i] = 0;
		}
		if(cntPushed[i] >= THRESHOLD_HOLD){
			isHeld[i] = 1;
			cntPushed[i] = 0;
			wasHeld[i] = 1;
		}
	}
}
