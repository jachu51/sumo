/*
 * buttons.c
 *
 * Created: 2011-05-18 22:54:01
 *  Author: Jachu
 */ 
#include "sys.h"
#include "buttons.h"

#define THRESHOLD 5
#define THRESHOLD_HOLD 200

volatile uint8_t isPushed[BUTTONS_NUM];
volatile uint8_t isHeld[BUTTONS_NUM];
volatile uint8_t cntPushed[BUTTONS_NUM];
volatile uint8_t wasHeld[BUTTONS_NUM];
//const uint8_t pinNum[BUTTONS_NUM] = {0, 1, 2, 3};
//void (*handlersPushed[BUTTONS_NUM])(void) = {handleButton1Pushed, handleButton2Pushed, handleButton3Pushed, handleButton4Pushed};
//void (*handlersHeld[BUTTONS_NUM])(void) = {handleButton1Held, handleButton2Held, handleButton3Held, handleButton4Held};

/*ISR(TCC1_OVF_vect){
	int i;
	for(i = 0; i < BUTTONS_NUM; i++){
		if(!(PORTE_IN & (1<<pinNum[i]))){
			cntPushed[i]++;
		}
		else{
			if(cntPushed[i] > THRESHOLD && wasHeld[i] == 0){
				isPushed[i] = 1;
			}
			wasHeld[i] = 0;
			cntPushed[i] = 0;
		}
		if(cntPushed[i] >= THRESHOLD_HOLD){
			isHeld[i] = 1;
			cntPushed[i] = 0;
			wasHeld[i] = 1;
		}
	}
}*/


void initButtons(){
	/*PORTE_PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN1CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN2CTRL = PORT_OPC_PULLUP_gc;
	PORTE_PIN3CTRL = PORT_OPC_PULLUP_gc;
	PMIC_CTRL = PMIC_LOLVLEN_bm;
	sei();
	TCC1_CTRLA = TC_CLKSEL_DIV1024_gc;
	TCC1_CTRLB = TC_WGMODE_NORMAL_gc;
	TCC1_INTCTRLA = TC_OVFINTLVL_LO_gc;
	TCC1_PER = 312;
	*/
	int i;
	for(i = 0; i < BUTTONS_NUM; i++){
		cntPushed[i] = 0;
		isPushed[i] = 0;
		isHeld[i] = 0;
		wasHeld[i] = 0;
	}
}
