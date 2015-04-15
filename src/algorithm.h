/*
 * algorith.h
 *
 *  Created on: 13 maj 2014
 *      Author: jachu
 */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "sys.h"

enum EnemyDir{
	EnDirLeft,
	EnDirRight,
	EnDirAhead,
	EnDirUnknown,
	EnDirBack
};

static const uint16_t startPin = GPIO_Pin_12;
static const uint16_t killPin = GPIO_Pin_13;

void startModuleInit();

void mainAlgorithm();
void switchDirection();
/**
 *  \return Direction of the enemy.
 */
EnemyDir getEnemyDir();

#endif /* ALGORITH_H_ */
