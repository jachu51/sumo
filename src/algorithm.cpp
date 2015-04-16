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
	Retreat,
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
	int32_t distLeftFr = adcSharpDist(curSharp[0]);
	int32_t distRightFr = adcSharpDist(curSharp[1]);
	int32_t distLeftBck = adcSharpDist(curSharp[2]);
	int32_t distRightBck = adcSharpDist(curSharp[3]);
	int32_t diff = distLeftFr - distRightFr;
	int32_t minDist = 100;
	int32_t maxDist = 300;
	if (diff < minDist && diff > -minDist && distLeftFr < maxDist && distRightFr < maxDist) {
		return EnDirAhead;
	}
	else if (diff >= minDist && distRightFr < maxDist) {
		return EnDirRight;
	}
	else if (diff <= -minDist && distLeftFr < maxDist) {
		return EnDirLeft;
	}
	else if(distLeftBck < maxDist || distRightBck < maxDist){
		return EnDirBack;
	}
	else {
		return EnDirUnknown;
	}
}

void startModuleInit(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gpioInit;
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpioInit.GPIO_Speed = GPIO_Speed_2MHz;
	gpioInit.GPIO_Pin = startPin | killPin;
	GPIO_Init(GPIOC, &gpioInit);
}

bool startModuleIsStart(){
	return GPIO_ReadInputDataBit(GPIOC, startPin);
}

bool startModuleIsStop(){
	return (GPIO_ReadInputDataBit(GPIOC, killPin) == Bit_RESET);		//inverted
}

void mainAlgorithm() {
	static const bool startModule = true;

	LcdClear();
	LcdGotoXYFont(1, 1);
	LcdStr(FONT_1X, (const byte*)"CZEKAM NA START");
	//parametry
	static const float speedMul = 1.2;
	static const float runSpeedSeek = 100;
	static const float runSpeedAttack = 200;
	static const float maxSpeed = 500;

	static const int32_t retreatTrigDelay = 1000;	// *10 ms
	static const int32_t retreatStopDelay = 200;	// *10 ms
	static const int32_t attackStopDelay = 50;		// *10 ms

	//stan robota
	EnemyDir enemyDir = EnDirLeft;
	Mode mode = Seek;

	//inicjacja
	motorSetVel(0, MotorLeft);
	motorSetVel(0, MotorRight);
	motorRunVel(MotorLeft);
	motorRunVel(MotorRight);

	if(startModule){
		while(!isPushed[LEFT_BUT] &&
				!startModuleIsStart())
		{

		}
	}

	LcdClear();
	LcdGotoXYFont(1, 1);
	LcdStr(FONT_1X, (const byte*)"WALKA");

	int32_t ctrUnknown = 0;
	int32_t ctrMotCurLimit = 0;
	int32_t ctrRetreat = 0;
	while(!isPushed[LEFT_BUT] &&
			(!startModule  || (startModuleIsStart() &&
								!startModuleIsStop())))
	{

		if(mode == Seek){
			LcdClear();
			LcdGotoXYFont(1, 1);
			LcdStr(FONT_1X, (const byte*)"Seek");
		}
		else if(mode == Retreat){
			LcdClear();
			LcdGotoXYFont(1, 1);
			LcdStr(FONT_1X, (const byte*)"Retreat");
		}
		else if(mode == Attack){
			LcdClear();
			LcdGotoXYFont(1, 1);
			LcdStr(FONT_1X, (const byte*)"Attack");
		}

		if (mode == Seek || mode == Retreat) {
			//Czujniki linii
			//oba
			if(lineDetCheck(curDet[0]) && lineDetCheck(curDet[1])) {
				switchDirection();
				//enemyDir = EnDirLeft;
				mode = Seek;
			}
			//lewy
			if(lineDetCheck(curDet[0])) {
				switchDirection();
				//enemyDir = EnDirLeft;
				mode = Seek;
			}
			//prawy
			if(lineDetCheck(curDet[1])) {
				switchDirection();
				//enemyDir = EnDirRight;
				mode = Seek;
			}
		}
		/// Aktualne położenie przeciwnika
		EnemyDir curEnemyDir = getEnemyDir();
		/// Szukanie - reakcja na odczyt kierunku
		if (mode == Seek) {
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
			else if (curEnemyDir == EnDirBack){
				switchDirection();
			}
		}
		/// Atak - reakcja na odczyt kierunku
		if (mode == Attack) {
			if (curEnemyDir == EnDirUnknown) {
				++ctrUnknown;
				if(ctrUnknown > attackStopDelay){
					if (enemyDir == EnDirAhead) {
						enemyDir = EnDirLeft;
					}
					mode = Seek;
					ctrUnknown = 0;
				}
			}
			else if (curEnemyDir == EnDirAhead) {
				enemyDir = EnDirAhead;
				ctrUnknown = 0;
			}
			else if (curEnemyDir == EnDirRight) {
				enemyDir = EnDirRight;
				ctrUnknown = 0;
			}
			else if (curEnemyDir == EnDirLeft) {
				enemyDir = EnDirLeft;
				ctrUnknown = 0;
			}
			else if (curEnemyDir == EnDirBack){
				switchDirection();
			}

			if(motorIsCurLimited(MotorLeft) || motorIsCurLimited(MotorRight)){
				++ctrMotCurLimit;
				if(ctrMotCurLimit > retreatTrigDelay){
					switchDirection();
					mode = Retreat;
					ctrMotCurLimit = 0;
				}
			}
			else{
				ctrMotCurLimit = 0;
			}
		}
		// Odwrot - licznik czasu
		if(mode == Retreat){
			++ctrRetreat;
			if(ctrRetreat > retreatStopDelay){
				mode = Seek;
				ctrRetreat = 0;
			}
		}

		// Szukanie - ustawianie prędkości silników
		if(mode == Seek || mode == Retreat) {
			if(enemyDir == EnDirLeft) {
				motorSetVel(runSpeedSeek*curSpeedMul*(1/speedMul), curMotor[0]);
				motorSetVel(runSpeedSeek*curSpeedMul*speedMul, curMotor[1]);
			}
			else if(enemyDir == EnDirRight) {
				motorSetVel(runSpeedSeek*curSpeedMul*speedMul, curMotor[0]);
				motorSetVel(runSpeedSeek*curSpeedMul*(1/speedMul), curMotor[1]);
			}
		}
		/// Atak - ustawianie prędkości silników
		if (mode == Attack) {
			if (enemyDir == EnDirAhead) {
				motorSetVel(runSpeedAttack*curSpeedMul*speedMul, curMotor[0]);
				motorSetVel(runSpeedAttack*curSpeedMul*speedMul, curMotor[1]);
			}
			else if (enemyDir == EnDirLeft) {
				motorSetVel(runSpeedAttack*curSpeedMul*(1/speedMul),curMotor[0]);
				motorSetVel(runSpeedAttack*curSpeedMul*speedMul, curMotor[1]);
			}
			else if (enemyDir == EnDirRight) {
				motorSetVel(runSpeedAttack*curSpeedMul*speedMul, curMotor[0]);
				motorSetVel(runSpeedAttack*curSpeedMul*(1/speedMul), curMotor[1]);
			}
		}
		sysDelayMs(10);
	}
	motorShutdown(MotorLeft);
	motorShutdown(MotorRight);
	isPushed[LEFT_BUT] = 0;
}


