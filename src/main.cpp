/*******************************************************************************
 *
 * Copyright (C) 2012 Jorge Aparicio <jorge.aparicio.r@gmail.com>
 *
 * This file is part of bareCortexM.
 *
 * bareCortexM is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * bareCortexM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with bareCortexM. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <stm32f10x.h>
#include <stm32f10x_conf.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include "led.h"
#include "sys.h"
#include "motor.h"
#include "lcd.h"

void delay(int del){
	while(del > 0){
		del--;
	}
}

int main()
{
	//sysInit();
	//motorRunVel();
	sysInit();
	motorSetVel(-200, Right);
	motorRunVel(Right);
	while(true){
		//ctrlCheckAndExec();
		ledToggle(LED1 | LED2 | LED3 | LED4);
		sysDelayMs(500);
		LcdClear();
		LcdGotoXYFont(1, 1);
		LcdStr(FONT_1X, (const unsigned char*)"Encoder: ");
		unsigned char buffer[20];
		uint8_t cnt = uitoa(motorReadEnc(Right), (char*)buffer, 10);
		buffer[cnt] = 0;
		LcdStr(FONT_1X, buffer);
		LcdStr(FONT_1X, (const unsigned char*)", TIM1: ");
		cnt = uitoa(motor_width[Right], (char*)buffer, 10);
		buffer[cnt] = 0;
		LcdStr(FONT_1X, buffer);
	}
}
