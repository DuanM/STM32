#include "includes.h"
#include "key.h"
extern void Delay(u32 time);
/*��ʱ��*/

void Key_Config(void)
{
	//ʱ��ʹ��
	GPIO_InitTypeDef GPIO_Initstruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

	
	//����IO��
	GPIO_Initstruct.GPIO_Pin = GPIO_Pin_0;			//���ð���
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
	
	//��ʼ��IO��
	

	

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
��������Get_Key
�βΣ���
����ֵ�����ذ���ֵ
�������ܣ���IO��״̬�����ذ���ֵ
****************************************************/

//u8 Get_Key(void)
//{
//	if(!KEY1 || !KEY2 || !KEY3 || KEY4)//�жϰ����Ƿ���
//	{
//		Delay(50000);
//		if(!KEY1 || !KEY2 || !KEY3 || KEY4)//����
//		{
//			if(!KEY1)
//			{
//				while(!KEY1);//���ּ��
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

























