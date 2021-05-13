#define LOOP 1
#include "..\CONFIGURATION.h"

/*���ȶ���ߵ�ϵͳʱ��Ƭ���ֺ���
˵����
Ϊ��ϵͳЭ���������ÿ��ʱ��Ƭ�����������ռ��ʱ�����ֵ�����ǵ��жϵ�ͻ���¼������������ƣ�
50Hz:  T20<=T20-T10=15ms
10Hz:  T100<=T100-T20=80ms
1Hz:	 T1000<=T1000-T100=900ms
*/
u8 temp_timer = 0;
void Loop(void)
{
	/*--------1Hz������---------*/
	if(Mission.Hz1)
	{
		feed_dog();//ι��
		switch(current_state)
		{
			/*Limited State Machine:������������״̬*/
			case STATE_NORMALLY_RUNNING:
			{
				if(RaspiOfflineIndicator>0) RaspiOfflineIndicator--;
			}
			case STATE_MANUAL_CTRL:	
			case STATE_COLLECTING_POINTS:
			{
				if(MadgwickGainAutoDecend >= MadgwickGainDecendTime) MadgwickGainAutoDecend = MadgwickGainDecendTime;
				else MadgwickGainAutoDecend ++;//����ʹMadgwick�����Զ���С��
				LED_BRINK();	
				break;
			}				
			/*Limited State Machine:����״̬*/
			case STATE_DEBUGGING:
			{
				//PrintAttitudeData();
				break;
			}
		}
		Mission.Hz1 = 0;
	}
	
	/*--------5Hz������---------*/
	if(Mission.Hz5)
	{
		///*===*/CHANGE_STATE_TO(STATE_DEBUGGING);/*===*/
		/*===*/CHANGE_STATE_TO(Analyze_Controller_Msg());/*===*/
		/*======================================*/
		switch(current_state)
		{
			/*Limited State Machine:������������״̬*/
			case STATE_NORMALLY_RUNNING:
			{
				#if !USE_USART1_COMM_DMA_RX
					if(resolve_frame_from_Host_computer()) 
					{
						MISSION_LED_BRINK();
						RaspiOfflineIndicator = MotorLockTimeAfterRaspiOffline;
					}
				#endif
			}
			case STATE_MANUAL_CTRL:
			case STATE_COLLECTING_POINTS:
			{
				synthesis_frame();//����λ�����͵�ǰ��̬����
				break;
			}
			/*Limited State Machine:����״̬*/
			case STATE_DEBUGGING:
			{
				//Motor_Brake();
				//TIM3_4channel_process();
				//printf("\nTim3-CH1:%d\n",CHANNEL_HIGHOUTPUT[3]);
				resolve_frame_from_Host_computer();
				MISSION_LED_BRINK();
				break;
			}
		}
		Mission.Hz5 = 0;
	}
	
	/*--------10Hz������---------*/
	if(Mission.Hz10)
	{
		TIM3_4channel_process();
		/*===����״̬����������10Hz��Ƶ������===*/

		temp_timer ++;
		switch(current_state)
		{
			/*Limited State Machine:������������״̬*/
			case STATE_NORMALLY_RUNNING:
			{
				if(RaspiOfflineIndicator && SignalFromGPS() && Motor_Enabled && temp_timer <= 5) 
				{
					Execute_Planned_Path(yaw,routeToGo.yaw,routeToGo.distance,routeToGo.control_status,10);
				}
				else
				{
					if(temp_timer >= 15) temp_timer = 0;
					Servo_Set_Angle(0);
					Motor_Brake();
				}		
			}
			case STATE_COLLECTING_POINTS:
			case STATE_MANUAL_CTRL:
			{
				#if !USE_USART3_GPS_DMA_RX
					pvtFromGPS();
				#endif
				receiveComData();
				dataFrom6050();
				filter_IMU_data(10);
				break;
			}
			/*Limited State Machine:����״̬*/
			case STATE_DEBUGGING:
			{
				receiveComData();
				dataFrom6050();
				filter_IMU_data(10);
				//print_IMU_data();
				break;
			}
		}
		Mission.Hz10 = 0;
	}
	
	/*--------50Hz������---------*/
	if(Mission.Hz50)
	{
		switch(current_state)
		{
			case STATE_MANUAL_CTRL:
			case STATE_COLLECTING_POINTS:
			case STATE_NORMALLY_RUNNING:
			case STATE_DEBUGGING:
			{
				MadgwickAHRSupdate(pAngleSpeed.channelX,pAngleSpeed.channelY,pAngleSpeed.channelZ, \
													pAcceleration.channelX,pAcceleration.channelY,pAcceleration.channelZ,\
													pCompass_Main.channelX,pCompass_Main.channelY,pCompass_Main.channelZ,\
													(MadgwickGainDecendTime-MadgwickGainAutoDecend)+BETA);
				Convert_Quaternion_To_Euler();	
				break;
			}
		}
		Mission.Hz50 = 0;
	}
}

/*������������ʼ������*/
void TIM5_STSTEM_PULSE_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	TIM_DeInit(TIM5);
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period = 100;									//5ms��ʱ��+1
	TIM_TimeBaseStructure.TIM_Prescaler = (7200)/APB1DIV-1;	//0.05ms�ж�һ��
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	//�ж����ȼ� NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; //TIM5 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 1 ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //�����ȼ� 1 ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure); //��ʼ�� NVIC �Ĵ���
	TIM_Cmd(TIM5, ENABLE); //ʹ�� TIM5
	TIM_ARRPreloadConfig(TIM5,ENABLE);//�Զ���װ��ʹ��
	TIM_Cmd(TIM5,ENABLE);
	system_pulse = 1;
	Mission.Hz1 = 0;Mission.Hz10 = 0;Mission.Hz50 = 0;
}


/*
�������systick������
*/
void TIM5_IRQHandler(void) //TIM5 �ж�
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) //��� TIM5 �����жϷ������
	{
		/*һ���ߵ�ƽ�����ʾ��Ӧ���������*/
		system_pulse++;
		if((system_pulse+1)%(4)==0)
		{
			if((system_pulse+1)%(20)==0)
			{
				if((system_pulse+1)%(40)==0)
				{
						if((system_pulse+1)%(200)==0)
						{
							Mission.Hz1 = 1;
							system_pulse = 0;
						}
						Mission.Hz5 = 1;
				}
				Mission.Hz10 = 1;
			}
			Mission.Hz50 = 1;
		}
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update); //��� TIM5 �����жϱ�־
	}
}

u8 Analyze_Controller_Msg(void)
{
	/*ͨ��0Ϊģʽѡ��
	ͨ��1Ϊ���ʹ�ܰ�ť
	ͨ��2Ϊ����ת��
	ͨ��3Ϊ����ɲ��*/
	u16 bottom_limit = 1200;
	u16 upper_limit = 1800;
	u8 Status = STATE_MANUAL_CTRL;
	int16_t angle  = ((int16_t)CHANNEL_HIGHOUTPUT[2]-1500)/50*50;
	int speed = ((int16_t)CHANNEL_HIGHOUTPUT[3]-1500)/50;
//	printf("speed = %d,angle = %d\n",speed,angle);	
		if(CHANNEL_HIGHOUTPUT[0]<=bottom_limit)
		{
			Status = STATE_COLLECTING_POINTS;
		}
		else if(CHANNEL_HIGHOUTPUT[0]<=upper_limit)
		{
			Status = STATE_NORMALLY_RUNNING;
		}
		else
		{
			Status = STATE_MANUAL_CTRL;
		}
		if(CHANNEL_HIGHOUTPUT[1]<=bottom_limit)
		{//֪ͨ��λ������GPS�����ת��
			Motor_Enabled = FALSE;
		}
		else if(CHANNEL_HIGHOUTPUT[1]<=upper_limit)
		{
			Motor_Enabled = FALSE;
		}
		else
		{
			Motor_Enabled = TRUE;
		}

	#if USE_BRUSHED_MOTOR
		if(Motor_Enabled)
		{
			if(Status==STATE_MANUAL_CTRL||Status==STATE_COLLECTING_POINTS)
			{
				MISSION_LED_BRINK();
				if(CHANNEL_HIGHOUTPUT[3]>=1700||CHANNEL_HIGHOUTPUT[3]<=1300)
					Motor_Function(ALL_WHEEL,Minimize_Greatest_Error_Increment_PID(Read_Speed_left_wheel(),Read_Speed_right_wheel(),(0.85*MOTOR_MAX_SPEED*speed)));
				else
					Motor_Brake(ALL_WHEEL);
				
				if(angle>=2||angle<=-2)
					Servo_Set_Angle(angle);
				else
					Servo_Set_Angle(0);				
			}
		}
		else
		{
			Servo_Set_Angle(0);
			Motor_Brake(ALL_WHEEL);			
		}
	#else
		if(Motor_Enabled)
		{
			if(Status==STATE_MANUAL_CTRL)
			{
				MISSION_LED_BRINK();
			}
			if(Status==STATE_MANUAL_CTRL||Status==STATE_COLLECTING_POINTS)
			{
				if(angle>=2||angle<=-2)
					Servo_Set_Angle(angle);
				else
					Servo_Set_Angle(0);
				
				if(CHANNEL_HIGHOUTPUT[3]>=1530||CHANNEL_HIGHOUTPUT[3]<=1470)
					Motor_Function(angle_param,speed);
				else
					Motor_Brake();
			}
		}
		else
		{
			Servo_Set_Angle(0);
			Motor_Brake();			
		}	
	#endif
		return Status;
}
