#include "stm32f10x.h"
#include "exti.h"
#include "key.h"
#include "stdio.h"
#include "includes.h"
/****************************************************
��������EXTI_KeyConfig()
�βΣ���
����ֵ����
�������ܣ������ж�ͨ��
****************************************************/
void EXTI_KeyConfig(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//������ʼ��
	Key_Config();
	//��AFIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//ӳ��PA0 -- EXTI0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	//EXTI�ĳ�ʼ��
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//�ж�ģʽ
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ؼ��
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	
	//NVIC������
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;//ʹ���ⲿEXTI0�ж�ͨ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//ռ�����ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//�μ����ȼ�
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}
/****************************************************
��������EXTI0_IRQHandler
�βΣ�
����ֵ��
�������ܣ��ⲿ�ж�0���жϷ�����
****************************************************/

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{

		//���жϱ�־λ
		EXTI_ClearITPendingBit(EXTI_Line0);
		//�û��Լ���д 
		printf("go to the interupt 0!\r\n");
	}
}

