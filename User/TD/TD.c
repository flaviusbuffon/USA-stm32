#define TRANSMITDATA 1
#include "..\CONFIGURATION.h"

/*==============Eastar Protocol 0.2==================*/

/*
�������ܣ�����ǰSTM32����״̬����ֵ����Ϊһ֡����FRAMELENGTH
�����ǰ��0x1122���ͣ������ʱ�Ƚ���0x11���ٽ���0x22
*/

void synthesis_frame(void)
{
	u8 counter;
	u8 CRC_validation = 0;
	uint32_t longtitude = (uint32_t)GPS_PVTData.lon;
	uint32_t latitude = (uint32_t)GPS_PVTData.lat;
	uint16_t Yaw = yaw;
	uint16_t Pitch = pitch;
	uint16_t Roll = roll;
	u8 control_field = 0;
	
	/*����hAcc---ˮƽ����*/
	uint32_t hAcc_in_dm = GPS_PVTData.hAcc / 1000 ;
	if(hAcc_in_dm >= 254) hAcc_in_dm = 0xff;
	/*���������Զ�����control_field�ֶ�*/
	/*��7-6λ��GPS������ʽ*/
	if(SignalFromGPS()!=5) 
	{
		control_field |= (SignalFromGPS())<<6;	//GPS������Ч
	}
	/*��5-4λ����ǰң��״̬*/
	control_field |= current_state << 4;
	
	/*��ʼ��������*/
	//���;���
	for(counter = 0;counter<4;counter++)
	{
		DATAFRAME[2+counter] = (longtitude & (0xff000000>>((counter)*8))) >> (8*(3-counter));
	}
	//����γ��
	for(counter = 0;counter<4;counter++)
	{
		DATAFRAME[2+4+counter] = (latitude & (0xff000000>>((counter)*8))) >> (8*(3-counter));
	}
	//����ƫ����
	for(counter = 0;counter<2;counter++)
	{
		DATAFRAME[2+4+4+counter] = (Yaw & (0xff00>>((counter)*8))) >> (8*(1-counter));
	}	
	//���͸����ǣ������ţ�
	for(counter = 0;counter<2;counter++)
	{
		DATAFRAME[2+4+4+2+counter] = (Pitch & (0xff00>>((counter)*8))) >> (8*(1-counter));
	}	
	//���͹�ת��(������)
	for(counter = 0;counter<2;counter++)
	{
		DATAFRAME[2+4+4+2+2+counter] = (Roll & (0xff00>>((counter)*8))) >> (8*(1-counter));
	}	
	//����ˮƽ����Ԥ��
	DATAFRAME[2+4+4+2+2+2] = (u8)hAcc_in_dm;
	//����չ�Ŀ��ֶ�
	for(counter = 0;counter<3;counter++)
	{
		DATAFRAME[2+4+4+2+2+2+1+counter] = 0x00;
	}	
	//�����ֶ�
	DATAFRAME[2+4+4+2+2+2+4] = control_field;
	
	//��������
	for(counter=0;counter<sizeof(DATAFRAME)/sizeof((u8)0);counter++)
	{
		if(counter>=2&&counter<(FRAMELENGTH-1))
		{
			CRC_validation += DATAFRAME[counter];
		}
		else if(counter==(FRAMELENGTH-1))
		{
			DATAFRAME[(FRAMELENGTH-1)] = CRC_validation;
		}
		#if !USE_USART1_COMM_DMA_TX
		if(WirelessPort_Online)
		{
			if(!Read_AUX_Status())
				USART1_SENDDATA(DATAFRAME[counter]);
		}
		else
		{
			USART1_SENDDATA(DATAFRAME[counter]);
		}
		#endif
	}
	#if USE_USART1_COMM_DMA_TX
		if(!DMA_GetCurrDataCounter(DMA1_Channel4))
		{
			if(DMA_GetFlagStatus(DMA1_FLAG_TC4)!=RESET)
			{
				DMA_ClearFlag(DMA1_FLAG_TC4);//���ͨ�� 4 ������ɱ�־
				USART1_DMA_ENABLE_ONCE(FRAMELENGTH);
			}//else USART1_DMA_ENABLE_ONCE(FRAMELENGTH);					
		}
	#endif
}

u8 resolve_frame_from_Host_computer(void)
{
	u16 counter=0,counter1=0;
	u8 validationFlag=0;
	u8 dataIn=0;
	u8 temp=0;
	u8 sumup=0;
	u8 Header[] = {0x63,0x73};
	u8 CRCvalidate[17];//Э��У�����ܳ���Ϊ16bytes
	//printf("Buff number:%d",Get_Buff_Data_Number());
	if(Get_Buff_Data_Number() >= FRAMELENGTH)//��ȷ���������ڴ洢���㹻�������
	{
		/*ͬ����Ϣ֡��Ѱ��������*/
		for(counter=0;counter<=FRAMELENGTH-1;counter++)
		{
			Read_USART1_ringBuff(&dataIn);
			if(dataIn==Header[counter1])//���λ������ڵ���������ͷ�ļ�һһ�ȶ�
			{
				//printf("counter1=%d,dataIn=%d\n",counter1,dataIn);
				counter1++;
				if(counter1 >= (sizeof(Header)/sizeof((u8)0)))//���ͷ�ļ��ȶԳɹ�����˵��֡��ͬ��
				{
					/*��ȡ��Ϣ֡������CRCУ��*/
						for(counter=0;counter<(sizeof(CRCvalidate)/sizeof((u8)0));counter++)
						{
							Read_USART1_ringBuff(&CRCvalidate[counter]);
//							printf("data %d : %d\n",counter,CRCvalidate[counter]);
							sumup+=CRCvalidate[counter];
						}
						
						Read_USART1_ringBuff(&temp);
						if(sumup==temp) 
						{
							validationFlag=1;
//							printf("\nCRC passed!\n");
						}
						else
						{
							printf("Sent CRC = %d,Calculated CRC = %d",temp,sumup);
						}
						break;
				}
			}else continue;
			
		}
	}
	if(validationFlag)
	{
		/*��ȡ·��ƫ����*/
		routeToGo.yaw = 0;
		for(counter=0;counter<2;counter++)
		{	
			temp = CRCvalidate[counter];
			routeToGo.yaw += (((u16)temp)<<(8*(1-counter)));
		}	
		/*��ȡ·������*/
		routeToGo.distance = CRCvalidate[2];
		routeToGo.distance= 0;
		for(counter=0;counter<2;counter++)
		{	
			temp = CRCvalidate[counter+2];
			routeToGo.distance += (((u16)temp)<<(8*(1-counter)));
		}			
		/*��ȡ����״̬*/
		routeToGo.control_status = CRCvalidate[4];
//		printf("YAW = %d,distance = %d,control_status=%d\n",routeToGo.yaw,routeToGo.distance,routeToGo.control_status);
		return 1;
	}
	return 0;
}	

