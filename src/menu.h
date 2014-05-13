/*
 * menu.h
 *
 *  Created on: 27-03-2013
 *      Author: jachu
 */

#ifndef MENU_H_
#define MENU_H_

#include <stm32f10x_gpio.h>

typedef struct menuentry{
	const char* text;
	uint8_t len;
	struct menuentry* nextLevel;
	uint8_t nextLevelSize;
	void (*function)(void);
} MenuEntry;

extern MenuEntry mainLevel[];
extern uint8_t mainLevelSize;

void displayMenu(MenuEntry* curLevel, uint8_t size);

#endif /* MENU_H_ */
