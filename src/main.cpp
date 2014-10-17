#include <stm32f10x.h>
#include <stm32f10x_conf.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include "led.h"
#include "sys.h"
#include "motor.h"
#include "lcd.h"
#include "buttons.h"
#include "menu.h"

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
	//motorSetVel(-200, Right);
	//motorRunVel(Right);
	displayMenu(mainLevel, mainLevelSize);
	while(true){
		//ctrlCheckAndExec();
		for(int i = 0; i < 4; i++){
			if(isHeld[i]){
				ledSet(ledPins[i]);
				isHeld[i] = 0;
			}
			else{
				ledReset(ledPins[i]);
			}
		}
		sysDelayMs(500);
		ledSet(ledPins[0]);
		sysDelayMs(500);
		ledReset(ledPins[0]);
		/*sysDelayMs(500);
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
		LcdStr(FONT_1X, buffer);*/
	}
}
