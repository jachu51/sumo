/*
 * buttons.h
 *
 * Created: 2011-05-18 22:37:40
 *  Author: Jachu
 */ 


#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "stm32f10x_gpio.h"

#define UP_BUT 0
#define DOWN_BUT 2
#define RIGHT_BUT 3
#define LEFT_BUT 1

#define BUTTONS_NUM 4
#define BUTTONS_PORT GPIOC
static const uint16_t buttonsPins[] = {GPIO_Pin_8,
										GPIO_Pin_9,
										GPIO_Pin_10,
										GPIO_Pin_11};

typedef void (*voidFunc)(void);

extern volatile uint8_t isPushed[BUTTONS_NUM];
extern volatile uint8_t isHeld[BUTTONS_NUM];
//extern void (*handlersPushed[BUTTONS_NUM])(void);
//extern void (*handlersHeld[BUTTONS_NUM])(void);

/*void handleButton1Pushed(void);
void handleButton2Pushed(void);
void handleButton3Pushed(void);
void handleButton4Pushed(void);
void handleButton5Pushed(void);
void handleButton1Held(void);
void handleButton2Held(void);
void handleButton3Held(void);
void handleButton4Held(void);
void handleButton5Held(void);*/
void buttonsInit(void);
void buttonsSys();

#endif /* BUTTONS_H_ */
