#ifndef configuration
#define configuration
#include "stm32f10x.h"
#include "User\Delay\delay.h"
#include <stdio.h>
/*��������*/
#define USE_BRUSHED_MOTOR 0 //ʹ����ˢ���ʱ
/*��������*/
#ifndef BASIC
	#define BASIC
	/*DMA��������λ*/
	#define USE_USART3_GPS_DMA_RX 1
	#define USE_USART1_COMM_DMA_TX 1	//��Ҫ����synthesis_frame()�����У���������Debugģʽ�뽫��λ����
	#define USE_USART1_COMM_DMA_RX 0	//DMA_RxĿǰ������
	/*���ش�ƫ�ǣ�ת��Ϊ����*/
	#define Magnatic_Static_Bias (float)(8.0/(180*PI))	
#endif
	
/*��λ������״̬��*/	
#ifndef STATE_MACHINE
	#define STATE_MACHINE
	/*��ʼ��״̬*/
	#define STATE_INITIALIZE 0
	/*�����������У�����������״̬*/
	#define STATE_NORMALLY_RUNNING 1
	/*GPS�ɵ�״̬*/
	#define STATE_COLLECTING_POINTS 2
	/*�ֶ�ң��״̬*/
	#define STATE_MANUAL_CTRL 3
	/*�������״̬*/
	#define STATE_DEBUGGING 4	
#endif
	
/*�꺯������*/
#ifndef MACRO_FUNCTION
	#define LED_BRINK() 		if(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)) GPIO_SetBits(GPIOB, GPIO_Pin_5);\
													else GPIO_ResetBits(GPIOB, GPIO_Pin_5)
														
	#define MISSION_LED_BRINK() 		if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)) GPIO_SetBits(GPIOE, GPIO_Pin_5);\
													else GPIO_ResetBits(GPIOE, GPIO_Pin_5)
#endif

#ifndef IMU_MODULE_SELECT
/*
	GY901��ȡ����
	ACCESS_PERIPH�����1����MCU�ڶ�ȡ����ʱ�ὫPERIPH���ݿ������ڡ����򣬲��迼�ǡ�
*/
	#define GY901_nMPU6050 0  //imuģ��ѡ����룬1��ʾʹ��GY901��0��ʾMPU6050
	#if GY901_nMPU6050
		#define ACCESS_Acceleration 1
		#define ACCESS_Angle_speed 1
		#define ACCESS_Angle 0
		#define ACCESS_Mag 1
	#endif	
#endif												
/*=========================ȫ�ֳ�������==========================*/
#ifndef GLOBAL
#define GLOBAL
	#define TRUE 1
	#define FALSE 0
	#define PI 3.1415926
	#define G	(float)9.8	
	/*����ֱ��*/
	#define WHEELDIAMETER (float)6.5		//cm		
#endif
/*==============================DATAFUSION����===============================*/
#ifdef DATAFUSION
	#include <math.h>
	#include "User\IMU\IMU.h"
	#include "USER\GPS\GPS.h"
	#include "USER\COMPASS\COMPASS.h"
	// System constants
	#define sampleFreq 50 // sample frequency in Hz
	// estimated orientation quaternion elements with initial conditions
	float SEq_1 = 1, SEq_2 = 0, SEq_3 = 0, SEq_4 = 0; 
	volatile float psi=0,theta=0,fhi=0;//ŷ���ǣ����ڱ�ʾ��̬��
	float yaw=0,pitch=0,roll=0;
#endif
/*=========================���λ���������==========================*/
#ifdef RINGBUFF
	#ifndef RINGBUFF_LEN
	/*=���Ҫ�Ķ�����������������Ҫ�˴�����Ӧ������ͬʱ����=*/
	#define LENGTH_OF_BUFF 200 //�ĵ�ʱ���������ringbuff.h��Ҳ��
	u16 RINGBUFF_LEN=LENGTH_OF_BUFF;  //GPS���λ���������
	
	#endif
	/*���λ������ṹ�嶨��*/
	typedef struct
	{
		volatile u16 Head;
		volatile u16 Tail;
		volatile u16 Length;
		volatile u8 Ring_Buff[LENGTH_OF_BUFF];
	}RingBuff_t;
	/*��ʼ��������*/
	volatile RingBuff_t ringBuff;
	volatile RingBuff_t ringBuff_IMU;	
	volatile RingBuff_t ringBuff_USART1;	
#endif

/*=========================ϵͳ�����������==========================*/
#ifdef LOOP
	#include "USER\IIC\IIC.h"
	#include "USER\Usart\USART.h"
	#include "USER\GPS\GPS.h"
	#include "USER\COMPASS\COMPASS.h"
	#include "USER\FILTER\FILTER.h"
	#include "USER\DATAFUSION\DATAFUSION.h"
	#include "USER\IMU\IMU.h"
	#include "User\TD\TD.h"
	#include "User\PID\PID.h"
	#include "User\Motor\Motor.h"
	#include "User\INPUT_CAPTURE\INPUT_CAPTURE.h"
	#include "User\PathPlanning\PathPlanning.h"
	#include "User\WDG\WDG.h"
	#define APB1DIV 2		//��Ƶϵ������������ʱ��Ƭ��ʱ

	/*Madgwick�˲�����������*/
	#define BETA 3.5
	#define MadgwickGainDecendTime 3  //��������3�����Զ��½�
	/*�����������*/
	u8 MotorLockTimeAfterRaspiOffline = 2;   //�������ڼ�����δ�յ���λ������������
	/*����״̬��������*/
	u8 current_state = STATE_MANUAL_CTRL;//STATE_NORMALLY_RUNNING;
	#define CHANGE_STATE_TO(next_state) current_state = next_state
	/*���ʹ�ܱ�־λ*/
	u8 Motor_Enabled = 0;
	extern void Initialize(void);
	u8 Analyze_Controller_Msg(void);
	/*ʱ��Ƭ�ṹ�嶨��*/
	typedef struct{	
		volatile u8 Hz200;
		volatile u8 Hz100;
		volatile u8 Hz50;
		volatile u8 Hz10;
		volatile u8 Hz5;
		volatile u8 Hz1;
	}TimeSlice;


	/*��ʼ��������*/
	volatile u16 system_pulse;
	volatile TimeSlice Mission;
	volatile int16_t MadgwickGainAutoDecend=0;
	int16_t RaspiOfflineIndicator = 0;
#endif
/*==============================GPS����===============================*/
#ifdef GPS
	#include "User\IIC\IIC.h"
	#include "User\RingBuff\ring_buff.h"
	#include "User\Usart\USART.h"
	#define MODEL_nVEHICLE_SEA 0 //ģʽ���ã���λΪ1ʱ��GPS��̬ģ������Ϊ����ģ�ͣ�����Ϊ�ؾ�ģ��
	#define PVTdataout 1				//Ϊ1��ʹ����Ӧ�����Ϊ0��ʧ�������
	#define SVinfoOut 0
	#define DOPout 0
	typedef struct{	
		u8 fixtype;		//GPS��������
		u8 fixFlags;	//GPS�������ͱ�־λ
		u8 numSV;			//���ڵ��������������Ŀ
		int32_t lon;	//����								 degree
		int32_t lat;	//ά��								 degree
		int32_t velN;	//����������ϵ���ٶ� mm/s
		int32_t velE;	//����������ϵ���ٶ� mm/s
		int32_t velD;	//����������ϵ����ٶ� mm/s
		u32 sAcc;			//Ԥ���ٶȾ���				 mm/s
		u32 hAcc;			//Ԥ��ˮƽ����				 mm
		u16 pdop;			//��λ��������  pdop<4,���Ⱥܺ�;4<pdop<7�����ȿɽ���;pdop>7,pdop�ϲ�
	}pvtData;

	pvtData GPS_PVTData;
#endif
/*==============================��λ��ͨ������===============================*/
#ifdef COMM
#include "User\RingBuff\ring_buff.h"
#include "User\TD\TD.h"
#include <stdio.h>
#include "User\LoopSequence\Loop.h"
	u8 DMA_RX = USE_USART1_COMM_DMA_RX;
	u8 DMA_TX = USE_USART1_COMM_DMA_TX;
#endif
/*==============================IIC����===============================*/
#ifdef IIC
	#define IIC_PORT GPIOC
	#define IIC_INITIAL_PORT RCC_APB2Periph_GPIOC
	#define SCL GPIO_Pin_12 
	#define SDA GPIO_Pin_11
#endif
/*=============================COMPASS����============================*/
#ifdef COMPASS
	#include "User\IIC\IIC.h"
	#include "User\IMU\IMU.h"
	#include <math.h>
	#define REGISTERA 0x78		//����Ƶ�ʣ�75Hz,��ͨ�˲���������8������ƽ��һ�Σ�
	#define REGISTERB 0X00		//Gauss���棺1090
	#define REGISTERMODE 0X00	//��������ģʽ
	int16_t Local_mag_bias=Magnatic_Static_Bias;
#endif
/*==============================IMU����===============================*/
#ifdef IMU
	#include "User\RingBuff\ring_buff.h"
	#include "User\IIC\IIC.h"
	#include "User\IMU\MPU6050.h"
	#include <math.h>
	#define FRAME_LENGTH 11	//��ʾGY901һ֡���ݵĳ���
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
	
	/*
	˵������������e�Ķ���ʾRaw���ݣ�δ���κδ���
				������ǰ׺��p�ı�ʾProcessed���ݣ���ʾ�Ѵ����ɹ�ʹ�á�
	*/
	Acceleration eAcceleration,pAcceleration,acceFilter;
	
	Angle_speed eAngleSpeed,pAngleSpeed,anglespeedFilter;
	Angle_speed Gyro_Cancelation;//�����Ǿ�������
	
	Angle eAngle,pAngle;
	
	Mag eCompass_Main,pCompass_Main;	
	Mag Compass_Hard_Iron_Cancellation;
#endif
/*==============================FILTER����============================*/
#if defined FILTER 
	#include "User\IMU\IMU.h"	
	#include "USER\DATAFUSION\DATAFUSION.h"
	#include <math.h>
#endif
/*=========================TRANSMITDATA����===========================*/
#ifdef TRANSMITDATA
	#include "User\DATAFUSION\DATAFUSION.h"
	#include "USER\Usart\USART.h"
	#include "USER\GPS\GPS.h"
	#include "User\RingBuff\ring_buff.h"
//	#include "USER\WirelessUSART\WirelessPort.h"
	#include "User\LoopSequence\Loop.h"
	#define one_frame_length 22 //����֡���ȹ�18Byte
	u16 FRAMELENGTH = one_frame_length;
	u8 DATAFRAME[one_frame_length] = {0x73,0x63};
	typedef struct{	
	u16 yaw;
	u16 distance;
	//����״̬��7λ��EndOfNav��־λ,��λΪ1��ʾ����������ͣ����
	u8 control_status;
	}Route;
	
	Route routeToGo;
#endif
/*==============================MOTOR����=============================*/
#ifdef MOTOR
#include "USER\PWM\pwm.h"
	#define FORWARD 1
	#define BACKWARD 2
	#define ENCODER_WIRE_NUMBER 1560
	#define LEFT_WHEEL 2
	#define RIGHT_WHEEL 1
	#define ALL_WHEEL 3
#endif
/*==============================PID����===============================*/
//#ifdef PID
//	#include "USER\PWM\pwm.h"
//	#include "USER\FILTER\FILTER.h"
//	#include <math.h>
//	#define Velocity_KP (float)12
//	#define Velocity_KI (float)-3.5
//	#define Velocity_KD (float)0

//#endif
/*==============================PATHPLAN����==========================*/
#ifdef PATHPLAN 
	#include "User\TD\TD.h"
	#include "User\Motor\Motor.h"
	#include "User\PID\PID.h"
	#include <math.h>
	#define CLOCKWISE 0
	#define COUNTER_CLOCKWISE 1
	
#endif

/*============================WIRELESS PORT����========================*/
#ifdef WirelessPort 
	#define MD0 GPIO_Pin_11
	#define AUX GPIO_Pin_10
	#define GPIOPORT GPIOD
	#define RCC_GPIOPORT RCC_APB2Periph_GPIOD
#endif

/*========================ң��������====================*/
#ifdef INPUT_CATPURE 
#define CAPTURE_PERIOD (u16)19
#define CAPTURE_PRESCALER (u16)71
u32 CHANNEL_HIGHOUTPUT[4];
#endif

#endif
