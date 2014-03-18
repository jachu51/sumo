/*
 * ctrl.c
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */
#include "ctrl.h"
#include "motor.h"
#include "sys.h"
#include "adc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include <stdio.h>
#include <string.h>

#define SIZE 128
#define DMA_Channel_RX DMA1_Channel5
#define DMA_Channel_TX DMA1_Channel4

const char iRpm[] = "I RPM ";
const char iCte[] = "I CTE ";
const char iCur[] = "I CUR ";
const char iMotV[] = "I VMOT ";
const char mV[] = " V ";
const char mP[] = " P ";
const char mR[] = " R ";
const char cStartV[] = " START VEL";
const char cStartP[] = " START POS";
const char cStop[] = " STOP";
const char cShutdown[] = " SHUTDOWN";

char ctrlBuffer[SIZE];
char dmaBuffer[SIZE];
char ctrlPos;

void ctrlInit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = CTRL_TX;
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpioInit);

	gpioInit.GPIO_Pin = CTRL_RX;
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpioInit);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_InitTypeDef dmaInit;
	dmaInit.DMA_BufferSize = SIZE;
	dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
	dmaInit.DMA_M2M = DMA_M2M_Disable;
	dmaInit.DMA_MemoryBaseAddr = (uint32_t)(ctrlBuffer);
	dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmaInit.DMA_Mode = DMA_Mode_Circular;
	dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->DR);
	dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaInit.DMA_Priority = DMA_Priority_Low;
	DMA_Init(DMA_Channel_RX, &dmaInit);
	ctrlPos = 0;

	dmaInit.DMA_BufferSize = 0;
	dmaInit.DMA_DIR = DMA_DIR_PeripheralDST;
	dmaInit.DMA_MemoryBaseAddr = (uint32_t)dmaBuffer;
	dmaInit.DMA_Mode = DMA_Mode_Normal;
	DMA_Init(DMA_Channel_TX, &dmaInit);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef usartInit;
	usartInit.USART_BaudRate = 115200;
	usartInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usartInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usartInit.USART_Parity = USART_Parity_Even;
	usartInit.USART_StopBits = USART_StopBits_1;
	usartInit.USART_WordLength = USART_WordLength_9b;
	USART_Init(USART1, &usartInit);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);

	DMA_Cmd(DMA_Channel_RX, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

int16_t ctrlCheck(){
	int16_t bufEnd = SIZE - DMA_GetCurrDataCounter(DMA_Channel_RX);
	uint16_t curPos = ctrlPos;
	while(curPos != bufEnd){
		if(ctrlBuffer[curPos] == '\n'){
			return curPos;
		}
		curPos++;
		curPos %= SIZE;
	}
	return -1;
}

bool ctrlCheckAndExec(){
	int16_t curPos = ctrlCheck();
	if(curPos < 0){
		return false;
	}
	char tmpBuf[SIZE/4];
	int len = 0;
	for(int i = 0; (i + ctrlPos) % SIZE != curPos; i++){
		tmpBuf[i] = ctrlBuffer[(ctrlPos + i) % SIZE];
		len = i;
	}
	tmpBuf[len + 1] = 0;
	ctrlPos = (curPos + 1) % SIZE;
	uint16_t pos = 0;
	if(tmpBuf[pos] == 'M'){
		pos++;
		if(strcmpLen(mV, tmpBuf + pos, sizeof(mV) - 1) == 0){
			pos += sizeof(mV) - 1;
			float rpm;
			pos += atof(&rpm, tmpBuf + pos);
			motorSetVel(rpm, Left);
		}
		else if(strcmpLen(mP, tmpBuf + pos, sizeof(mP) - 1) == 0){
			pos += sizeof(mP) - 1;
			float position;
			pos += atof(&position, tmpBuf + pos);
		}
		else if(strcmpLen(mR, tmpBuf + pos, sizeof(mR) - 1) == 0){
			pos += sizeof(mR) - 1;
			float kp, ki, kd;
			pos += atof(&kp, tmpBuf + pos);
			pos++;
			pos += atof(&ki, tmpBuf + pos);
			pos++;
			pos += atof(&kd, tmpBuf + pos);
			motorSetPid(kp, ki, kd);
		}
		else{
			return false;
		}
	}
	else if(tmpBuf[pos] == 'C'){
		pos++;
		if(strcmpLen(cStartV, tmpBuf + pos, sizeof(cStartV) - 1) == 0){
			pos += sizeof(cStartV) - 1;
			motorRunVel(Left);
		}
		else if(strcmpLen(cStartP, tmpBuf + pos, sizeof(cStartP) - 1) == 0){
			pos += sizeof(cStartP) - 1;
			motorRunPos(Left);
		}
		else if(strcmpLen(cStop, tmpBuf + pos, sizeof(cStop) - 1) == 0){
			pos += sizeof(cStop) - 1;
			motorStop(Left);
		}
		else if(strcmpLen(cShutdown, tmpBuf + pos, sizeof(cShutdown) - 1) == 0){
			pos += sizeof(cShutdown) - 1;
			motorShutdown(Left);
		}
		else{
			return false;
		}

	}
	else{
		ctrlPos = (curPos + 1) % SIZE;
		return false;
	}
	return true;
}

void ctrlSendInfo(){
	char buffer[SIZE];
	uint16_t cnt = 0;

	float speed = cur_speed[Left]*PID_freq*60/cpr;
	cnt += appendInfo(buffer + cnt, iRpm, sizeof(iRpm) - 1, speed);

	float cte = cte_prev[Left]*PID_freq*60/cpr;;
	cnt += appendInfo(buffer + cnt, iCte, sizeof(iCte) - 1, cte);

	cnt += appendInfo(buffer + cnt, iCur, sizeof(iCur) - 1, 1);

	cnt += appendInfo(buffer + cnt, iMotV, sizeof(iMotV) - 1, 1);

	buffer[cnt] = 0;

	ctrlSend(buffer, cnt);
}


uint16_t appendInfo(char* buffer, const char* data, uint16_t len, float num){
	uint16_t cnt = 0;
	strcpy(buffer + cnt, data);
	cnt += len;
	cnt += ftoa(num, buffer + cnt, 5, 2);
	buffer[cnt++] = '\n';
	return cnt;
}

void ctrlSend(char* data, uint16_t len){
	while(DMA_Channel_TX->CNDTR > 0);
	DMA_Cmd(DMA_Channel_TX, DISABLE);
	memcpy(dmaBuffer, data, len);
	DMA_Channel_TX->CNDTR = len;
	DMA_Cmd(DMA_Channel_TX, ENABLE);
}

