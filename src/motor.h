/*
 * motor.h
 *
 *  Created on: 26-12-2012
 *      Author: jachu
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "stm32f10x_conf.h"

#define OUT_FWD_1 GPIO_Pin_8
#define OUT_BCKWD_1 GPIO_Pin_14
#define OUT_FWD_2 GPIO_Pin_11
#define OUT_BCKWD_2 GPIO_Pin_15
#define OUT_PORT_FWD GPIOA
#define OUT_PORT_BCKWD GPIOB

#define IN_A_1 GPIO_Pin_6
#define IN_B_1 GPIO_Pin_7
#define IN_1_PORT GPIOB

#define IN_A_2 GPIO_Pin_6
#define IN_B_2 GPIO_Pin_7
#define IN_2_PORT GPIOC

static float maxAcc = 1000;

enum Direction{
	MotorForward,
	MotorBackward
};

enum Motor{
	MotorLeft = 0,
	MotorRight = 1
};

//extern volatile int32_t motor_width[2];
//extern volatile int32_t cur_speed[2];
//extern volatile int32_t cte_int[2], cte_prev[2];
//extern uint16_t cpr;

void motorInit(float imotorKp,
				float imotorKi,
				float imotorKd,
				float icurrentKp,
				float icurrentKi,
				uint32_t icpr);
void motorRunVel(Motor motor);
void motorRunPos(Motor motor);
void motorStop(Motor motor);
void motorShutdown(Motor motor);
void motorPID(Motor motor);
void motorRamp(float freq, Motor motor);
void motorSetVel(float speed, Motor motor); //rpm
void motorSetPos(float posL, Motor motor);		//rotations
void motorSetPid(float imotorKp, float imotorKi, float imotorKd);
bool motorIsCurLimited(Motor motor);

uint16_t motorReadEnc(Motor motor);
float motorReadVel(Motor motor);
float motorReadDist(Motor motor);
void motorResetDist(Motor motor);
void motorEnableCC(Direction dir, Motor motor);

extern "C" {

void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);

}

#endif /* MOTOR_H_ */
