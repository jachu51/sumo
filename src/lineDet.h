/*
 * lineDet.h
 *
 *  Created on: 17-03-2014
 *      Author: jachu
 */

#ifndef LINEDET_H_
#define LINEDET_H_

enum Detector{
	DetFrontLeft = 0,
	DetFrontRight = 1,
	DetRearLeft = 2,
	DetRearRight = 3
};

static const uint16_t lineDetPins[] = {	GPIO_Pin_12,
											GPIO_Pin_13,
											GPIO_Pin_4,
											GPIO_Pin_5};

static GPIO_TypeDef* lineDetPorts[] = {	GPIOB,
										GPIOB,
										GPIOC,
										GPIOC};

void lineDetInit();
bool lineDetCheck(Detector det);


#endif /* LINEDET_H_ */
