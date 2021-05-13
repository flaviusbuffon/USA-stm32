#define COMPASS 1
#include "..\CONFIGURATION.h"
#define QMC5883_REG_DATA       0x00
#define QMC5883_REG_OUT_X_L    0x00
#define QMC5883_REG_OUT_X_M    0x01
#define QMC5883_REG_OUT_Y_L    0x02
#define QMC5883_REG_OUT_Y_M    0x03
#define QMC5883_REG_OUT_Z_L    0x04
#define QMC5883_REG_OUT_Z_M    0x05
 
#define QMC5883_REG_STATUS     0x06
   #define QMC5883_DRDY_BIT0      //0: no new data, 1: new data is ready
   #define QMC5883_OVL_BIT1       //0: normal,      1: data overflow
   #define QMC5883_DOR_BIT2       //0: normal,      1: data skipped for reading
   
#define QMC5883_REG_TEMP_OUT_L 0x07
#define QMC5883_REG_TEMP_OUT_H 0x08
 
#define QMC5883_REG_CTRL1      0x09
   #define QMC5883_CMD_MODE_STANDBY     0x00  //mode 
   #define QMC5883_CMD_MODE_CON         0x01
   #define QMC5883_CMD_ODR_10HZ         0x00  //Output Data Rate
   #define QMC5883_CMD_ODR_50HZ         0x04
   #define QMC5883_CMD_ODR_100HZ        0x08
   #define QMC5883_CMD_ODR_200HZ        0x0C
   #define QMC5883_CMD_RNG_2G           0x00  //Full Scale
   #define QMC5883_CMD_RNG_8G           0x10    
   #define QMC5883_CMD_OSR_512          0x00  //Over Sample Ratio
   #define QMC5883_CMD_OSR_256          0x40    
   #define QMC5883_CMD_OSR_128          0x80    
   #define QMC5883_CMD_OSR_64           0xC0    
 
#define QMC5883_REG_CTRL2      0x0A
   #define QMC5883_CMD_INT_ENABLE       0x00 
   #define QMC5883_CMD_INT_DISABLE      0x01
   #define QMC5883_CMD_ROL_PNT_ENABLE   0x40  //pointer roll-over function,only 0x00-0x06 address
   #define QMC5883_CMD_ROL_PNT_DISABLE  0x00 
   #define QMC5883_CMD_SOFT_RST_ENABLE  0x80
   #define QMC5883_CMD_SOFT_RST_DISABLE 0x00 
   
#define QMC5883_REG_SET_RESET  0x0B
   #define QMC5883_CMD_SET_RESET        0x01 
  
#define QMC5883_REG_PRODUCTID  0x0D           //chip id :0xFF
/*默认（出厂） HMC5883LL 7 位从机地址为 0x3C 的写入操作，或 0x3D 的读出操作。*/
/*默认（出厂） QMC5883 7 位从机地址为 0x1a 的写入操作，或 0x1b 的读出操作。*/
u8 verifyCompass(void)
{
	u8 receiveData=0xFF;
	IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x0D);
	IIC_Wait_Ack();
	IIC_Stop();
	//移动5883内部指针移动至0x0A
	IIC_Start();
	IIC_Send_Byte(0x1B);
	IIC_Wait_Ack();
	receiveData = IIC_Read_Byte(0);//nACK
	IIC_Stop();
	if(receiveData==0xff) return 1;
	else return 0;
}

u8 checkDataReady(void)
{
	u8 receiveData=0;
	IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x06);
	IIC_Wait_Ack();
	IIC_Stop();
	//移动5883内部指针移动至0x06
	IIC_Start();
	IIC_Send_Byte(0x1B);
	IIC_Wait_Ack();
	receiveData = IIC_Read_Byte(0);//nACK
	IIC_Stop();
	if((receiveData&0x01)==0x01) return 1;
	else return 0;
}

void cfgCompassReg(void)
{//地磁场强度大约是0.5-0.6高斯,增益需要增大
	IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x20);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x40);
	IIC_Stop();
	
	IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x21);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x01);
	IIC_Stop();
	
		IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x09);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x1d);
	IIC_Stop();
	
	IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(QMC5883_REG_CTRL2);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x41);//QMC5883_CMD_SOFT_RST_ENABLE|QMC5883_CMD_INT_DISABLE|QMC5883_CMD_ROL_PNT_ENABLE);
	IIC_Stop();
	
	IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(QMC5883_REG_CTRL1);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x01);//QMC5883_CMD_MODE_CON|QMC5883_CMD_ODR_10HZ|QMC5883_CMD_RNG_8G|QMC5883_CMD_OSR_512);
	IIC_Stop();

}


u8 receiveComData(void)
{
  uint8_t buffer[6]={0};
	u8 counter;
	
	
	if(!checkDataReady()) return 0;
	IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(QMC5883_REG_DATA);
	IIC_Wait_Ack();
	IIC_Stop();	
	
	IIC_Start();
	IIC_Send_Byte(0x1B);
	IIC_Wait_Ack();
	for(counter=0;counter<6;counter++)
	{
		if(counter>=5) buffer[counter] = IIC_Read_Byte(0);//nACK
		else buffer[counter] = IIC_Read_Byte(1);
	}
	IIC_Stop();

	eCompass_Main.channelX = (int16_t)(buffer[1] << 8 | buffer[0]);	
	eCompass_Main.channelZ = (int16_t)(buffer[5] << 8 | buffer[4]);
	eCompass_Main.channelY = (int16_t)(buffer[3] << 8 | buffer[2]);
	if(eCompass_Main.channelX>0x7fff) eCompass_Main.channelX=0x1ffff-eCompass_Main.channelX;		 
	else eCompass_Main.channelX=0xffff+eCompass_Main.channelX;
	
	if(eCompass_Main.channelY>0x7fff) eCompass_Main.channelY=0x1ffff-eCompass_Main.channelY;		
	else eCompass_Main.channelY=0xffff+eCompass_Main.channelY;
	
	if(eCompass_Main.channelZ>0x7fff) eCompass_Main.channelZ=0x1ffff-eCompass_Main.channelZ;		
	else eCompass_Main.channelZ=0xffff+eCompass_Main.channelZ;
	//printf("MAG X%d Y%d Z%d\n",eCompass_Main.channelX,eCompass_Main.channelY,eCompass_Main.channelZ);
	return 1;
	
}

/* HMC5883
u8 receiveComData(void)
{
	u8 counter;
	u8 datain[6];
	if(!checkDataReady()) return 0;
	IIC_Start();
	IIC_Send_Byte(0x1A);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x03);
	IIC_Wait_Ack();
	IIC_Stop();	
	
	IIC_Start();
	IIC_Send_Byte(0x1B);
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
*/
