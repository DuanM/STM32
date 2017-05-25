#include "stm32f10x.h"
#include "exti.h"
#include "key.h"
#include "stdio.h"
#include "includes.h"
/****************************************************
函数名：EXTI_KeyConfig()
形参：无
返回值：无
函数功能：配置中断通道
****************************************************/
void EXTI_KeyConfig(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	//按键初始化
	Key_Config();
	//打开AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//映射PA0 -- EXTI0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	
	//EXTI的初始化
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿检测
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
	
	//NVIC的配置
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;//使能外部EXTI0中断通道
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//占先优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//次级优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}
/****************************************************
函数名：EXTI0_IRQHandler
形参：
返回值：
函数功能：外部中断0的中断服务函数
****************************************************/

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
	{

		//清中断标志位
		EXTI_ClearITPendingBit(EXTI_Line0);
		//用户自己编写 
		printf("go to the interupt 0!\r\n");
	}
}

