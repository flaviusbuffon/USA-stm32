#define COMPASS 1
#include "..\CONFIGURATION.h"

/*Ĭ�ϣ������� HMC5883LL 7 λ�ӻ���ַΪ 0x3C ��д��������� 0x3D �Ķ���������*/

u8 verifyCompass(void)
{
	u8 receiveData=0xFF;
	IIC_Start();
	IIC_Send_Byte(0x3C);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x0A);
	IIC_Wait_Ack();
	IIC_Stop();
	//�ƶ�5883�ڲ�ָ���ƶ���0x0A
	IIC_Start();
	IIC_Send_Byte(0x3D);
	IIC_Wait_Ack();
	receiveData = IIC_Read_Byte(0);//nACK
	IIC_Stop();
	if(receiveData=='H') return 1;
	else return 0;
}

u8 checkDataReady(void)
{
	u8 receiveData=0;
	IIC_Start();
	IIC_Send_Byte(0x3C);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x09);
	IIC_Wait_Ack();
	IIC_Stop();
	//�ƶ�5883�ڲ�ָ���ƶ���0x0A
	IIC_Start();
	IIC_Send_Byte(0x3C|0x01);
	IIC_Wait_Ack();
	receiveData = IIC_Read_Byte(0);//nACK
	IIC_Stop();
	if((receiveData&0x01)==0x01) return 1;
	else return 0;
}

void cfgCompassReg(void)
{//�شų�ǿ�ȴ�Լ��0.5-0.6��˹,������Ҫ����
	IIC_Start();
	IIC_Send_Byte(0x3C);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);
	IIC_Wait_Ack();
	IIC_Send_Byte(REGISTERA);
	IIC_Wait_Ack();
	IIC_Send_Byte(REGISTERB);
	IIC_Wait_Ack();
	IIC_Send_Byte(REGISTERMODE);
	IIC_Wait_Ack();
	IIC_Stop();		
}

u8 receiveComData(void)
{
	u8 counter;
	u8 datain[6];
	if(!checkDataReady()) return 0;
	IIC_Start();
	IIC_Send_Byte(0x3C);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x03);
	IIC_Wait_Ack();
	IIC_Stop();	
	
	IIC_Start();
	IIC_Send_Byte(0x3D);
	IIC_Wait_Ack();
	for(counter=0;counter<6;counter++)
	{
		if(counter>=5) datain[counter] = IIC_Read_Byte(0);//nACK
		else datain[counter] = IIC_Read_Byte(1);
	}
	IIC_Stop();
	
	eCompass_Main.channelX = (int16_t)((datain[0]<<8)+datain[1]);
	eCompass_Main.channelZ = (int16_t)((datain[2]<<8)+datain[3]);
	eCompass_Main.channelY = (int16_t)((datain[4]<<8)+datain[5]);
	//printf("MAGX%dY%dZ%d",eCompass_Main.channelX,eCompass_Main.channelY,eCompass_Main.channelZ);
	return 1;
}

