#define INPUT_CATPURE 1
#include "..\CONFIGURATION.h"


u16 TIM3_CH1_STATUS,TIM3_CH2_STATUS,TIM3_CH3_STATUS,TIM3_CH4_STATUS;
u16 TIM3_CH2_START_VALUE,TIM3_CH2_END_VALUE,TIM3_CH1_START_VALUE,TIM3_CH1_END_VALUE;
u16 TIM3_CH3_START_VALUE,TIM3_CH3_END_VALUE,TIM3_CH4_START_VALUE,TIM3_CH4_END_VALUE;

/*
===========================Warning======================================= 
arr����ֵԽС��ͨ����������Խ�ߣ�������ͨ���ĸߵ�ƽʱ�����ֵ��С��
					ʵ�⣺arr��ֵΪ10ʱ���������ֵΪ180ms���ҡ�
					psc����Ϊ72-1����arr+1������ʱ�������ж�һ�ε�ʱ�䡣��λΪus.
*/
void TIM3_Capture_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_ICInitTypeDef TIM_ICInitStructure;
		
		/*ʱ��ʹ��*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
		
		/*����TIM*/
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
		TIM_TimeBaseStructure.TIM_Period = CAPTURE_PERIOD;
		TIM_TimeBaseStructure.TIM_Prescaler = CAPTURE_PRESCALER;
		TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
		
		/*����GPIO*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 	//����ߵ�ƽ�������˿�
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
		GPIO_Init(GPIOB,&GPIO_InitStructure);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
		GPIO_Init(GPIOB,&GPIO_InitStructure);

		/*���ò���*/
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
		TIM_ICInitStructure.TIM_ICFilter = 0;  //���˲�
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ش���
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInit(TIM3,&TIM_ICInitStructure);
		TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
		/*���ò���*/
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
		TIM_ICInitStructure.TIM_ICFilter = 0;  //���˲�
		TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ش���
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
		TIM_ICInit(TIM3,&TIM_ICInitStructure);
		TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);
		/*���ò���*/
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
		TIM_ICInit(TIM3,&TIM_ICInitStructure);
		TIM_ITConfig(TIM3,TIM_IT_CC3,ENABLE);
		/*���ò���*/
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
		TIM_ICInit(TIM3,&TIM_ICInitStructure);
		TIM_ITConfig(TIM3,TIM_IT_CC4,ENABLE);
		/*����NVIC*/
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		/*ʹ��ָ���ն�����*/
		TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);
		
		TIM_Cmd(TIM3,ENABLE);
}


u8 TIM3_4channel_process(void)
{
	u8 processed=0;
	u32 STATUS_CH;
	/*��ͨ��1������Чʱ*/
	TIM_Cmd(TIM3,DISABLE);
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,DISABLE);
	if(TIM3_CH1_STATUS&0x8000)
	{
		STATUS_CH = TIM3_CH1_STATUS&0x3FFF;
		processed=1;						/*���STATUS>0*/
		if(STATUS_CH>0) 
		{
			CHANNEL_HIGHOUTPUT[0] = (STATUS_CH-1)*(CAPTURE_PERIOD+1) + (CAPTURE_PERIOD+1-TIM3_CH1_START_VALUE)+TIM3_CH1_END_VALUE;
		}
		else 
		{
			if(TIM3_CH1_END_VALUE>=TIM3_CH1_START_VALUE) CHANNEL_HIGHOUTPUT[0] = TIM3_CH1_END_VALUE-TIM3_CH1_START_VALUE;
			else CHANNEL_HIGHOUTPUT[0] = TIM3_CH1_START_VALUE - TIM3_CH1_END_VALUE;
		}
	}
	
	if(TIM3_CH2_STATUS&0x8000)
	{
		STATUS_CH = TIM3_CH2_STATUS&0x3FFF;
		processed=1;
		if(STATUS_CH>0) 
		{
			CHANNEL_HIGHOUTPUT[1] = (STATUS_CH-1)*(CAPTURE_PERIOD+1) + (CAPTURE_PERIOD+1-TIM3_CH2_START_VALUE)+TIM3_CH2_END_VALUE;
		}
		else 
		{
			if(TIM3_CH2_END_VALUE>=TIM3_CH2_START_VALUE) CHANNEL_HIGHOUTPUT[1] = TIM3_CH2_END_VALUE-TIM3_CH2_START_VALUE;
			else CHANNEL_HIGHOUTPUT[1] = TIM3_CH2_START_VALUE - TIM3_CH2_END_VALUE;
		}
	}

	if(TIM3_CH3_STATUS&0x8000)
	{
		STATUS_CH = TIM3_CH3_STATUS&0x3FFF;
		processed=1;
		if(STATUS_CH>0) 
		{
			CHANNEL_HIGHOUTPUT[2] = (STATUS_CH-1)*(CAPTURE_PERIOD+1) + (CAPTURE_PERIOD+1-TIM3_CH3_START_VALUE)+TIM3_CH3_END_VALUE;
		}
		else 
		{
			if(TIM3_CH3_END_VALUE>=TIM3_CH3_START_VALUE) CHANNEL_HIGHOUTPUT[2] = TIM3_CH3_END_VALUE-TIM3_CH3_START_VALUE;
			else CHANNEL_HIGHOUTPUT[2] = TIM3_CH3_START_VALUE - TIM3_CH3_END_VALUE;
		}
	}

	if(TIM3_CH4_STATUS&0x8000)
	{
		STATUS_CH = TIM3_CH4_STATUS&0x3FFF;
		processed=1;
		if(STATUS_CH>0) 
		{
			CHANNEL_HIGHOUTPUT[3] = (STATUS_CH-1)*(CAPTURE_PERIOD+1) + (CAPTURE_PERIOD+1-TIM3_CH4_START_VALUE)+TIM3_CH4_END_VALUE;
		}
		else 
		{
			if(TIM3_CH4_END_VALUE>=TIM3_CH4_START_VALUE) CHANNEL_HIGHOUTPUT[3] = TIM3_CH4_END_VALUE-TIM3_CH4_START_VALUE;
			else CHANNEL_HIGHOUTPUT[3] = TIM3_CH4_START_VALUE - TIM3_CH4_END_VALUE;
		}
	}	

	/*
	���������ݺ���������־λ�����¿�ʼ��һ�ζ�ȡ
	*/
	TIM3_CH1_STATUS = 0;
	TIM3_CH2_STATUS = 0;
	TIM3_CH3_STATUS = 0;
	TIM3_CH4_STATUS = 0;
	TIM_Cmd(TIM3,ENABLE);
	TIM_ITConfig(TIM3,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4,ENABLE);	
	return processed;
}


void TIM3_IRQHandler(void)
{
	/*
	============�����жϴ���==============
	65536usƫ������
	��65536:
	SRART_VALUE:300
	END_VALUE:700
	ʵ�ʣ�65536 - 300 + 700 us
	*/
	if(TIM_GetITStatus(TIM3,TIM_IT_Update))
	{
		if((!(TIM3_CH1_STATUS&0x8000))&&(TIM3_CH1_STATUS&0x4000))
		{
			if((TIM3_CH1_STATUS&0x3FFF)==0x3FFF)
			{
				TIM3_CH1_STATUS |= 0x8000;
				TIM3_CH1_END_VALUE=0xffff;
			}else 
			{
				TIM3_CH1_STATUS ++;			
			}
		}
		
		if((!(TIM3_CH2_STATUS&0x8000))&&(TIM3_CH2_STATUS&0x4000))
		{
				if((TIM3_CH2_STATUS&0x3FFF)==0x3FFF)
				{
					TIM3_CH2_STATUS |= 0x8000;
					TIM3_CH2_END_VALUE=0xffff;
				}else 
				{
					TIM3_CH2_STATUS ++;			
				}
		}
		
		if((!(TIM3_CH3_STATUS&0x8000))&&(TIM3_CH3_STATUS&0x4000))
		{
				if((TIM3_CH3_STATUS&0x3FFF)==0x3FFF)
				{
					TIM3_CH3_STATUS |= 0x8000;
					TIM3_CH3_END_VALUE=0xffff;
				}else 
				{
					TIM3_CH3_STATUS ++;			
				}
		}
		
		if((!(TIM3_CH4_STATUS&0x8000))&&(TIM3_CH4_STATUS&0x4000))
		{
				if((TIM3_CH4_STATUS&0x3FFF)==0x3FFF)
				{
					TIM3_CH4_STATUS |= 0x8000;
					TIM3_CH4_END_VALUE=0xffff;
				}else 
				{
					TIM3_CH4_STATUS ++;			
				}
		}
		
		
	}
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	
	/*
	==========�����жϴ���============
	*/
	if(TIM_GetITStatus(TIM3,TIM_IT_CC1))	//�����ж�
	{
		if(TIM3_CH1_STATUS&0x4000)
		{	
			TIM3_CH1_END_VALUE = TIM_GetCapture1(TIM3);
			TIM3_CH1_STATUS |= 0x8000;
			TIM_OC1PolarityConfig(TIM3,TIM_ICPolarity_Rising);	//�ظ�Ϊ�����ش���
		}
		else
		{
			TIM3_CH1_START_VALUE = TIM_GetCapture1(TIM3);
			TIM3_CH1_STATUS= 0;
			TIM3_CH1_STATUS |= 0x4000;
			TIM_OC1PolarityConfig(TIM3,TIM_ICPolarity_Falling);
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
	}
	
	if(TIM_GetITStatus(TIM3,TIM_IT_CC2))	//�����ж�
	{
		if(TIM3_CH2_STATUS&0x4000)
		{
			TIM3_CH2_END_VALUE = TIM_GetCapture2(TIM3);
			TIM3_CH2_STATUS |= 0x8000;
			TIM_OC2PolarityConfig(TIM3,TIM_ICPolarity_Rising);	//�ظ�Ϊ�����ش���
		}
		else
		{
			TIM3_CH2_START_VALUE = TIM_GetCapture2(TIM3);
			TIM3_CH2_STATUS= 0;
			TIM3_CH2_STATUS |= 0x4000;
			TIM_OC2PolarityConfig(TIM3,TIM_ICPolarity_Falling);
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC2);
	}
	
	if(TIM_GetITStatus(TIM3,TIM_IT_CC3))	//�����ж�
	{
		if(TIM3_CH3_STATUS&0x4000)
		{
			TIM3_CH3_END_VALUE = TIM_GetCapture3(TIM3);
			TIM3_CH3_STATUS |= 0x8000;
			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising);	//�ظ�Ϊ�����ش���
		}
		else
		{
			TIM3_CH3_START_VALUE = TIM_GetCapture3(TIM3);
			TIM3_CH3_STATUS= 0;
			TIM3_CH3_STATUS |= 0x4000;
			TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Falling);
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC3);
	}

	if(TIM_GetITStatus(TIM3,TIM_IT_CC4))	//�����ж�
	{
		if(TIM3_CH4_STATUS&0x4000)
		{
			TIM3_CH4_END_VALUE = TIM_GetCapture4(TIM3);
			TIM3_CH4_STATUS |= 0x8000;
			TIM_OC4PolarityConfig(TIM3,TIM_ICPolarity_Rising);	//�ظ�Ϊ�����ش���
		}
		else
		{
			TIM3_CH4_START_VALUE = TIM_GetCapture4(TIM3);
			TIM3_CH4_STATUS= 0;
			TIM3_CH4_STATUS |= 0x4000;
			TIM_OC4PolarityConfig(TIM3,TIM_ICPolarity_Falling);
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_CC4);
	}		
}
