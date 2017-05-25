#include "includes.h"

/*
static __INLINE uint32_t SysTick_Config(uint32_t ticks);
//����װ�ض�ʱ��ʱ��ֵ
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource);
//�������õδ�ʱ�ӵ�ʱ��Ƶ��

Ŀ�ģ��õδ�ʱ���γ�һ��1us�Ķ�ʱ����
1us = T*(1/f)��
T:Ҫװ�ص�����
f:�δ�ʱ�����е�ʱ��Ƶ�ʣ�

1s = 1000ms;
1ms = 1000us;
1s = 1000000us;
1M = 1000000;
1/1M = 1us;
t= 1/72M = 1/72us
1us = 72 * t;
SystemCoreClock/1000000 = 72     ---- 1us
SystemCoreClock/100000  = 720    ---- 10us
SystemCoreClock/10000   = 7200   ---- 100us
SystemCoreClock/1000    = 72000  ---- 1ms
SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//�򿪵δ�ʱ��
	
SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//�رյδ�ʱ��
*/
/****************************************************
��������SysTickConfig
�βΣ���
����ֵ����
�������ܣ��δ�ʱ�ӳ�ʼ��
****************************************************/
void SysTickConfig(void)
{
	//����Ϊ72Mhzʱ��
	//t= 1/72M = 1/72us;
//	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	if(SysTick_Config(SystemCoreClock/1000000) == 1)//����ֵ
	{                     //72
		while(1);
	}
	
	//�رյδ�ʱ��
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	//SysTick->CTRL &= ~0x01;// 0xfe  1111 1110
	//SysTick->CTRL |= 0x01;
	//��ĳЩλд0���ð�λ�루&��
	//��ĳЩλд1���ð�λ��|��
}
/****************************************************
��������Delay_us
�βΣ�time����Ҫ��ʱ��ʱ�䣨��λus��
����ֵ����
�������ܣ���ȷus��ʱ����
****************************************************/
u32 TimeDelay = 0;
void Delay_us(u32 time)
{
	TimeDelay = time;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	while(TimeDelay !=0);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
/****************************************************
��������Delay_ms
�βΣ�time����Ҫ��ʱ��ʱ�䣨��λms��
����ֵ����
�������ܣ���ȷms��ʱ����
****************************************************/
void Delay_ms(u32 time)
{
	TimeDelay = time*1000;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	while(TimeDelay !=0);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
/****************************************************
��������SysTick_Handler
�βΣ���
����ֵ����
�������ܣ��δ�ʱ���жϷ�����
****************************************************/
void SysTick_Handler(void)
{
	if(TimeDelay > 0)
		TimeDelay--;
}








