/*
 * adc.h
 *
 *  Created on: 19-01-2013
 *      Author: jachu
 */

#ifndef ADC_H_
#define ADC_H_

#include <stm32f10x_dma.h>

extern volatile float curMean;
extern volatile float emfMean;
extern volatile float motVMean;

#define CUR_PIN GPIO_Pin_0
#define CUR_CHANNEL ADC_Channel_0
#define CUR_RANK 1
#define CUR_MUL (3.3/20.0/0.025/4096*0.827586)

#define EMF_POS_PIN GPIO_Pin_1
#define EMF_POS_CHANNEL ADC_Channel_1
#define EMF_POS_RANK 2

#define EMF_NEG_PIN GPIO_Pin_2
#define EMF_NEG_CHANNEL ADC_Channel_2
#define EMF_NEG_RANK 3
#define EMF_MUL (1.0)

#define MOT_V_PIN GPIO_Pin_3
#define MOT_V_CHANNEL ADC_Channel_3
#define MOT_V_RANK 4
#define MOT_V_MUL (3.3*(27.0 + 2.4)/2.4/4096*0.962963)

#define ADC_DMA_CHANNEL DMA1_Channel1
#define ADC_NSAMP_MEAN 100

void adcInit();
float adcCurMeas();
float adcEmfMeas();
float adcMotSupplyMeas();

extern "C" {

void ADC1_2_IRQHandler(void);


}

#endif /* ADC_H_ */
