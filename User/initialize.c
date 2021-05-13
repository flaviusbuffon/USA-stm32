#include "inclusion.h"
/*��ʼ���궨��*/
#define USART1_BAUDRATE 921600
#define GPS_BAUDRATE_CONFIG 9600

void Initialize(void)
{
	/*==========��ʼ��===========*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //���� NVIC �жϷ��� 2:2 λ
	delay_init(SystemCoreClock);
	TIM5_STSTEM_PULSE_Init();
	GPIOPin_Init();
	IIC_Init();
	ringBuff_Init();
	ringBuff_IMU_Init();
	//���õ������
	Motor_IN_Pin_Init();
	Motor_Brake();
	/*�������*/
	Servo_Set_Angle(0);
	delay_ms(700); //��ʱ���ȴ�����ȫ������
	usart_init(USART1_BAUDRATE,0,0);//��ʼ��USART1ʱ��ʹ��DMA
	/*=====����GPS====*/
	GPSDATA_Init();
	usart3_init(9600);//GPS���ڲ���������
	//�޸�GPS������
	delay_ms(5);
	UBX_CFG_USART(GPS_BAUDRATE_CONFIG);
	//delay_ms(5);usart3_init(GPS_BAUDRATE_CONFIG);//���óɹ�,��GPS�����ʱ��޸�
	delay_ms(5);UBX_CFG_RATE();
	delay_ms(5);UBX_CFG_DATAOUT();
	delay_ms(5);UBX_CFG_MODEL();
	/*����Compass*/
	if(verifyCompass()) 
	{
		printf("\nCompass Online!\n");
		cfgCompassReg();
	}
	MPU6050_Init();
	if(verifyMPU6050()) printf("\nMPU6050 Online!\n");
	/*ִ�д�������У׼����*/
	Caliberate_Gyro(100);
	Caliberate_Mag();
	/*��ȡ��ʼ��̬*/
	receiveComData();
	dataFrom6050();
	filter_IMU_data(0);
	print_IMU_data();
	theta = atan2(pAcceleration.channelY,pAcceleration.channelZ);//pitch
	psi = atan2(pAcceleration.channelX,pAcceleration.channelZ);//roll
	fhi = 3.1415926-atan2((float)pCompass_Main.channelY,(float)pCompass_Main.channelX);//yaw
	fhi -= Local_mag_bias;
	//printf("Currnet Initial Yaw:%f\nROLL:%f,\npitch:%f\n",fhi*180/3.1415926,psi*180/3.1415926,theta*180/3.1415926);
	/*����ʼ��̬ת��Ϊ��ʼ��Ԫ��*/
	SEq_4 = cos(fhi/2)*cos(theta/2)*cos(psi/2)+sin(fhi/2)*sin(theta/2)*sin(psi/2);
	SEq_1 = sin(fhi/2)*cos(theta/2)*cos(psi/2)-cos(fhi/2)*sin(theta/2)*sin(psi/2);
	SEq_2 = cos(fhi/2)*sin(theta/2)*cos(psi/2)+sin(fhi/2)*cos(theta/2)*sin(psi/2);
	SEq_3 = cos(fhi/2)*cos(theta/2)*sin(psi/2)-sin(fhi/2)*sin(theta/2)*cos(psi/2);
	Convert_Quaternion_To_Euler();
	//printf("Processed Initial Yaw:%f\n",fhi*180/3.1415926);

	/*���ô�����ʱ������DMA
	������LOOP֮ǰ����DMA*/
	usart_init(USART1_BAUDRATE,DMA_RX,DMA_TX);
	/*����ң�����źŲ���*/
	TIM3_Capture_Init();
	//���ÿ��Ź�
	WDG_Initialize(7,240);
	//����������Ź�
	delay_ms(500);
	WDG_Start();
 }
