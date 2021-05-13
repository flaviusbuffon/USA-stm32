#define PATHPLAN 1
#include "..\CONFIGURATION.h"

//Path planning

//������ǰYaw��Ŀ��yaw��������������ٶ�
//��λ��YAW:��������ΧΪ0-360    distance:��λΪ����
void Execute_Planned_Path(u16 currentYAW,u16 nextYAW,u8 distance,u8 control_status,u8 freq)
{
	//printf("\ncurrentYaw=%d,nextyaw=%d,distance=%dcm,\n",currentYAW,nextYAW,distance);
	u8 spinDirection;	//0����˳ʱ��ת��1������ʱ��ת
	int degreesToSpin;
	volatile u8 tolerant_bias = 15; //�����ƫ�������,��λΪ��
	/*������������ʱ������ǶȲ��˸ı���졣*/
	static int16_t delay_counter = 0;
	
	if(delay_counter>=freq/5)//Լÿ100msִ��һ��
	{
		delay_counter = 0;
		/*�����������*/
		//������ת��������ת����
		if(currentYAW>=nextYAW)
		{
			degreesToSpin = currentYAW - nextYAW;
			if(degreesToSpin>=180) spinDirection = CLOCKWISE;
			else spinDirection = COUNTER_CLOCKWISE;
		}
		else
		{
			degreesToSpin =  nextYAW - currentYAW;
			if(degreesToSpin>=180) spinDirection = COUNTER_CLOCKWISE;
			else spinDirection = CLOCKWISE;		
		}
		/*���ݽǶȵ������*/
		/*��Ҫת�ĽǶȴ��ڿ����̵ĽǶ�ֵ*/
		if(degreesToSpin>=tolerant_bias)
		{
			if(degreesToSpin>65)
			{
				switch(spinDirection)
				{
					case CLOCKWISE:Servo_Set_Angle(-500);break;
					case COUNTER_CLOCKWISE:Servo_Set_Angle(500);break;
					default:Servo_Set_Angle(0);break;
				}
			}
			else
			{
				switch(spinDirection)
				{
					case CLOCKWISE:Servo_Set_Angle(degreesToSpin>40?-400:(-(10*degreesToSpin)));break;//degreesToSpin
					case COUNTER_CLOCKWISE:Servo_Set_Angle(degreesToSpin>40?400:((10*degreesToSpin)));break;
					default:Servo_Set_Angle(0);break;
				}			
			}
		}
		/*��Ҫת�ĽǶ�С�ڿ����̵ĽǶ�ֵ*/
		else
		{
			Servo_Set_Angle(0);
		}
	}
	
	if(control_status&0x80)
	{
		Motor_Brake();
	}
	/*���ݾ����������*/
	else if(distance>=200)
	{
		Motor_Function(angle_param,1);
	}
	else
	{
		Motor_Function(angle_param,1);//;distance/100);
	}
	delay_counter ++;
}
