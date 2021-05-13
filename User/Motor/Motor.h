#ifndef __Motor
#define __Motor
#include "stm32f10x.h"
	#define FORWARD 1
	#define BACKWARD 2
	#define ENCODER_WIRE_NUMBER 1560
	#define LEFT_WHEEL 2
	#define RIGHT_WHEEL 1
	#define ALL_WHEEL 3
void Motor_Brake(void);
void Motor_IN_Pin_Init(void);
void Motor_Function(int input_angle_param, int speed);
void Servo_Set_Angle(int16_t controller_param);
extern int angle_param ;
extern int Speed;
#endif
