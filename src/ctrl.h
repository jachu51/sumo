/*
 * ctrl.h
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */

#ifndef CTRL_H_
#define CTRL_H_

#include <stm32f10x_gpio.h>

#define CTRL_RX GPIO_Pin_10
#define CTRL_TX GPIO_Pin_9

void ctrlInit();
int16_t ctrlCheck();
bool ctrlCheckAndExec();
void ctrlSendInfo();
void ctrlSend(char* data, uint16_t len);
uint16_t appendInfo(char* buffer, const char* data, uint16_t len, float num);

#endif /* CTRL_H_ */
