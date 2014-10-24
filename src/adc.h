/*
 * adc.h
 *
 *  Created on: 19-01-2013
 *      Author: jachu
 */

#ifndef ADC_H_
#define ADC_H_

#include <stm32f10x_adc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_dma.h>

enum Sharps{
	ShFrontLeft = 0,
	ShFrontRight = 1,
	ShRearLeft = 2,
	ShRearRight = 3
};

extern volatile float sharpMean[4];


static const uint16_t sharpPins[] = {	GPIO_Pin_0,
										GPIO_Pin_1,
										GPIO_Pin_2,
										GPIO_Pin_3};

static const uint8_t sharpChannels[] = {	ADC_Channel_10,
											ADC_Channel_11,
											ADC_Channel_12,
											ADC_Channel_13};

static const uint8_t sharpRanks[] = {1, 2, 3, 4};

static const float sharpMul[] = {	3.3/4096,
									3.3/4096,
									3.3/4096,
									3.3/4096};

#define SHARP_PORT GPIOC

#define ADC_DMA_CHANNEL DMA1_Channel1
#define ADC_NSAMP_MEAN 4

void adcInit();
float adcSharpVol(Sharps sharp);
int32_t adcSharpDist(Sharps sharp);

extern "C" {

void ADC1_2_IRQHandler(void);


}

#endif /* ADC_H_ */
