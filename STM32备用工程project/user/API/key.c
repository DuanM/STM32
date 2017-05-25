#include "includes.h"
#include "key.h"
extern void Delay(u32 time);
/*打开时钟*/

void Key_Config(void)
{
	//时钟使能
	GPIO_InitTypeDef GPIO_Initstruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

	
	//配置IO口
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_0;			//配置按键
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_Initstruct);
	
	
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_Initstruct);
	
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initstruct);
	
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_Initstruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOE,&GPIO_Initstruct);
	
	//初始化IO口
	

	

}

//set all

void Set_All(void)
{
	
	  LED1(1);
		LED2(1);
		LED3(1);
		LED4(1);

}
	
//close all
void Close_All(void)
{


	  LED1(0);
		LED2(0);
		LED3(0);
		LED4(0);
}


/****************************************************
函数名：Get_Key
形参：无
返回值：返回按键值
函数功能：读IO口状态，返回按键值
****************************************************/

//u8 Get_Key(void)
//{
//	if(!KEY1 || !KEY2 || !KEY3 || KEY4)//判断按键是否按下
//	{
//		Delay(50000);
//		if(!KEY1 || !KEY2 || !KEY3 || KEY4)//消抖
//		{
//			if(!KEY1)
//			{
//				while(!KEY1);//松手检测
//				return Key_Up;
//			}
//			else if(!KEY2)
//			{
//				while(!KEY2);
//				return Key_Right;
//			}
//			else if(!KEY3)
//			{
//				while(!KEY2);
//				return Key_Down;
//			}
//			else if(KEY4)
//			{
//				while(KEY4);
//				return Key_Left;
//			}
//		}
//	}
//	return Key_None;
//}

























