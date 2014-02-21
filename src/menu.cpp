/*
 * menu.cpp
 *
 *  Created on: 27-03-2013
 *      Author: jachu
 */

#include "menu.h"
#include "lcd.h"
#include <string.h>

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

MenuEntry testsLevel[] = {
		{testsMenu1, sizeof(testsMenu1) - 1, 0, 0, testFunc},
		{testsMenu1, sizeof(testsMenu2) - 1, 0, 0, testFunc},
		{testsMenu1, sizeof(testsMenu3) - 1, 0, 0, testFunc},
};

MenuEntry mainLevel[] = {
	{mainLevel1, sizeof(mainLevel1) - 1, testsLevel, 3, 0},
	{mainLevel2, sizeof(mainLevel2) - 1, 0, 0, testFunc}
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
	/*uint8_t activeEntry = 0;
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
					int8_t lines = numEntryRows(pgm_read_byte(&(curLevel[activeEntry].len)));
					while(lines > 0){
						lines -= numEntryRows(pgm_read_byte(&(curLevel[topEntry++].len)));
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
					lines += numEntryRows(pgm_read_byte(&(curLevel[topEntry--].len)));
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
			if(pgm_read_word(&(curLevel[activeEntry].function)) != NULL){
				((voidFunc)pgm_read_word(&(curLevel[activeEntry].function)))();
			}
			if(pgm_read_word(&(curLevel[activeEntry].nextLevel)) != NULL){
				displayMenu((MenuEntry*)pgm_read_word(&(curLevel[activeEntry].nextLevel)), 1);	//! POPRAWIC!!!!!!!!!!
			}
			bottomEntry = redrawMenu(curLevel, topEntry, activeEntry, size);
			isPushed[RIGHT_BUT] = 0;
		}
		if(isPushed[LEFT_BUT] == 1){
			return;
			isPushed[LEFT_BUT] = 0;
		}
	}*/
}



