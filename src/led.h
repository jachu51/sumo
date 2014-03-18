/*
 * led.h
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */

#ifndef LED_H_
#define LED_H_

#include <stm32f10x_gpio.h>

static const uint16_t ledPins[] = {	GPIO_Pin_8,
										GPIO_Pin_9,
										GPIO_Pin_10,
										GPIO_Pin_11};

void ledInit();
void ledSet(uint16_t);
void ledReset(uint16_t);
void ledToggle(uint16_t);


#endif /* LED_H_ */
