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

enum SeekDir{
	SeekDirLeft,
	SeekDirRight
};

enum Mode{
	Attack,
	Seek
};

enum EnemyDir{
	EnDirLeft,
	EnDirRight
};

int curSpeedMul = 1;

void mainAlgorithm(){
	LcdClear();
	LcdGotoXYFont(1, 1);
	LcdStr(FONT_1X, (const byte*)"WALKA");
	//parametry
	float speedMul = 1.2;
	float runSpeed = 400;
	float maxSpeed = 500;

	//stan robota
	SeekDir seekDir = SeekDirLeft;
	EnemyDir enemyDir = EnDirLeft;
	Mode mode = Seek;

	//inicjacja
	motorSetVel(0, MotorLeft);
	motorSetVel(0, MotorRight);
	motorRunVel(MotorLeft);
	motorRunVel(MotorRight);

	while(!isPushed[LEFT_BUT]){
		//Czujniki linii
		//oba
		if(lineDetCheck(curDet[0]) && lineDetCheck(curDet[1])){
			switchDirection();
			seekDir = SeekDirLeft;
			mode = Seek;
		}
		//lewy
		if(lineDetCheck(curDet[0])){
			switchDirection();
			seekDir = SeekDirLeft;
			mode = Seek;
		}
		//prawy
		if(lineDetCheck(curDet[1])){
			switchDirection();
			seekDir = SeekDirRight;
			mode = Seek;
		}

		/// Tryb szukania przeciwnika
		if (mode == Seek){
			/// Przeciwnik widziany którymkolwiek sharpem
			/*if (adcSharpDist(ShFrontLeft)<1000 || adcSharpDist(ShFrontRight)<1000){
				mode = Attack;
				/// Przeciwnik po lewej stronie
				if (adcSharpDist(ShFrontLeft)<1000 && adcSharpDist(ShFrontRight)>1000){
					enemyDir = EnDirLeft;
				}
				/// Przeciwnik po prawej stronie
				else if (adcSharpDist(ShFrontLeft)>1000 && adcSharpDist(ShFrontRight)<1000){
					enemyDir = Right;
				}
			}*/
		}
		/// Tryb ataku
		if (mode == Attack){

		}

		//ustawianie prędkości silników
		if(mode == Seek){
			if(seekDir == SeekDirLeft){
				motorSetVel(runSpeed*curSpeedMul*(1/speedMul),
							curMotor[0]);
				motorSetVel(runSpeed*curSpeedMul*speedMul,
							curMotor[1]);
			}
			else if(seekDir == SeekDirRight){
				motorSetVel(runSpeed*curSpeedMul*speedMul,
							curMotor[0]);
				motorSetVel(runSpeed*curSpeedMul*(1/speedMul),
							curMotor[1]);
			}
		}
	}

	motorShutdown(MotorLeft);
	motorShutdown(MotorRight);
	isPushed[LEFT_BUT] = 0;
}


void switchDirection(){
    if (curDet[0] == DetFrontLeft){
        curDet[0] = DetRearRight;
        curDet[1] = DetRearLeft;
        curDet[2] = DetFrontRight;
        curDet[3] = DetFrontLeft;
    }
    else if (curDet[0] == DetRearRight){
        curDet[0] = DetFrontLeft;
        curDet[1] = DetFrontRight;
        curDet[2] = DetRearLeft;
        curDet[3] = DetRearRight;
    }

    if (curSharp[0] == ShFrontLeft){
        curSharp[0] = ShRearRight;
        curSharp[1] = ShRearLeft;
        curSharp[2] = ShFrontRight;
        curSharp[3] = ShFrontLeft;
    }
    else if (curSharp[0] == ShRearRight){
        curSharp[0] = ShFrontLeft;
        curSharp[1] = ShFrontRight;
        curSharp[2] = ShRearLeft;
        curSharp[3] = ShRearRight;
    }
    if(curMotor[0] == MotorLeft){
    	curMotor[0] = MotorRight;
    	curMotor[1] = MotorLeft;
    }
    else{
    	curMotor[0] = MotorLeft;
    	curMotor[1] = MotorRight;
	}

    curSpeedMul *= -1;
}


