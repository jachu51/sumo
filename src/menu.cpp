/*
 * menu.cpp
 *
 *  Created on: 27-03-2013
 *      Author: jachu
 */

#include <string.h>
#include "menu.h"
#include "lcd.h"
#include "buttons.h"
#include "lineDet.h"
#include "sys.h"
#include "adc.h"
#include "motor.h"
#include "algorithm.h"
#include "eeprom.h"

#define LCD_ROWS 6
#define LCD_COLS 14
#define LCD_CHAR_HEIGHT 8
#define ENTRY_BUF 28
#define TAB 2

static const char mainLevel1[]  = "Testy";
static const char mainLevel2[]  = "Walka";
static const char mainLevel3[]  = "Parametry";

static const char testsMenu1[]  = "Sharp";
static const char testsMenu2[]  = "Czujnik linii";
static const char testsMenu3[]  = "Silnik";

static const char paramMenu1[]  = "kp";
static const char paramMenu2[]  = "ki";
static const char paramMenu3[]  = "kd";



void testFunc(void){

}

void testLineDet(void){
	while(isPushed[LEFT_BUT] == 0){
		LcdClear();
		static const byte rectA = 10;
		static const byte detRect[][2][2] = {{{0, 0},
												{rectA-1, rectA-1}},

												{{LCD_X_RES-rectA, 0},
												{LCD_X_RES-1, rectA-1}},

												{{0, LCD_Y_RES-rectA},
												{rectA-1, LCD_Y_RES-1}},

												{{LCD_X_RES-rectA, LCD_Y_RES-rectA},
												{LCD_X_RES-1, LCD_Y_RES-1}}};
		static const Detector detList[] = {DetFrontLeft,
											DetFrontRight,
											DetRearLeft,
											DetRearRight};
		for(int i = 0; i < sizeof(detList)/sizeof(detList[0]); i++){
			if(lineDetCheck(detList[i])){
				LcdSingleBar(	detRect[i][0][0],
								detRect[i][1][1],
								detRect[i][1][1] - detRect[i][0][1] + 1,
								detRect[i][1][0] - detRect[i][0][0] + 1,
						PIXEL_ON);
			}
			else{
				LcdLine(detRect[i][0][0],
						detRect[i][0][0],
						detRect[i][0][1],
						detRect[i][1][1],
						PIXEL_ON);
				LcdLine(detRect[i][0][0],
						detRect[i][1][0],
						detRect[i][1][1],
						detRect[i][1][1],
						PIXEL_ON);
				LcdLine(detRect[i][1][0],
						detRect[i][1][0],
						detRect[i][1][1],
						detRect[i][0][1],
						PIXEL_ON);
				LcdLine(detRect[i][1][0],
						detRect[i][0][0],
						detRect[i][0][1],
						detRect[i][0][1],
						PIXEL_ON);
			}
		}
		sysDelayMs(100);
	}
	isPushed[LEFT_BUT] = 0;
}

void testSharps(void){
	while(isPushed[LEFT_BUT] == 0){
		LcdClear();

		LcdGotoXYFont(1, 1);
		LcdStr(FONT_1X, (const unsigned char*)"Sharp FL: ");
		char buffer[20];
		uint32_t dist = adcSharpDist(ShFrontLeft);
		int pos = uitoa(dist, buffer, 10);
		buffer[pos] = 0;
		LcdStr(FONT_1X, (const unsigned char*)buffer);
		LcdStr(FONT_1X, (const unsigned char*)"mm");

		LcdGotoXYFont(1, 2);
		LcdStr(FONT_1X, (const unsigned char*)"Sharp FR: ");
		dist = adcSharpDist(ShFrontRight);
		pos = uitoa(dist, buffer, 10);
		buffer[pos] = 0;
		LcdStr(FONT_1X, (const unsigned char*)buffer);
		LcdStr(FONT_1X, (const unsigned char*)"mm");

		LcdGotoXYFont(1, 3);
		LcdStr(FONT_1X, (const unsigned char*)"Sharp RL: ");
		dist = adcSharpDist(ShRearLeft);
		pos = uitoa(dist, buffer, 10);
		buffer[pos] = 0;
		LcdStr(FONT_1X, (const unsigned char*)buffer);
		LcdStr(FONT_1X, (const unsigned char*)"mm");

		LcdGotoXYFont(1, 4);
		LcdStr(FONT_1X, (const unsigned char*)"Sharp RR: ");
		dist = adcSharpDist(ShRearRight);
		pos = uitoa(dist, buffer, 10);
		buffer[pos] = 0;
		LcdStr(FONT_1X, (const unsigned char*)buffer);
		LcdStr(FONT_1X, (const unsigned char*)"mm");

		sysDelayMs(100);
	}
	isPushed[LEFT_BUT] = 0;
}

void testMotors(void){
	static const float maxSpeed = 500;
	float curSpeed[] = {0, 0};

	static const Motor motorsList[] = {MotorLeft, MotorRight};
	static const byte rectX = 20;
	static const byte rectY = 60;
	static const byte offX = 2;
	static const byte offY = 2;
	static const byte motorsRect[][2][2] = {{{offX, offY},
												{offX + rectX - 1, offY + rectY - 1}},

												{{LCD_X_RES - offX - rectX, offY},
												{LCD_X_RES - offX - 1, offY + rectY - 1}}};
	int active = 0;
	for(int i = 0; i < sizeof(motorsList)/sizeof(motorsList[0]); i++){
		motorSetVel(0, motorsList[i]);
		motorRunVel(motorsList[i]);
	}
	while(isPushed[LEFT_BUT] == 0){
		LcdClear();
		for(int i = 0;i < sizeof(motorsList)/sizeof(motorsList[0]); i++){
			//Rectangle
			LcdLine(motorsRect[i][0][0],
					motorsRect[i][0][0],
					motorsRect[i][0][1],
					motorsRect[i][1][1],
					PIXEL_ON);
			LcdLine(motorsRect[i][0][0],
					motorsRect[i][1][0],
					motorsRect[i][1][1],
					motorsRect[i][1][1],
					PIXEL_ON);
			LcdLine(motorsRect[i][1][0],
					motorsRect[i][1][0],
					motorsRect[i][1][1],
					motorsRect[i][0][1],
					PIXEL_ON);
			LcdLine(motorsRect[i][1][0],
					motorsRect[i][0][0],
					motorsRect[i][0][1],
					motorsRect[i][0][1],
					PIXEL_ON);
			//Speed indicator
			byte y1 = offY + rectY/2;
			byte y2 = y1 - rectY/2 * curSpeed[i]/maxSpeed;
			LcdRect(motorsRect[i][0][0],
					motorsRect[i][1][0],
					min(y1, y2),
					max(y1, y2),
					PIXEL_ON);
			if(i == active){
				LcdLine(motorsRect[i][0][0]-1,
						motorsRect[i][0][0]-1,
						motorsRect[i][0][1]-1,
						motorsRect[i][1][1]+1,
						PIXEL_ON);
				LcdLine(motorsRect[i][0][0]-1,
						motorsRect[i][1][0]+1,
						motorsRect[i][1][1]+1,
						motorsRect[i][1][1]+1,
						PIXEL_ON);
				LcdLine(motorsRect[i][1][0]+1,
						motorsRect[i][1][0]+1,
						motorsRect[i][1][1]+1,
						motorsRect[i][0][1]-1,
						PIXEL_ON);
				LcdLine(motorsRect[i][1][0]+1,
						motorsRect[i][0][0]-1,
						motorsRect[i][0][1]-1,
						motorsRect[i][0][1]-1,
						PIXEL_ON);
			}
			motorSetVel(curSpeed[i], motorsList[i]);
		}
		if(isPushed[RIGHT_BUT] != 0){
			active = (active + 1) % sizeof(motorsList)/sizeof(motorsList[0]);

			isPushed[RIGHT_BUT] = 0;
		}
		if(isPushed[UP_BUT] != 0){
			curSpeed[active] += maxSpeed/20;
			curSpeed[active] = curSpeed[active] > maxSpeed ? maxSpeed : curSpeed[active];

			isPushed[UP_BUT] = 0;
		}
		if(isPushed[DOWN_BUT] != 0){
			curSpeed[active] -= maxSpeed/20;
			curSpeed[active] = curSpeed[active] < -maxSpeed ? -maxSpeed : curSpeed[active];

			isPushed[DOWN_BUT] = 0;
		}
		sysDelayMs(100);
	}
	for(int i = 0; i < sizeof(motorsList)/sizeof(motorsList[0]); i++){
		motorShutdown(motorsList[i]);
	}
	isPushed[LEFT_BUT] = 0;
}

enum ParamType{
	Float,
	Int
};

static const char paramName[][20] = {"kp",
										"ki",
										"kd"};
static const ParamType paramType[] = {Float,
										Float,
										Float};
static const uint16_t paramAddr[] = {0x00,
										0x04,
										0x08};

void paramChange(int i){
	int ind = 0;
	bool updateDisp = true;
	float valFloat;
	int valInt;

	uint16_t hiVal, lowVal;
	EE_ReadVariable(paramAddr[i] + 2, &lowVal);
	EE_ReadVariable(paramAddr[i], &hiVal);

	if(paramType[i] == Float){
		valFloat = (((uint32_t)hiVal << 16) | lowVal);
		if(valFloat > 2000){
			valFloat = 0;
		}
	}
	if(paramType[i] == Int){
		valInt = (((uint32_t)hiVal << 16) | lowVal);
		if(valInt > 2000){
			valInt = 0;
		}
	}

	while(isPushed[LEFT_BUT] == 0){
		if(updateDisp){
			LcdClear();
			LcdGotoXYFont(1, 1);
			if(paramType[i] == Float){
				char buffer[20];
				ftoa(valFloat, buffer, 4, 2);
				LcdStr(FONT_1X, (const byte*)buffer);
			}
			if(paramType[i] == Int){
				char buffer[20];
				uitoa(iabs(valInt), buffer, 10);
				LcdStr(FONT_1X, (const byte*)buffer);
			}
			LcdUpdate();
			updateDisp = false;
		}
		if(isPushed[UP_BUT] != 0){
			if(paramType[i] == Float){
				valFloat += 0.5;
			}
			if(paramType[i] == Int){
				valInt += 1;
			}
			updateDisp = true;
			isPushed[UP_BUT] = 0;
		}
		if(isPushed[DOWN_BUT] != 0){
			if(paramType[i] == Float){
				valFloat -= 0.5;
			}
			if(paramType[i] == Int){
				valInt -= 1;
			}
			updateDisp = true;
			isPushed[DOWN_BUT] = 0;
		}
	}
	if(paramType[i] == Float){
		lowVal = (uint16_t)((uint32_t)valFloat & 0xffff);
		hiVal = (uint16_t)(((uint32_t)valFloat >> 16) & 0xffff);
	}
	if(paramType[i] == Int){
		lowVal = (uint16_t)((uint32_t)valInt & 0xffff);
		hiVal = (uint16_t)(((uint32_t)valInt >> 16) & 0xffff);
	}
	EE_WriteVariable(paramAddr[i] + 2, lowVal);
	EE_WriteVariable(paramAddr[i], hiVal);

	isPushed[LEFT_BUT] = 0;
}

void paramChange0(void){
	paramChange(0);
}

void paramChange1(void){
	paramChange(1);
}

void paramChange2(void){
	paramChange(2);
}

MenuEntry testsLevel[] = {
		{testsMenu1, sizeof(testsMenu1) - 1, 0, 0, testSharps},
		{testsMenu2, sizeof(testsMenu2) - 1, 0, 0, testLineDet},
		{testsMenu3, sizeof(testsMenu3) - 1, 0, 0, testMotors},
};

MenuEntry paramLevel[] = {
		{paramMenu1, sizeof(paramMenu1) - 1, 0, 0, paramChange0},
		{paramMenu2, sizeof(paramMenu2) - 1, 0, 0, paramChange1},
		{paramMenu3, sizeof(paramMenu3) - 1, 0, 0, paramChange2}
};

MenuEntry mainLevel[] = {
	{mainLevel1, sizeof(mainLevel1) - 1, testsLevel, 3, 0},
	{mainLevel2, sizeof(mainLevel2) - 1, 0, 0, mainAlgorithm},
	{mainLevel3, sizeof(mainLevel3) - 1, paramLevel, 3, 0}
};

uint8_t mainLevelSize = 3;

uint8_t numEntryRows(uint8_t len){
	return (uint8_t)(((int8_t)len - 2 - 1)/(LCD_COLS - 2) + 1);
}

//activeLine - lowest line of the activeEntry
void markActiveEntry(uint8_t activeLine, uint8_t entryLen){
	LcdSingleBar(0, (activeLine + 1)*LCD_CHAR_HEIGHT, numEntryRows(entryLen)*LCD_CHAR_HEIGHT, LCD_X_RES, PIXEL_XOR);
}

uint8_t redrawMenu(MenuEntry* curLevel, uint8_t topEntry, uint8_t activeEntry, uint8_t size){
	uint8_t i = topEntry;
	uint8_t activeLine = 0;
	LcdClear();
	LcdGotoXYFont(1,1);
	while(i < size && activeLine < LCD_ROWS){
		uint8_t pos = 0;
		char lcdData[ENTRY_BUF];
		strcpy(lcdData, curLevel[i].text);
		while(lcdData[pos] != 0){
			if(pos == LCD_COLS){
				if(++activeLine == LCD_ROWS){
					i--;
					break;
				}
				LcdGotoXYFont(1 + TAB, activeLine + 1);
			}
			LcdChr(FONT_1X, lcdData[pos]);
			pos++;
		}
		if(i == activeEntry){
			markActiveEntry(activeLine, curLevel[activeEntry].len);
		}
		i++;
		LcdGotoXYFont(1, ++activeLine + 1);
	}
	activeLine = 0;
	LcdUpdate();
	return i - 1;
}

void displayMenu(MenuEntry* curLevel, uint8_t size){
	uint8_t activeEntry = 0;
	uint8_t topEntry = 0;
	uint8_t bottomEntry = redrawMenu(curLevel, topEntry, activeEntry, size);
	while(1){
		if(isPushed[DOWN_BUT] == 1){
			if(activeEntry + 1 == size){
				activeEntry = 0;
				topEntry = 0;
			}
			else{
				activeEntry++;
				if(activeEntry > bottomEntry){
					int8_t lines = numEntryRows(curLevel[activeEntry].len);
					while(lines > 0){
						lines -= numEntryRows(curLevel[topEntry++].len);
					}
				}
			}
			bottomEntry = redrawMenu(curLevel, topEntry, activeEntry, size);
			isPushed[DOWN_BUT] = 0;
		}
		if(isPushed[UP_BUT] == 1){
			if(activeEntry - 1 < 0){
				activeEntry = topEntry = size - 1;
				uint8_t lines = 0;
				while(lines < LCD_ROWS){
					lines += numEntryRows(curLevel[topEntry--].len);
				}
				topEntry++;
				if(lines > LCD_ROWS){
					topEntry++;
				}
			}
			else{
				activeEntry--;
				if(activeEntry < topEntry){
					topEntry--;
				}
			}
			bottomEntry = redrawMenu(curLevel, topEntry, activeEntry, size);
			isPushed[UP_BUT] = 0;
		}
		if(isPushed[RIGHT_BUT] == 1){
			isPushed[RIGHT_BUT] = 0;
			if(curLevel[activeEntry].function != NULL){
				((voidFunc)curLevel[activeEntry].function)();
			}
			if(curLevel[activeEntry].nextLevel != NULL){
				displayMenu(curLevel[activeEntry].nextLevel, curLevel[activeEntry].nextLevelSize);
			}
			bottomEntry = redrawMenu(curLevel, topEntry, activeEntry, size);
		}
		if(isPushed[LEFT_BUT] == 1){
			isPushed[LEFT_BUT] = 0;
			return;
		}
	}
}



