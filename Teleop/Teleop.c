#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S2,     HTPB,           sensorI2CCustom9V)
#pragma config(Motor,  mtr_S1_C1_1,     motorD,       tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     motorE,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     motorF,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     FourBar,        tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Servo,  srvo_S1_C3_1,    servo1,               tServoStandard)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoStandard)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/* This code is for a simple tank drive motor setup with all motors wired + = red, - = black.
A motor controller with the drive motors is wired directly to the NXT input1 (motors D and E
are drive motors.  A second motor controller is daisy chained to the first.  The four bar
motor is wired into motor 2.  A servo controller is daisy chained to the second motor controller.
Two servos are plugged into ports 1 and 2 to move the Vcup.

The HiTechnic protoboard has two limit switches wired into Analog0 and Analog 2 inputs.
The protoboard is plugged into input2 on the NXT.

Two joysticks run this robot.  Joystick 1 is tank drive for the drive motors on the two
joysticks.  Button 5 on joystick one is turbo.  Button 6 on joystick one is slow.

Joystick 2 runs the four bar and the Vcup.  The 2nd joystick is the
power for the f-bar.  Up moves the fourbar up.  Down moves it down.

Buttons 5 and 6 on the 2nd joystick move the Vcup up or down by increments
of 10 servo counts (about 8 degrees).

Debugging information:
line 1: servos, first is #1, second is #2
line 2: drive power: leftpower then rightpower
line 3: 4-bar encoder position
line 4: limit switch signals (0 means OK, 1 means too high, 2 means too low)

*/

#include "JoystickDriver.c"
#include "../common/drivers/hitechnic-protoboard.h"



int fourbarpower = 0;  //motor power for the 4bar motor
int servoposition1 = 128;  //servo positions
int servoposition2 = 122;
int debug = 0;		//set to zero to turn off debug
float leftpower = 0;	//drive motor powers
float rightpower = 0;
float drive_multiplier = 1;  //for turbo & slow speed

int fourbarposition = 1;
const int ArmSpeed = 50;
#include "../common/ArmLibrary.h"

///////////////////////////////////////////MOVES V-CUP UP AND DOWN//////////////////////////////////////////////////////

void vCupPosition() //The funtion which moves the v cup up and down
{
	if(joy2Btn(5) == 1) //If button 5 on the joystick which controls the arm is pressed then do the following...
	{
		servoposition1 += 5;
		if(servoposition1 > 255) servoposition1 = 255;
		servoposition2 = 255 - servoposition1;
	}
	else if(joy2Btn(6) == 1) //If button 6 is pressed then do the following...
	{
		servoposition1 -= 5;
		if(servoposition1 < 0) servoposition1 = 0;
		servoposition2 = 255 - servoposition1;
	}
	MoveVCupToPosition(servoposition2);
	//if(debug){nxtDisplayString(0, "servos: %i, %i", servoposition1, servoposition2);}

}


///////////////////////////////////////SETS TURBO, SLOW, OR REGULAR DRIVE SPEED///////////////////////////////////////////

void drivespeed() //The funtion which allows you to ajust the speed of the drive
{
	if(joy1Btn(5)) // If button 5 on the joystick is pressed then go turbo speed
	{
		drive_multiplier = 1; //multiplies the power by 1, increasing the speed to 200 percent it's normal speed
	}
	else if(joy1Btn(6)) //If button 6 on the joystick is pressed then go slow speed
	{
		drive_multiplier = 0.2;// multiplies the power by .2, decreasing the speed to 40 percent it's normal speed
	}
	else  // if neither of the buttons are being pressed proceed at normal speed
	{
		drive_multiplier = 0.5;
	}
}

///////////////////////////////////////MOVES TO MIDDLE PEG POSITION/////////////////////////////////////////////////////////////////////
void MovePosition2()
{
	if((nMotorEncoder[FourBar]-5700)<-5) //If the four bar is under -5700 then do the following...
	{
		fourbarpower=100; //Four bar goes up
	}
	else if((nMotorEncoder[FourBar]-5700)>5)
	{
		fourbarpower=-100; //fourbar goes down
	}
	else
	{
		fourbarpower=0;
	}
	motor[FourBar] = fourbarpower;
}
//////////////////////////////////////////////////////MOVES TO BOTTOM PEG POSITION/////////////////////////////////////////////////
void MovePosition1()
{

	if(nMotorEncoder[FourBar]<-5)
	{
		fourbarpower=100;
	}
	else if(nMotorEncoder[FourBar]>5)
	{
		fourbarpower=-100;
	}
	else
	{
		fourbarpower=0;
	}
	motor[FourBar] = fourbarpower;
}

/////////////////////////////////////BASIC TANK DRIVE////////////////////////////////////////////////////////////////////
void tankdrive()
{
	rightpower = joystick.joy1_y2*100/128;		//read joystick y values (both sticks)
	leftpower = joystick.joy1_y1*100/128;

	if(abs(rightpower) < 8)			//deadband with threshold of 8

	{
		rightpower = 0;
	}

	if(abs(leftpower) < 8)			//deadband with threshold of 8

	{
		leftpower = 0;
	}
	//if(debug) {nxtDisplayString(1, "drive: %i, %i", leftpower, rightpower);}
	motor[motorD] = -drive_multiplier*leftpower;		//TURN LEFTDRIVE AT leftpower
	motor[motorE] = drive_multiplier*rightpower;		//TURN RIGHTDRIVE AT rightpower
}


//////////////////////////////////////SETS 4-BAR POSITION////////////////////////////////////////////////////////////////
void fourbarlift()
{
	status="fourbarlift";
	fourbarpower = joystick.joy1_y2;
	if(abs(fourbarpower) < 8)			//deadband with threshold of 8
	{
		fourbarpower = 0;
	}
	//fourbarpower = 100;

	if (fourbarpower != 0) {
		//if we're going up
		if (fourbarpower > 0) {
			fourbarposition = MoveArmUp(fourbarposition, ArmSpeed*drive_multiplier);
		}
		//if we're going down
		if (fourbarpower < 0) {
			fourbarposition = MoveArmDown(fourbarposition, ArmSpeed*drive_multiplier);
		}
	}
	//if(debug) {nxtDisplayString(2, "encoder: %i", nMotorEncoder[FourBar]);}
}


//////////////////////////////////////////TASK MAIN///////////////////////////////////////////////////////////////
task main()
{
	bDisplayDiagnostics = false;

	//servo[servo1]=400;
	//servo[servo2]=400;
	nMotorEncoder[FourBar] = 0;
	while(true)  //infinite loop
	{
		getJoystickSettings(joystick);

		if(joy2Btn(1)==1)
		{
			servoposition1=238;
			servoposition2=12;
			servo[servo1]=servoposition1;
			servo[servo2]=servoposition2;
			MovePosition2();
			//if(debug) {nxtDisplayString(6, "Position2");}
		}
		else if(joy2Btn(2)==1)
		{
			servoposition1=238;
			servoposition2=12;
			servo[servo1]=servoposition1;
			servo[servo2]=servoposition2;
			MovePosition1();
			//if(debug) {nxtDisplayString(6, "Position1");}
		}
		else
		{
			vCupPosition();
			fourbarlift();
			//if(debug) {nxtDisplayString(6, "Joystick");}
		}
		drivespeed();
		tankdrive();
		wait1Msec(50);
		eraseDisplay();

	}
}
