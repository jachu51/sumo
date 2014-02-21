/*
 * led.h
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */

#ifndef LED_H_
#define LED_H_

#include <stm32f10x_gpio.h>

#define LED1 GPIO_Pin_8
#define LED2 GPIO_Pin_9
#define LED3 GPIO_Pin_10
#define LED4 GPIO_Pin_11

void ledInit();
void ledSet(uint16_t);
void ledReset(uint16_t);
void ledToggle(uint16_t);


#endif /* LED_H_ */
