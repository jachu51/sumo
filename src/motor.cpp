/*
 * motor.c
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */

#include "motor.h"
#include "sys.h"
#include "led.h"
#include "adc.h"
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_gpio.h>

#define MAX_WIDTH 720 //2880 //1440
#define MAX_RPM 500
#define MAX_MAX_WIDTH_COEFF (0.9f)

static const float motorCurLimit = 1.0f;		//[A]
volatile float maxWidthCoeff[2];

volatile int32_t cte_int[2], cte_prev[2];
volatile float motorKp, motorKi, motorKd;
volatile float currentKp, currentKi;
uint16_t cpr = 1;
volatile int32_t motor_width[2];
volatile uint16_t prev_enc[2];
volatile int32_t cur_speed[2];
volatile float cur_pos[2];
volatile int32_t set_speed[2];
volatile int32_t des_set_speed[2];
volatile float set_pos[2];
volatile bool motorUpdate[2];
volatile bool motorEnable[2];
volatile float currentCteInt[2];

void motorInit(float imotorKp,
				float imotorKi,
				float imotorKd,
				float icurrentKp,
				float icurrentKi,
				uint32_t icpr)
{
	motorKp = imotorKp; motorKd = imotorKd; motorKi = imotorKi; cpr = icpr;
	currentKp = icurrentKp;
	currentKi = icurrentKi;
	for(int i = 0; i < 2; i++){
		cte_int[i] = 0; cte_prev[i] = 0;
		prev_enc[i] = 0;
		cur_speed[i] = 0;
		cur_pos[i] = 0;
		motor_width[i] = 0;
		motorEnable[i] = false;
		motorUpdate[i] = true;
		set_speed[i] = 0;
		des_set_speed[i] = 0;
		set_pos[i] = 0;
		maxWidthCoeff[i] = MAX_MAX_WIDTH_COEFF;
		currentCteInt[i] = 0;
	}

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	gpioInit.GPIO_Pin = OUT_FWD_1 | OUT_FWD_2;
	GPIO_Init(OUT_PORT_FWD, &gpioInit);

	gpioInit.GPIO_Pin = OUT_BCKWD_1 | OUT_BCKWD_2;
	GPIO_Init(OUT_PORT_BCKWD, &gpioInit);

	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInit.GPIO_Pin = IN_A_1 | IN_B_1;
	GPIO_Init(IN_1_PORT, &gpioInit);

	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInit.GPIO_Pin = IN_A_2 | IN_B_2;
	GPIO_Init(IN_2_PORT, &gpioInit);

	//Encoder left
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_ICInitTypeDef icInit;
	icInit.TIM_Channel = TIM_Channel_1;
	icInit.TIM_ICPolarity = TIM_ICPolarity_Rising;
	icInit.TIM_ICFilter = 0;
	icInit.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	icInit.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM4, &icInit);
	icInit.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM4, &icInit);

	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_SetAutoreload(TIM4, 0xffff);
	TIM_SetCounter(TIM4, 0);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef initNVIC;
	initNVIC.NVIC_IRQChannel = TIM4_IRQn;
	initNVIC.NVIC_IRQChannelPreemptionPriority = 0;
	initNVIC.NVIC_IRQChannelSubPriority = 0;
	initNVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&initNVIC);

	TIM_Cmd(TIM4, ENABLE);

	//Encoder right

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	icInit.TIM_Channel = TIM_Channel_1;
	icInit.TIM_ICPolarity = TIM_ICPolarity_Rising;
	icInit.TIM_ICFilter = 0;
	icInit.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	icInit.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM3, &icInit);
	icInit.TIM_Channel = TIM_Channel_2;
	TIM_ICInit(TIM3, &icInit);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_SetAutoreload(TIM3, 0xffff);
	TIM_SetCounter(TIM3, 0);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	initNVIC.NVIC_IRQChannel = TIM3_IRQn;
	initNVIC.NVIC_IRQChannelPreemptionPriority = 0;
	initNVIC.NVIC_IRQChannelSubPriority = 0;
	initNVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&initNVIC);

	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

	TIM_Cmd(TIM3, ENABLE);

	//PWM
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_TimeBaseInitTypeDef tim1Init;
	TIM_OCInitTypeDef ocInit;
	tim1Init.TIM_Period = MAX_WIDTH;
	tim1Init.TIM_Prescaler = 1;
	tim1Init.TIM_ClockDivision = 0;
	tim1Init.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &tim1Init);

	/* PWM1 Mode configuration: Channel2 */
	ocInit.TIM_OCMode = TIM_OCMode_Inactive;
	ocInit.TIM_Pulse = 0;
	ocInit.TIM_OCPolarity = TIM_OCPolarity_High;
	ocInit.TIM_OCNPolarity = TIM_OCNPolarity_High;



	ocInit.TIM_OutputState = TIM_OutputState_Enable;
	ocInit.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OC1Init(TIM1, &ocInit);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

	ocInit.TIM_OutputState = TIM_OutputState_Disable;
	ocInit.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OC2Init(TIM1, &ocInit);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	ocInit.TIM_OutputState = TIM_OutputState_Disable;
	ocInit.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OC3Init(TIM1, &ocInit);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

	ocInit.TIM_OutputState = TIM_OutputState_Enable;
	ocInit.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OC4Init(TIM1, &ocInit);
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM1, ENABLE);

	TIM_ARRPreloadConfig(TIM1, ENABLE);

	/* TIM1 enable counter */
	TIM_Cmd(TIM1, ENABLE);
}

void motorRunVel(Motor motor){
	motorEnable[motor] = true;
	cte_prev[motor] = 0;
	cte_int[motor] = 0;
	motor_width[motor] = 0;
}

void motorRunPos(Motor motor){

}

void motorStop(Motor motor){
	des_set_speed[motor] = 0;
}

void motorShutdown(Motor motor){
	motorEnable[motor] = false;
	if(motor == MotorLeft){
		uint16_t tmpccmr = TIM1->CCMR1;
		tmpccmr &= (uint16_t)(~(TIM_CCMR1_OC1M | TIM_CCMR1_OC2M));
		tmpccmr |= (uint16_t)((TIM_OCMode_Inactive << 8) | TIM_OCMode_Inactive);
		TIM1->CCMR1 = tmpccmr;
	}
	else{
		uint16_t tmpccmr = TIM1->CCMR2;
		tmpccmr &= (uint16_t)(~(TIM_CCMR2_OC3M | TIM_CCMR2_OC4M));
		tmpccmr |= (uint16_t)((TIM_OCMode_Inactive << 8) | TIM_OCMode_Inactive);
		TIM1->CCMR2 = tmpccmr;
	}
	cte_int[motor] = 0; cte_prev[motor] = 0;
	prev_enc[motor] = 0;
	cur_speed[motor] = 0;
	cur_pos[motor] = 0;
	motor_width[motor] = 0;
	motorEnable[motor] = false;
}

void motorSetVel(float speed, Motor motor){ 	//rpm
	if(motor == MotorRight){
		speed = -speed;
	}
	des_set_speed[motor] = speed*cpr/(60*PID_freq);
	//set_speed[motor] = speed*cpr/(60*PID_freq);
}

void motorSetPos(float pos, Motor motor){		//rotations
	set_pos[motor] = pos;
}


void motorSetPid(float imotorKp, float imotorKi, float imotorKd){
	motorKp = imotorKp; motorKd = imotorKd; motorKi = imotorKi;
}

bool motorIsCurLimited(Motor motor){
	return (maxWidthCoeff[motor] < MAX_MAX_WIDTH_COEFF - 1e-2);
}

void motorPID(Motor motor){
	static const bool regulate = true;

	uint16_t cur_enc = motorReadEnc(motor);

	if(motorUpdate[motor] == true){
		if(iabs((int)cur_enc - prev_enc[motor]) > 0xffff/2){
			if(cur_enc < prev_enc[motor]){
				cur_speed[motor] = ((int)cur_enc - prev_enc[motor] + 0xffff);
			}
			else{
				cur_speed[motor] = ((int)cur_enc - prev_enc[motor] - 0xffff);
			}
		}
		else{
			cur_speed[motor] = ((int)cur_enc - prev_enc[motor]);
		}
		motorUpdate[motor] = false;
	}
	else{
		cur_speed[motor] = ((int)cur_enc - prev_enc[motor]);
	}
	//cur_speed = cur_speed*60*PID_freq/cpr;
	prev_enc[motor] = cur_enc;


	if(motorEnable[motor]){
		if(regulate == true){
			int32_t cte = (set_speed[motor] - cur_speed[motor]);
			//uint8_t a = 0;

			/* Current limit PI */
			Meas motCurMeasType = RMotCur;
			if(motor == MotorLeft){
				motCurMeasType = LMotCur;
			}
			else{
				motCurMeasType = RMotCur;
			}
			float curCurrentCte = motorCurLimit - adcMeasVol(motCurMeasType);

			maxWidthCoeff[motor] = currentKp * curCurrentCte +
									currentKi * currentCteInt[motor]/PID_freq;

			if(maxWidthCoeff[motor] > 0 && maxWidthCoeff[motor] < MAX_MAX_WIDTH_COEFF){
				currentCteInt[motor] += curCurrentCte;
			}

			maxWidthCoeff[motor] = max(min(maxWidthCoeff[motor], MAX_MAX_WIDTH_COEFF), 0.0f);

			/* Velocity PID */

			motor_width[motor] = motorKp*cte +
									motorKd*(cte - cte_prev[motor])*PID_freq +
									motorKi*cte_int[motor]/PID_freq;

			//motor_width[motor] = (float)set_speed[motor]/500 * 0.9 * MAX_WIDTH;
			if(motor_width[motor] > MAX_WIDTH*maxWidthCoeff[motor]){
				motor_width[motor] = MAX_WIDTH*maxWidthCoeff[motor];
			}
			if(motor_width[motor] < -MAX_WIDTH*maxWidthCoeff[motor]){
				motor_width[motor] = -MAX_WIDTH*maxWidthCoeff[motor];
			}

			if(motor_width[motor] < (int32_t)(MAX_WIDTH*maxWidthCoeff[motor]) &&
					motor_width[motor] > (int32_t)(-MAX_WIDTH*maxWidthCoeff[motor]))
			{
				cte_int[motor] += cte;
			}
//			if(cte_int[motor] > 2000 || cte_int[motor] < -2000){
//				int a = 0;
//				a++;
//			}

			cte_prev[motor] = cte;
		}
		else{
			motor_width[motor] = set_speed[motor] * (60*PID_freq) * MAX_WIDTH / MAX_RPM / cpr;
			if(motor_width[motor] > MAX_WIDTH*0.9){
				motor_width[motor] = MAX_WIDTH*0.9;
			}
			if(motor_width[motor] < -MAX_WIDTH*0.9){
				motor_width[motor] = -MAX_WIDTH*0.9;
			}
		}

		uint16_t ccrVal;
		if(motor_width[motor] < 0){
			motorEnableCC(MotorBackward, motor);
			ccrVal = -motor_width[motor];
		}
		else{
			motorEnableCC(MotorForward, motor);
			ccrVal = motor_width[motor];
		}
		if(motor == MotorRight){
			TIM1->CCR1 = ccrVal;
			TIM1->CCR2 = ccrVal;
		}
		else{
			TIM1->CCR3 = ccrVal;
			TIM1->CCR4 = ccrVal;
		}
	}
}


void motorRamp(float freq, Motor motor){
	float dAcc = maxAcc/freq;
	if(fabs(des_set_speed[motor] - set_speed[motor]) < dAcc){
		set_speed[motor] = des_set_speed[motor];
	}
	else if(des_set_speed[motor] > set_speed[motor]){
		set_speed[motor] += dAcc;
	}
	else{
		set_speed[motor] -= dAcc;
	}
}

uint16_t motorReadEnc(Motor motor){
	if(motor == MotorRight){
		return TIM_GetCounter(TIM4);
	}
	else{
		return TIM_GetCounter(TIM3);
	}
}

float motorReadVel(Motor motor){
	return cur_speed[motor];
}

float motorReadDist(Motor motor){
	return 0;
}

void motorResetDist(Motor motor){

}

/**
 * Left fwd: OC1, bckwd: OC2N
 * Right fwd: OC4, bckwd: OC3N
 */

void motorEnableCC(Direction dir, Motor motor){
	if(dir == MotorForward && motor == MotorRight){
		uint16_t tmpccmr = TIM1->CCMR1;
		//is OC2 in PWM mode 1?
		if(((uint16_t)(tmpccmr >> 8) & TIM_OCMode_PWM1) != TIM_OCMode_Inactive){
			tmpccmr &= (uint16_t)(~(TIM_CCMR1_OC2M));
			tmpccmr |= (uint16_t)(TIM_OCMode_Inactive << 8);
		}
		else{
			tmpccmr |= (uint16_t)(TIM_OCMode_PWM1);
		}
		TIM1->CCMR1 = tmpccmr;
	}
	else if(dir == MotorBackward && motor == MotorRight){
		uint16_t tmpccmr = TIM1->CCMR1;
		//is OC1 in PWM mode 1?
		if(((uint16_t)(tmpccmr) & TIM_OCMode_PWM1) != TIM_OCMode_Inactive){
			tmpccmr &= (uint16_t)(~(TIM_CCMR1_OC1M));
			tmpccmr |= (uint16_t)(TIM_OCMode_Inactive);
		}
		else{
			tmpccmr |= (uint16_t)(TIM_OCMode_PWM1 << 8);
		}
		TIM1->CCMR1 = tmpccmr;
	}
	else if(dir == MotorForward && motor == MotorLeft){
		uint16_t tmpccmr = TIM1->CCMR2;
		//is OC3 in PWM mode 1?
		if(((uint16_t)(tmpccmr) & TIM_OCMode_PWM1) != TIM_OCMode_Inactive){
			tmpccmr &= (uint16_t)(~(TIM_CCMR2_OC3M));
			tmpccmr |= (uint16_t)(TIM_OCMode_Inactive);
		}
		else{
			tmpccmr |= (uint16_t)(TIM_OCMode_PWM1 << 8);
		}
		TIM1->CCMR2 = tmpccmr;
	}
	else if(dir == MotorBackward && motor == MotorLeft){
		uint16_t tmpccmr = TIM1->CCMR2;
		//is OC4 in PWM mode 1?
		if(((uint16_t)(tmpccmr >> 8) & TIM_OCMode_PWM1) != TIM_OCMode_Inactive){
			tmpccmr &= (uint16_t)(~(TIM_CCMR2_OC4M));
			tmpccmr |= (uint16_t)(TIM_OCMode_Inactive << 8);
		}
		else{
			tmpccmr |= (uint16_t)(TIM_OCMode_PWM1);
		}
		TIM1->CCMR2 = tmpccmr;
	}
}

extern "C" {

void TIM3_IRQHandler(void){
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	motorUpdate[MotorLeft] = true;
}

void TIM4_IRQHandler(void){
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	motorUpdate[MotorRight] = true;
}

}

