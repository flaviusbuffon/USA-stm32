#ifndef __IMU
#define __IMU
#include "stm32f10x.h"

//�ṹ�嶨��
typedef struct{	
	volatile float channelX;
	volatile float channelY;
	volatile float channelZ;
	}Acceleration;

	typedef struct{	
	volatile float channelX;
	volatile float channelY;
	volatile float channelZ;
	}Angle_speed;
	
	typedef struct{	
	volatile float Roll;
	volatile float Pitch;
	volatile float Yaw;
	}Angle;
	
	typedef struct{	
	volatile int16_t channelX;
	volatile int16_t channelY;
	volatile int16_t channelZ;
	volatile int16_t ratioX;
	volatile int16_t ratioY;
	volatile int16_t ratioZ;
	}Mag;

	typedef struct{	
	volatile float channelX;
	volatile float channelY;
	volatile float channelZ;
	}Gyro_Noise;//�����ź�
//��������	
extern Acceleration eAcceleration,pAcceleration,acceFilter;
extern Angle_speed eAngleSpeed,pAngleSpeed,anglespeedFilter;
extern Angle eAngle,pAngle;
extern Mag eCompass_Main,pCompass_Main;
extern Gyro_Noise Gyro_Cancelation;
extern Mag Compass_Hard_Iron_Cancellation;
/*GY901*/
u8 dataFromGY901(void);
void usart2_init(u32 baudrate);
void USART2_IRQHandler(void);
void ringBuff_USART2_TEST(void);
u8 IMU_CFG_RATE(void);
u8 IMU_CFG_OUTPUT(void);
u8 IMU_SENDDATA(u8 datain);
/*MPU6050*/
void dataFrom6050(void);
void MPU6050_Init(void);
int16_t MPU6050_Get_Data(uint8_t regAddr);
uint8_t MPU6050_Read_Reg(uint8_t regAddr);
void MPU6050_Write_Reg(uint8_t regAddr, uint8_t regData);
u8 verifyMPU6050(void);
void print_IMU_data(void);
void Caliberate_Gyro(u16 number_of_dataset);
void Caliberate_Mag(void);


void SendYawFromCompass(void);
#endif
