#include "stm32f10x.h"


u8 us;
u16 ms;

void delay_init(u8 SYSCLK)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //��SYSTICKԴѡΪHCLK8��Ƶ
	us = SystemCoreClock/8000000;
	ms=(u16)us*1000;
}

void delay_us(u32 nus)
{
	u32 temp;
	SysTick->LOAD = nus*us;
	SysTick->VAL =0x00;
	SysTick-> CTRL =SysTick_CTRL_ENABLE_Msk;
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));//��counterflag�Ƚ�
	
	SysTick->CTRL = ~SysTick_CTRL_ENABLE_Msk;  //�رռ�����
	SysTick -> VAL = 0x00;//��ռ�����
}

void delay_ms(u16 nms)
{
	u32 temp;
	SysTick->LOAD = nms*ms;
	SysTick->VAL =0x00;
	SysTick-> CTRL =0x01;
	do
	{
		temp = SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));//��counterflag�Ƚ�
	
	SysTick->CTRL = 0x00;  //�رռ�����
	SysTick -> VAL = 0x00;//��ռ�����
}
