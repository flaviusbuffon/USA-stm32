#define WATCHDOG 1
#include "..\CONFIGURATION.h"

//��ʼ�����Ź�
void WDG_Initialize(uint8_t prescaler,uint16_t reload)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//ʹ��дȨ��
	//Tout=((4��2^prer) ��rlr) /40   ,���ʱ�䵥λΪms
	IWDG_SetPrescaler(prescaler); //���� IWDG Ԥ��Ƶֵ
	IWDG_SetReload(reload); //���� IWDG ��װ��ֵ
}
//�������Ź�
//���Ź�һ�����������޷��رգ�
void WDG_Start(void)
{
	IWDG_Enable(); //ʹ�� IWDG
}

//ι��
void feed_dog(void)
{
	IWDG_ReloadCounter(); //���� IWDG ��װ�ؼĴ�����ֵ��װ�� IWDG ������
}
