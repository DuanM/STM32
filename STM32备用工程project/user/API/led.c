#include "stm32f10x.h"


#include "led.h"


//配置时钟
void Led_Config(void)
{
	//时钟使能
	GPIO_InitTypeDef GPIO_Initstruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	//配置IO口
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
	
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
	
		GPIO_Initstruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstruct);
}

//开灯
void Led_Light1(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}

void Led_Light2(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);
}

void Led_Light3(void)
{
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}

void Led_Light4(void)
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_5);
}



//关灯
void Led_Dark(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	GPIO_SetBits(GPIOA,GPIO_Pin_6);
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
	GPIO_SetBits(GPIOE,GPIO_Pin_5);
}

