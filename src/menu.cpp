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

#define LCD_ROWS 6
#define LCD_COLS 14
#define LCD_CHAR_HEIGHT 8
#define ENTRY_BUF 28
#define TAB 2

static const char mainLevel1[]  = "Testy";
static const char mainLevel2[]  = "Tryby";

static const char testsMenu1[]  = "Sharp";
static const char testsMenu2[]  = "Czujnik linii";
static const char testsMenu3[]  = "Silnik";




void testFunc(void){

}

void testLineDet(void){
	while(isPushed[LEFT_BUT] == 0){
		LcdClear();
		static const byte rectA = 10;
		static const byte frontLeftRect[][2] = {{0, 0},
												{rectA-1, rectA-1}};
		static const byte frontRightRect[][2] = {{0, LCD_Y_RES-rectA},
												{rectA-1, LCD_Y_RES-1}};
		static const byte rearLeftRect[][2] = {{LCD_X_RES-rectA, 0},
												{LCD_X_RES-1, rectA-1}};
		static const byte rearRightRect[][2] = {{LCD_X_RES-rectA, LCD_Y_RES-rectA},
												{LCD_X_RES-1, LCD_Y_RES-1}};
		if(lineDetCheck(DetFrontLeft)){
			LcdSingleBar(	frontLeftRect[0][0],
							frontLeftRect[0][1],
							frontLeftRect[1][0]-frontLeftRect[0][0],
							frontLeftRect[1][1]-frontLeftRect[0][1],
							PIXEL_ON);
		}
		else{
			LcdRect(frontLeftRect[0][0],
					frontLeftRect[1][0],
					frontLeftRect[0][1],
					frontLeftRect[1][1],
					PIXEL_ON);
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

}

MenuEntry testsLevel[] = {
		{testsMenu1, sizeof(testsMenu1) - 1, 0, 0, testSharps},
		{testsMenu2, sizeof(testsMenu2) - 1, 0, 0, testLineDet},
		{testsMenu3, sizeof(testsMenu3) - 1, 0, 0, testMotors},
};

MenuEntry mainLevel[] = {
	{mainLevel1, sizeof(mainLevel1) - 1, testsLevel, 3, 0},
	{mainLevel2, sizeof(mainLevel2) - 1, 0, 0, testFunc}
};



uint8_t mainLevelSize = 2;

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



