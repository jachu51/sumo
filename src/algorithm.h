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
	EnDirUnknown
};

void mainAlgorithm();
void switchDirection();
/**
 *  \return Direction of the enemy.
 */
EnemyDir getEnemyDir();

#endif /* ALGORITH_H_ */
