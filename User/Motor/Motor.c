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

//TIM_SetCompare1(TIM8,1);ʱ�����2ms�Ĳ�
//TIM_SetCompare1(TIM8,10);ʱ�����1.5ms�Ĳ�
//TIM_SetCompare1(TIM8,20);ʱ�����1ms�Ĳ�


//Speed����0����ת��SpeedС��0����ת
//Speed=0ʱ�����ͣת��Speed=10ʱ��ռ�ձ�Ϊ20;Speed=-10ʱ��ռ�ձ�Ϊ0
void Motor_Function(int input_angle_param, int speed) 
{
	u8 full_cycle = 180;//20;//40*0.05ms=2ms Ϊ���ռ�ձ�
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
			else if(angle_to_turn > 0)//?????������ʾ��ת��������ʾ��ת
			{
				//Motor_Brake();
				TIM_SetCompare1(TIM8,full_cycle/2+10+Speed);//-speed+angle_to_turn);
				TIM_SetCompare2(TIM8,full_cycle/2-9-Speed);
			}
			else//��ת
			{
				//Motor_Brake();
				TIM_SetCompare1(TIM8,full_cycle/2-9-Speed);
				TIM_SetCompare2(TIM8,full_cycle/2+10+Speed);//-Speed-angle_to_turn);	
			}			
		}
//		else
//		{
//			if(angle_to_turn == 0)//������ʾ��ת��������ʾ��ת
//			{
//				TIM_SetCompare1(TIM8,full_cycle/2-Speed);
//				TIM_SetCompare2(TIM8,full_cycle/2-Speed);
//			}
//			if(angle_to_turn > 0)//������ʾ��ת��������ʾ��ת
//			{
//				TIM_SetCompare1(TIM8,full_cycle/2+Speed);
//				TIM_SetCompare2(TIM8,full_cycle/2-Speed);
//			}
//			else//��ת
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
	//TIM8_PWM_Init(40,3599);//50Hz һ��������400������ÿ����ռ0.05MS
	TIM8_PWM_Init(400,359);//50Hz һ��������400������ÿ����ռ0.05MS
}

/*
���ٵ����Ƕ�
������ʾ��ת��������ʾ��ת
���������((int16_t)CHANNEL_HIGHOUTPUT[2]-1500)/50*50;  ����Χ��-500~500
angle_param ��Χ��-10~10
ת���߼�����תʱ���ҵ���ٶȲ��䣬��������
*/
void Servo_Set_Angle(int16_t controller_param)
{
	angle_param = (controller_param)/(50);//������ʾ��ת��������ʾ��ת
}

