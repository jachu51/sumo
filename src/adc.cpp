/*
 * adc.cpp
 *
 *  Created on: 19-01-2013
 *      Author: jachu
 */
#include <string.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_gpio.h>
#include <misc.h>
#include "sys.h"
#include "adc.h"
#include "sharpConvArr.h"

#define ADC_BUFFER (7*ADC_NSAMP_MEAN)

uint16_t adcBuffer[ADC_BUFFER];

float sharpMean[4];
int32_t sharpDist[4];

#define MEAS_LT_MEAN_NSAMP 500

int measMeanPos;
static const uint32_t measFPMul = 100;
uint32_t measMean[3];
uint32_t measLongTermBuffer[3*MEAS_LT_MEAN_NSAMP];
uint32_t measLongTermMean[3];


void adcInit(){
	memset(measLongTermBuffer, 0, sizeof(measLongTermBuffer));
	memset(measLongTermMean, 0, sizeof(measLongTermMean));
	measMeanPos = 0;

	/* ADCCLK = PCLK2/4 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = sharpPins[0] | sharpPins[1] | sharpPins[2] | sharpPins[3];
	gpioInit.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(SHARP_PORT, &gpioInit);

	gpioInit.GPIO_Pin = measPins[0] | measPins[1] | measPins[2];
	GPIO_Init(MEAS_PORT, &gpioInit);

	NVIC_InitTypeDef nvicInit;
	nvicInit.NVIC_IRQChannel = ADC1_2_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority = 0;
	nvicInit.NVIC_IRQChannelSubPriority = 0;
	nvicInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicInit);

	DMA_InitTypeDef dmaInit;
	dmaInit.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
	dmaInit.DMA_MemoryBaseAddr = (uint32_t)adcBuffer;
	dmaInit.DMA_DIR = DMA_DIR_PeripheralSRC;
	dmaInit.DMA_BufferSize = ADC_BUFFER;
	dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dmaInit.DMA_Mode = DMA_Mode_Circular;
	dmaInit.DMA_Priority = DMA_Priority_Low;
	dmaInit.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(ADC_DMA_CHANNEL, &dmaInit);
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);

	ADC_InitTypeDef adcInit;
	adcInit.ADC_Mode = ADC_Mode_Independent;
	adcInit.ADC_ScanConvMode = ENABLE;
	adcInit.ADC_ContinuousConvMode = DISABLE;
	adcInit.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adcInit.ADC_DataAlign = ADC_DataAlign_Right;
	adcInit.ADC_NbrOfChannel = 7;
	ADC_Init(ADC1, &adcInit);
	ADC_RegularChannelConfig(ADC1, sharpChannels[0], sharpRanks[0], ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, sharpChannels[1], sharpRanks[1], ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, sharpChannels[2], sharpRanks[2], ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, sharpChannels[3], sharpRanks[3], ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, measChannels[0], measRanks[0], ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, measChannels[1], measRanks[1], ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, measChannels[2], measRanks[2], ADC_SampleTime_28Cycles5);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	/* Start ADC1 Software Conversion */
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


float adcSharp(Sharps sharp){
	return sharpMean[sharp];
}

int32_t adcSharpDist(Sharps sharp){
	return convArr[min((uint32_t)(sharpMean[sharp]*100 + 0.5),
						sizeof(convArr)/sizeof(convArr[0]) - 1)];
}

float adcMeasVol(Meas measType){
	return (float)measLongTermMean[measType] / MEAS_LT_MEAN_NSAMP / measFPMul;
}

void updateMeasVol(){

	for(int m = 0; m < 3; ++m){
		measLongTermMean[m] = measLongTermMean[m]
  							+ measMean[m]
							- measLongTermBuffer[m*MEAS_LT_MEAN_NSAMP + measMeanPos];
		measLongTermBuffer[m*MEAS_LT_MEAN_NSAMP + measMeanPos] = measMean[m];
	}

	measMeanPos = (measMeanPos + 1) % MEAS_LT_MEAN_NSAMP;
}

extern "C" {

void ADC1_2_IRQHandler(void){
	int32_t sharpTmp[4] = {0, 0, 0, 0};
	int32_t measTmp[3] = {0, 0, 0};
	for(int i = 0; i < ADC_NSAMP_MEAN; i++){
		for(int s = 0; s < 4; s++){
			sharpTmp[s] += adcBuffer[i*7 + sharpRanks[s] - 1];
		}
		for(int s = 0; s < 3; s++){
			measTmp[s] += adcBuffer[i*7 + measRanks[s] - 1];
		}
	}
	for(int s = 0; s < 4; s++){
		sharpMean[s] = (float)sharpTmp[s] * sharpMul[s] / ADC_NSAMP_MEAN;
	}
	for(int s = 0; s < 3; s++){
		measMean[s] = measTmp[s] * measMul[s] * measFPMul / ADC_NSAMP_MEAN;
	}
	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}

}




