#ifndef __GPS
#define __GPS
#include "stm32f10x.h"
u8 UBX_CFG_USART(u32 baudrate);
u8 GPS_SENDDATA(u8 datain);
void USART3_IRQHandler(void);
void usart3_init(u32 baudrate);
void ringBuff_USART3_TEST(void);
u8 UBX_CFG_MODEL(void);
u8 UBX_CFG_DATAOUT(void);
u8 UBX_CFG_RATE(void);
u8 pvtFromGPS(void);
u8 SignalFromGPS(void);
void GPSDATA_Init(void);
void DisplayDataGPS(void);
typedef struct{	
	u8 fixtype;		//GPS��������
	u8 fixFlags;	//GPS�������ͱ�־λ
	u8 numSV;			//���ڵ��������������Ŀ
	int32_t lon;	//����								 degree
	int32_t lat;	//γ��								 degree
	int32_t velN;	//����������ϵ���ٶ� mm/s
	int32_t velE;	//����������ϵ���ٶ� mm/s
	int32_t velD;	//����������ϵ����ٶ� mm/s
	u32 sAcc;			//Ԥ���ٶȾ���				 mm/s
	u32 hAcc;			//Ԥ��ˮƽ����				 mm
	u16 pdop;			//��λ��������  pdop<4,���Ⱥܺ�;4<pdop<7�����ȿɽ���;pdop>7,pdop�ϲ�
}pvtData;
extern pvtData GPS_PVTData;
#endif
