#define IMU 1
#include "..\CONFIGURATION.h"


#if GY901_nMPU6050
/*===GY901ģ�����===*/    //MPU6050ģ�����������档
const u8 IMU_output_cfg[] = {0xFF,0xAA,0x02,0x16,0x00};	//IMU��������ٶȡ����ٶȡ��ų���Ϣ���ϵ�Ĭ�����á�
const u8 IMU_rate_cfg[] = {0xFF,0xAA,0x03,0x06,0x00};	//���ûش����ʣ�0x08  50Hz  0X06  10Hz
const u8 IMU_baud_cfg[] = {0xFF,0xAA,0x04,0x09,0x00};	//���ô��ڲ�����921600���ϵ��ʼΪ9600
const u8 IMU_save_cfg[] = {0xFF,0xAA,0x00,0x00,0x00};	//���浱ǰ����



u8 dataFromGY901(void)
{
	u8 data[FRAME_LENGTH];
	u8 sum;
	u16 counter,counter1=0,counter2;
	u8 Acceleration_flag=0,AngleSpeed_flag=0,Angle_flag=0,Compass_Main_flag=0;
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);//�ڴ�������ʱ�رպ������ݵ����룬���������ݳ��������
	for(counter=0;counter<RINGBUFF_LEN;counter++)//�����������λ�����
	{
		if(Read_IMU_ringBuff(&data[counter1]))//���λ������ǿ�
		{
			if(data[0]==0x55)//��־һ֡�ѿ�ʼ
			{
				counter1++;
				if(counter1==FRAME_LENGTH) 
				{	
					counter1 = 0; //һ֡�����Ѵ�����
					if((Acceleration_flag||!ACCESS_Acceleration)\
							&&(AngleSpeed_flag||!ACCESS_Angle_speed)\
							&&(Angle_flag||!ACCESS_Angle)\
							&&(Compass_Main_flag||!ACCESS_Mag)) return 1;//�������ݶ�ȡ���
					switch(data[1])
					{
						case 0x51:
						{
							if(!Acceleration_flag)//�������ٶ�����δ�����ʱ
							{
								/*CRCУ��*/
								sum = 0;
								for(counter2=0;counter2<FRAME_LENGTH;counter2++)
								{
									sum+= data[counter2];
								}
								if(sum==data[FRAME_LENGTH-1])
								{
									/*У��ͨ��*/
									eAcceleration.channelX = (float)(((int16_t)data[3]<<8)|data[2])/(float)(32768*16*G);
									eAcceleration.channelY = (float)(((int16_t)data[5]<<8)|data[4])/(float)(32768*16*G);
									eAcceleration.channelX = (float)(((int16_t)data[7]<<8)|data[6])/(float)(32768*16*G);
									Acceleration_flag = 1;
								}
								else continue;
							}
							else continue;
						}
						case 0x52:
						{
							if(!AngleSpeed_flag)//�������ٶ�����δ�����ʱ
							{
								/*CRCУ��*/
								sum = 0;
								for(counter2=0;counter2<FRAME_LENGTH;counter2++)
								{
									sum+= data[counter2];
								}
								if(sum==data[FRAME_LENGTH-1])
								{
									/*У��ͨ��*/
									eAngleSpeed.channelX = (float)(((int16_t)data[3]<<8)|data[2])/(float)(32768*2000);
									eAngleSpeed.channelY = (float)(((int16_t)data[5]<<8)|data[4])/(float)(32768*2000);
									eAngleSpeed.channelX = (float)(((int16_t)data[7]<<8)|data[6])/(float)(32768*2000);
									AngleSpeed_flag = 1;
								}
								else continue;								
							}
							else continue;						
						}
						case 0x53:
						{
							if(!Angle_flag)//�������ٶ�����δ�����ʱ
							{
								/*CRCУ��*/
								sum = 0;
								for(counter2=0;counter2<FRAME_LENGTH;counter2++)
								{
									sum+= data[counter2];
								}
								if(sum==data[FRAME_LENGTH-1])
								{
									/*У��ͨ��*/
									eAngle.Roll = (float)(((int16_t)data[3]<<8)|data[2])/(float)(32768*180);
									eAngle.Pitch = (float)(((int16_t)data[5]<<8)|data[4])/(float)(32768*180);
									eAngle.Yaw = (float)(((int16_t)data[7]<<8)|data[6])/(float)(32768*180);
									Angle_flag = 1;
								}
								else continue;											
							}
							else continue;						
						}
						case 0x54:
						{
							if(!Compass_Main_flag)//�������ٶ�����δ�����ʱ
							{
								/*CRCУ��*/
								sum = 0;
								for(counter2=0;counter2<FRAME_LENGTH;counter2++)
								{
									sum+= data[counter2];
								}
								if(sum==data[FRAME_LENGTH-1])
								{
									/*У��ͨ��*/
									eCompass_Main.channelX = (((int16_t)data[3]<<8)|data[2]);
									eCompass_Main.channelY = (((int16_t)data[5]<<8)|data[4]);
									eCompass_Main.channelZ = (((int16_t)data[7]<<8)|data[6]);
									Compass_Main_flag = 1;
								}
								else continue;	/*У�鲻ͨ��*/								
							}
							else continue;						
						}
						default: 
						{//�������0x55�����Byte�������Ϸ�Χ�ڣ�������ͬ������֡��
							counter1 = 0;
							continue;
						}
					}
				} else continue;
			} else continue;
		}
		else return 0;//���λ�������
	}
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	return 0xff;
}

void ringBuff_USART2_TEST(void)
{
	u8 receivedData,counter;
	for(counter=0;counter<FRAME_LENGTH;counter++)
	{
		if(Read_IMU_ringBuff(&receivedData))
		{
			printf("%c",receivedData);
		}//else continue;
	}

}
void usart2_init(u32 baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//ʹ������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//���ڸ�λ
	USART_DeInit(USART2);
	//GPIOģʽ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//��TX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//��RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//��������???
	GPIO_Init(GPIOA,&GPIO_InitStructure);			

	//��������ʱ�����ý����ж�
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;		//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	

	//��ʼ������
	USART_InitStructure.USART_BaudRate = baudrate;	//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ����ֹλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode =USART_Mode_Rx|USART_Mode_Tx; //�շ�ģʽ
	USART_Init(USART2,&USART_InitStructure);
	
	//����#����#�ж�
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	//ʹ�ܴ���
	USART_Cmd(USART2,ENABLE);
}

void USART2_IRQHandler(void)//startup_stm32f10x_hd.s�ļ��ж����USART3�жϺ������
{
	//u8 datain;
	if(USART_GetITStatus(USART2,USART_IT_RXNE))
	{
		Write_IMU_ringBuff(USART_ReceiveData(USART2));
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
	}
	
	/*�����Ҫ�õ�USART���գ������Ǹ��ص㣡*/
	if(USART_GetFlagStatus(USART2, USART_FLAG_ORE))
	{
		USART_ReceiveData(USART2);
		USART_ClearFlag(USART2,USART_FLAG_ORE);
	}
}

u8 IMU_SENDDATA(u8 datain)
{
	u16 timer=0;
	USART_SendData(USART2, datain); //�򴮿�2/IMU ��������
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET)
	{
		timer++;
		if(timer>0xff) 
		{
			timer = 0;
			return 0;
		}
	}
	return 1;
}


u8 IMU_CFG_OUTPUT(void)
{
	u8 counter;
	u8 engagedMemory=0;
	IMU_SENDDATA(0x0A);//����һ�������ַ�����ֹ��λ
	engagedMemory=sizeof(IMU_output_cfg);//�����������
	for(counter=0;counter<engagedMemory;counter++)
	{
		if(!IMU_SENDDATA(IMU_output_cfg[counter]))
		{
			return 0;
		}
	}
	IMU_SENDDATA(0x0A);//����һ�������ַ�����ֹ��λ
	engagedMemory=sizeof(IMU_save_cfg);//��������
	for(counter=0;counter<engagedMemory;counter++)
	{
		if(!IMU_SENDDATA(IMU_save_cfg[counter]))
		{
			return 0;
		}
	}
	return 1;
}

u8 IMU_CFG_RATE(void)
{
	u8 counter;
	u8 engagedMemory=0;
	IMU_SENDDATA(0x0A);//����һ�������ַ�����ֹ��λ
	engagedMemory=sizeof(IMU_rate_cfg);//�����������
	for(counter=0;counter<engagedMemory;counter++)
	{
		if(!IMU_SENDDATA(IMU_rate_cfg[counter]))
		{
			return 0;
		}
	}
	IMU_SENDDATA(0x0A);//����һ�������ַ�����ֹ��λ
	engagedMemory=sizeof(IMU_save_cfg);//��������
	for(counter=0;counter<engagedMemory;counter++)
	{
		if(!IMU_SENDDATA(IMU_save_cfg[counter]))
		{
			return 0;
		}
	}	
	return 1;
}

#elif !GY901_nMPU6050
/*==========MPU6050ģ������=========*/
void MPU6050_Write_Reg(uint8_t regAddr, uint8_t regData)
{
	IIC_Start();
	IIC_Send_Byte(DEV_ADDR<<1|0x00);
	IIC_Wait_Ack();
	IIC_Send_Byte(regAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(regData);
	IIC_Wait_Ack();
	IIC_Stop();	
}

u8 MPU6050_Read_Reg(uint8_t regAddr)
{
	u8 regData;
	IIC_Start();
	IIC_Send_Byte(DEV_ADDR<<1);
	IIC_Wait_Ack();
	IIC_Send_Byte(regAddr);
	IIC_Wait_Ack();
	IIC_Stop();
	IIC_Start();
	IIC_Send_Byte(DEV_ADDR<<1|0x01);
	IIC_Wait_Ack();
	regData = IIC_Read_Byte(0);
	IIC_Stop();
	return regData;
}

int16_t MPU6050_Get_Data(uint8_t regAddr)
{
    u8 Data_H, Data_L;
    int16_t data;
    
    Data_H = MPU6050_Read_Reg(regAddr);
    Data_L = MPU6050_Read_Reg(regAddr + 1);
    data = (Data_H << 8) | Data_L;  // �ϳ����� 
    return data;
}
u8 verifyMPU6050(void)
{
	u8 receiveData=0xFF;
	receiveData = MPU6050_Read_Reg(WHO_AM_I);
	if(receiveData==0x68) return 1;
	else return 0;
}

void MPU6050_Init(void)
{   
    MPU6050_Write_Reg(PWR_MGMT_1, 0x00);    //�������״̬     
    MPU6050_Write_Reg(SMPLRT_DIV, 0x04);    //�����ǲ����ʣ�����ֵ��0x04(200Hz)     
    MPU6050_Write_Reg(CONFIG, 0x04);        //��ͨ�˲�Ƶ�ʣ�����ֵ��0x06(5Hz)     
    MPU6050_Write_Reg(GYRO_CONFIG, 0x00);   //�������Լ켰������Χ������ֵ��0x00(���Լ죬250deg/s)     
    MPU6050_Write_Reg(ACCEL_CONFIG, 0x01);  //���ټ��Լ졢������Χ����ͨ�˲�Ƶ�ʣ�����ֵ��0x01(���Լ죬2G��5Hz) 
}

void dataFrom6050(void)
{
	eAngleSpeed.channelX = (float)(MPU6050_Get_Data(GYRO_XOUT_H)/(131*57.3));//131.072*PI/180�Ż�Ϊ��ߵ�ֵ��������ת��Ϊ����/s
	eAngleSpeed.channelY = (float)(MPU6050_Get_Data(GYRO_YOUT_H)/(131*57.3));
	eAngleSpeed.channelZ = (float)(MPU6050_Get_Data(GYRO_ZOUT_H)/(131*57.3));
	eAcceleration.channelX = (float)MPU6050_Get_Data(ACCEL_XOUT_H)/16384;
	eAcceleration.channelY = (float)MPU6050_Get_Data(ACCEL_YOUT_H)/16384;
	eAcceleration.channelZ = (float)MPU6050_Get_Data(ACCEL_ZOUT_H)/16384;
}

/*�����ɴ����Ƽ����ƫ����*/

void SendYawFromCompass(void)
{
	float fhi_local;
	int16_t yaw = 0;
	fhi_local = PI-atan2((float)pCompass_Main.channelY,(float)pCompass_Main.channelX);
//	yaw = (int16_t)((fhi_local)*180/PI - Magnatic_Static_Bias) - 90;
//	if(yaw<0) yaw = 360+yaw;
	yaw = (int16_t)((fhi_local)*180/PI - Magnatic_Static_Bias);
	printf("yaw=%d\n",yaw);
//	printf("yaw=%d\n",(int)( atan2((double)pCompass_Main.channelY,(double)pCompass_Main.channelX) * (180 / 3.14159265) + 180));
}

void print_IMU_data(void)
{
	printf("\n====IMU DATA====");
	printf("\nACCEL_X: %.2f ",eAcceleration.channelX );
  printf("ACCEL_Y: %.2f ", eAcceleration.channelY);
  printf("ACCEL_Z: %.2f\n",eAcceleration.channelZ);
	//131.072����1deg/s
	printf("GYRO_X: %.2f ", eAngleSpeed.channelX);
  printf("GYRO_Y: %.2f ", eAngleSpeed.channelY);
  printf("GYRO_Z: %.2f\n", eAngleSpeed.channelZ);	
	printf("MAG_X: %d ", eCompass_Main.channelX);
  printf("MAG_Y: %d ", eCompass_Main.channelY);
  printf("MAG_Z: %d\n", eCompass_Main.channelZ);	
	printf("================\n");
}
/*
�����Ǿ�Ư��������������ʱ��ȡ100�����ݣ�ȡƽ��ֵ��Ϊ������
*/
void Caliberate_Gyro(u16 number_of_dataset)
{
	u16 counter;
	for(counter=0;counter<number_of_dataset;counter++)
	{
		dataFrom6050();
		delay_ms(2);
		Gyro_Cancelation.channelX +=eAngleSpeed.channelX;
		Gyro_Cancelation.channelY +=eAngleSpeed.channelY;
		Gyro_Cancelation.channelZ +=eAngleSpeed.channelZ;
	}
	Gyro_Cancelation.channelX /= (float)number_of_dataset;
	Gyro_Cancelation.channelY /= (float)number_of_dataset;
	Gyro_Cancelation.channelZ /= (float)number_of_dataset;
}

/*
�����ƾ�Ư��������������ʱ��ȡ100�����ݣ�ȡƽ��ֵ��Ϊ������
*/
void Caliberate_Mag(void)
{
	int16_t channel_X_Max=7100,channel_X_Min=-1493;
	int16_t channel_Y_Max=7296,channel_Y_Min=-1000;
	int16_t channel_Z_Max=5371,channel_Z_Min=-1600;
	
	Compass_Hard_Iron_Cancellation.ratioX = 1;
	Compass_Hard_Iron_Cancellation.ratioY = (channel_X_Max-channel_X_Min)/(channel_Y_Max-channel_Y_Min);
	Compass_Hard_Iron_Cancellation.ratioZ = (channel_X_Max-channel_X_Min)/(channel_Z_Max-channel_Z_Min);
	
	Compass_Hard_Iron_Cancellation.channelX = (channel_X_Max+channel_X_Min)/2;
	Compass_Hard_Iron_Cancellation.channelY = (channel_Y_Max+channel_Y_Min)/2;
	Compass_Hard_Iron_Cancellation.channelZ = (channel_Z_Max+channel_Z_Min)/2;
}

#endif
