/*
 * adc.cpp
 *
 *  Created on: 19-01-2013
 *      Author: jachu
 */
#include <stm32f10x_rcc.h>
#include <stm32f10x_dma.h>
#include <stm32f10x_adc.h>
#include <stm32f10x_gpio.h>
#include <misc.h>
#include "adc.h"

#define ADC_BUFFER (4*ADC_NSAMP_MEAN)

uint16_t adcBuffer[ADC_BUFFER];
volatile float curMean;
volatile float emfMean;
volatile float motVMean;

void adcInit(){
	/* ADCCLK = PCLK2/4 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div4);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Pin = CUR_PIN | EMF_POS_PIN | EMF_NEG_PIN | MOT_V_PIN;
	gpioInit.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &gpioInit);

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
	adcInit.ADC_NbrOfChannel = 4;
	ADC_Init(ADC1, &adcInit);
	ADC_RegularChannelConfig(ADC1, CUR_CHANNEL, CUR_RANK, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, EMF_POS_CHANNEL, EMF_POS_RANK, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, EMF_NEG_CHANNEL, EMF_NEG_RANK, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, MOT_V_CHANNEL, MOT_V_RANK, ADC_SampleTime_28Cycles5);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

float adcCurMeas(){
	return curMean;
}

float adcEmfMeas(){
	return emfMean;
}

float adcMotSupplyMeas(){
	return motVMean;
}

extern "C" {

void ADC1_2_IRQHandler(void){
	int32_t tmpCur = 0;
	int32_t tmpEmfPos = 0;
	int32_t tmpEmfNeg = 0;
	int32_t tmpMotV = 0;
	for(int i = 0; i < ADC_NSAMP_MEAN; i++){
		tmpCur += adcBuffer[i*4 + CUR_RANK - 1];
		tmpEmfPos += adcBuffer[i*4 + EMF_POS_RANK - 1];
		tmpEmfNeg += adcBuffer[i*4 + EMF_NEG_RANK - 1];
		tmpMotV += adcBuffer[i*4 + MOT_V_RANK - 1];
	}
	curMean = (float)tmpCur * CUR_MUL / ADC_NSAMP_MEAN;
	emfMean = ((float)tmpEmfPos - (float)tmpEmfNeg) * EMF_MUL / ADC_NSAMP_MEAN;
	motVMean = (float)tmpMotV * MOT_V_MUL / ADC_NSAMP_MEAN;

	ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
}

}




