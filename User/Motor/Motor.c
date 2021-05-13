#define MOTOR 1
#include "..\CONFIGURATION.h"
int angle_param = 0;
int Speed = 0;
/*Motor Driver Board Pin Initialize*/
void Motor_Brake(void)
{
	TIM_SetCompare1(TIM8,90);
	TIM_SetCompare2(TIM8,90);
}

//TIM_SetCompare1(TIM8,1);时，输出2ms的波
//TIM_SetCompare1(TIM8,10);时，输出1.5ms的波
//TIM_SetCompare1(TIM8,20);时，输出1ms的波


//Speed大于0，正转；Speed小于0，反转
//Speed=0时，电机停转；Speed=10时，占空比为20;Speed=-10时，占空比为0
void Motor_Function(int input_angle_param, int speed) 
{
	u8 full_cycle = 180;//20;//40*0.05ms=2ms 为最大占空比
  Speed = speed;
	int angle_to_turn = 0;
	if(Speed>=0 && input_angle_param>0)
	{
		angle_to_turn = input_angle_param<=(Speed)?input_angle_param:(Speed);
	}
	else if((Speed>=0 && input_angle_param<0))
	{
		angle_to_turn = (-input_angle_param)<=(Speed)?input_angle_param:-(Speed);
	}
	else if((Speed<=0 && input_angle_param>0))
	{
		angle_to_turn = (input_angle_param)<=(-Speed)?input_angle_param:-(Speed);		
	}	
	else if((Speed<=0 && input_angle_param<0))
	{
		angle_to_turn = (-input_angle_param)<=(-Speed)?input_angle_param:(Speed);	
	}

//	printf("speed = %d,angle = %d\n",speed,angle_to_turn);
	if(Speed == 0)
	{
		Motor_Brake();
	}
	else
	{
		//TIM_SetCompare2(TIM8,full_cycle/2-2);
		if(Speed > 0)
		{
			if(angle_to_turn == 0)
			{
				TIM_SetCompare1(TIM8,full_cycle/2-8-Speed);//-speed+angle_to_turn);
				TIM_SetCompare2(TIM8,full_cycle/2-9-Speed);
			}
			else if(angle_to_turn > 0)//?????负数表示右转，正数表示左转
			{
				//Motor_Brake();
				TIM_SetCompare1(TIM8,full_cycle/2+10+Speed);//-speed+angle_to_turn);
				TIM_SetCompare2(TIM8,full_cycle/2-9-Speed);
			}
			else//右转
			{
				//Motor_Brake();
				TIM_SetCompare1(TIM8,full_cycle/2-9-Speed);
				TIM_SetCompare2(TIM8,full_cycle/2+10+Speed);//-Speed-angle_to_turn);	
			}			
		}
//		else
//		{
//			if(angle_to_turn == 0)//负数表示右转，正数表示左转
//			{
//				TIM_SetCompare1(TIM8,full_cycle/2-Speed);
//				TIM_SetCompare2(TIM8,full_cycle/2-Speed);
//			}
//			if(angle_to_turn > 0)//负数表示右转，正数表示左转
//			{
//				TIM_SetCompare1(TIM8,full_cycle/2+Speed);
//				TIM_SetCompare2(TIM8,full_cycle/2-Speed);
//			}
//			else//右转
//			{
//				TIM_SetCompare1(TIM8,full_cycle/2-Speed);
//				TIM_SetCompare2(TIM8,full_cycle/2+Speed);	
//			}					
//		}

	}
}

/*Initialize System*/
void Motor_IN_Pin_Init(void)
{
	//TIM8_PWM_Init(40,3599);//50Hz 一个周期数400个数，每个数占0.05MS
	TIM8_PWM_Init(400,359);//50Hz 一个周期数400个数，每个数占0.05MS
}

/*
差速调整角度
负数表示右转，正数表示左转
输入参数是((int16_t)CHANNEL_HIGHOUTPUT[2]-1500)/50*50;  ，范围是-500~500
angle_param 范围是-10~10
转弯逻辑：左转时，右电机速度不变，左电机减速
*/
void Servo_Set_Angle(int16_t controller_param)
{
	angle_param = (controller_param)/(50);//负数表示右转，正数表示左转
}

