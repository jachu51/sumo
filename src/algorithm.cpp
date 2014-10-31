#include "algorithm.h"
#include "lineDet.h"
#include "adc.h"
#include "motor.h"
#include "buttons.h"
#include "sys.h"
#include "lcd.h"

Detector curDet[] = {
    DetFrontLeft,
    DetFrontRight,
    DetRearLeft,
    DetRearRight};

Sharps curSharp[] = {
    ShFrontLeft,
    ShFrontRight,
    ShRearLeft,
    ShRearRight
};

Motor curMotor[] = {
	MotorLeft,
	MotorRight
};

//enum SeekDir{
//	SeekDirLeft,
//	SeekDirRight
//};

enum Mode{
	Attack,
	Seek
};

float curSpeedMul = 1.0;

void switchDirection() {
    if (curDet[0] == DetFrontLeft) {
        curDet[0] = DetRearRight;
        curDet[1] = DetRearLeft;
        curDet[2] = DetFrontRight;
        curDet[3] = DetFrontLeft;
    }
    else if (curDet[0] == DetRearRight) {
        curDet[0] = DetFrontLeft;
        curDet[1] = DetFrontRight;
        curDet[2] = DetRearLeft;
        curDet[3] = DetRearRight;
    }

    if (curSharp[0] == ShFrontLeft) {
        curSharp[0] = ShRearRight;
        curSharp[1] = ShRearLeft;
        curSharp[2] = ShFrontRight;
        curSharp[3] = ShFrontLeft;
    }
    else if (curSharp[0] == ShRearRight) {
        curSharp[0] = ShFrontLeft;
        curSharp[1] = ShFrontRight;
        curSharp[2] = ShRearLeft;
        curSharp[3] = ShRearRight;
    }
    if(curMotor[0] == MotorLeft) {
    	curMotor[0] = MotorRight;
    	curMotor[1] = MotorLeft;
    }
    else {
    	curMotor[0] = MotorLeft;
    	curMotor[1] = MotorRight;
	}

    curSpeedMul *= -1;
}

EnemyDir getEnemyDir() {
	int32_t distLeft = adcSharpDist(curSharp[0]);
	int32_t distRight = adcSharpDist(curSharp[1]);
	int32_t diff = distLeft - distRight;
	if (diff < 100 && diff > -100 && distLeft < 1200 && distRight < 1200) {
		return EnDirAhead;
	}
	else if (diff >= 100 && distRight < 1200) {
		return EnDirRight;
	}
	else if (diff <= -100 && distLeft < 1200) {
		return EnDirLeft;
	}
	else {
		return EnDirUnknown;
	}
}

void mainAlgorithm() {
	LcdClear();
	LcdGotoXYFont(1, 1);
	LcdStr(FONT_1X, (const byte*)"WALKA");
	//parametry
	float speedMul = 1.2;
	float runSpeed = 400;
	float maxSpeed = 500;

	//stan robota
	EnemyDir enemyDir;
	Mode mode = Seek;

	//inicjacja
	motorSetVel(0, MotorLeft);
	motorSetVel(0, MotorRight);
	motorRunVel(MotorLeft);
	motorRunVel(MotorRight);

	while(!isPushed[LEFT_BUT]) {
		//Czujniki linii
		//oba
		if(lineDetCheck(curDet[0]) && lineDetCheck(curDet[1])) {
			switchDirection();
			enemyDir = EnDirLeft;
			mode = Seek;
		}
		//lewy
		if(lineDetCheck(curDet[0])) {
			switchDirection();
			enemyDir = EnDirLeft;
			mode = Seek;
		}
		//prawy
		if(lineDetCheck(curDet[1])) {
			switchDirection();
			enemyDir = EnDirRight;
			mode = Seek;
		}

		/// Tryb szukania przeciwnika
		if (mode == Seek) {
			EnemyDir curEnemyDir = getEnemyDir();
			if (curEnemyDir == EnDirAhead) {
				mode = Attack;
				enemyDir = EnDirAhead;
			}
			else if (curEnemyDir == EnDirUnknown) {
				// Keep seeking in the old direction
			}
			else if (curEnemyDir == EnDirRight) {
				enemyDir = EnDirRight;
				mode = Attack;
			}
			else if (curEnemyDir == EnDirLeft) {
				enemyDir = EnDirLeft;
				mode = Attack;
			}
		}
		// ustawianie prędkości silników
		if(mode == Seek) {
			if(enemyDir == EnDirLeft) {
				motorSetVel(runSpeed*curSpeedMul*(1/speedMul), curMotor[0]);
				motorSetVel(runSpeed*curSpeedMul*speedMul, curMotor[1]);
			}
			else if(enemyDir == EnDirRight) {
				motorSetVel(runSpeed*curSpeedMul*speedMul, curMotor[0]);
				motorSetVel(runSpeed*curSpeedMul*(1/speedMul), curMotor[1]);
			}
		}
		/// Tryb ataku
		if (mode == Attack) {
			if (enemyDir == EnDirAhead) {
				// TODO Dobrać speedMul
				speedMul = 1.1;
				motorSetVel(runSpeed*speedMul, curMotor[0]);
				motorSetVel(runSpeed*speedMul, curMotor[1]);
			}
			else if (enemyDir == EnDirLeft) {
				speedMul = 1.2;
				motorSetVel(runSpeed*curSpeedMul*(1/speedMul),curMotor[0]);
				motorSetVel(runSpeed*curSpeedMul*speedMul, curMotor[1]);
			}
			else if (enemyDir == EnDirRight) {
				speedMul = 1.2;
				motorSetVel(runSpeed*curSpeedMul*speedMul, curMotor[0]);
				motorSetVel(runSpeed*curSpeedMul*(1/speedMul), curMotor[1]);
			}
			// TODO Dobrać wartość opóźnienia
			sysDelayMs(10);
			EnemyDir curEnemyDir = getEnemyDir();
			if (curEnemyDir == EnDirUnknown) {
				if (enemyDir == EnDirAhead) {
					enemyDir == EnDirLeft;
				}
				mode = Seek;
			}
			else if (curEnemyDir == EnDirAhead) {
				enemyDir = EnDirAhead;
			}
			else if (curEnemyDir == EnDirRight) {
				enemyDir = EnDirRight;
			}
			else if (curEnemyDir == EnDirLeft) {
				enemyDir = EnDirLeft;
			}
		}
	}
	motorShutdown(MotorLeft);
	motorShutdown(MotorRight);
	isPushed[LEFT_BUT] = 0;
}


