#define IIC 1
#include "..\CONFIGURATION.h"
//��ʼ�� IIC
void IIC_Init(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
//ʹ������ IO PORTC ʱ��
RCC_APB2PeriphClockCmd(IIC_INITIAL_PORT, ENABLE );
GPIO_InitStructure.GPIO_Pin = SCL|SDA;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ; //�������
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(GPIOC, &GPIO_InitStructure);
GPIO_SetBits(IIC_PORT,SCL);
GPIO_SetBits(IIC_PORT,SDA);
	
}

void SDA_OUT()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//ʹ������ IO PORTC ʱ��
	GPIO_InitStructure.GPIO_Pin = SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ; //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
void SDA_IN()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//ʹ������ IO PORTC ʱ��
	GPIO_InitStructure.GPIO_Pin = SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; //��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
//���� IIC ��ʼ�ź�

void IIC_Start(void)
{
	SDA_OUT(); //sda �����
	GPIO_SetBits(IIC_PORT,SDA);
	delay_us(2);
	GPIO_SetBits(IIC_PORT,SCL);
	delay_us(3);
	GPIO_ResetBits(IIC_PORT,SDA);//START:when CLK is high,DATA change form high to low
	delay_us(3);
	GPIO_ResetBits(IIC_PORT,SCL);//ǯס I2C ���ߣ�׼�����ͻ��������
}

//���� IIC ֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda �����
	GPIO_ResetBits(IIC_PORT,SCL);
	GPIO_ResetBits(IIC_PORT,SDA);//STOP:when CLK is high DATA change form low to high
	delay_us(3);
	GPIO_SetBits(IIC_PORT,SCL);
	GPIO_SetBits(IIC_PORT,SDA);//���� I2C ���߽����ź�
	delay_us(3);
}

//�ȴ�Ӧ���źŵ���
//����ֵ��0������Ӧ��ʧ��
// 1������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u16 ucErrTime=0;
	SDA_IN(); //SDA ����Ϊ����
	GPIO_SetBits(IIC_PORT,SDA);delay_us(1);
	GPIO_SetBits(IIC_PORT,SCL);delay_us(1);
	while(GPIO_ReadInputDataBit(IIC_PORT,SDA))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 0;
		}
	}
	GPIO_ResetBits(IIC_PORT,SCL);//ʱ����� 0
	return 1;
}

//���� ACK Ӧ��
void IIC_Ack(void)
{
	GPIO_ResetBits(IIC_PORT,SCL);
	SDA_OUT();
	GPIO_ResetBits(IIC_PORT,SDA);
	delay_us(2);
	GPIO_SetBits(IIC_PORT,SCL);
	delay_us(2);
	GPIO_ResetBits(IIC_PORT,SCL);
}
//������ ACK Ӧ��
void IIC_NAck(void)
{
	GPIO_ResetBits(IIC_PORT,SCL);
	SDA_OUT();
	GPIO_SetBits(IIC_PORT,SDA);
	delay_us(2);
	GPIO_SetBits(IIC_PORT,SCL);
	delay_us(2);
	GPIO_ResetBits(IIC_PORT,SCL);
}
//IIC ����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT();
	GPIO_ResetBits(IIC_PORT,SCL);//����ʱ�ӿ�ʼ���ݴ���
	for(t=0;t<8;t++)
	{
		switch((txd&0x80)>>7)
		{
			case 1: GPIO_SetBits(IIC_PORT,SDA);break;
			case 0:GPIO_ResetBits(IIC_PORT,SDA);break;
		}
		txd<<=1;
		delay_us(2); //�� TEA5767 ��������ʱ���Ǳ����
		GPIO_SetBits(IIC_PORT,SCL);
		delay_us(2);
		GPIO_ResetBits(IIC_PORT,SCL);
		delay_us(2);
	}
}

//�� 1 ���ֽڣ�ack=1 ʱ������ ACK��ack=0������ nACK
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA ����Ϊ����
	for(i=0;i<8;i++ )
	{
		GPIO_ResetBits(IIC_PORT,SCL);
		delay_us(2);
		GPIO_SetBits(IIC_PORT,SCL);
		receive<<=1;
		if(GPIO_ReadInputDataBit(IIC_PORT,SDA))receive++;
		delay_us(1);
		
	}
	if (!ack)
	IIC_NAck();//���� nACK
	else
	IIC_Ack(); //���� ACK
	return receive;
}

