#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f10x.h"
#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)		//��ȡ����1  up       ��

#define KEY2 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)		//��ȡ����2     right   ��
#define KEY3 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)		//��ȡ����3    down     ��
#define KEY4 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)		//��ȡ����4   left      ��
void Key_Config(void);
void Set_All(void);
void Close_All(void);

typedef enum                                         //ö�ٰ�����״̬
{
	Key_None = 0,Key_Up = 1,Key_Down,Key_Left,Key_Right
}KEY_Type;

extern KEY_Type Key_Switch; 
                         //����һ��ö�ٱ���
void GPIO_KEY_Init(void);                            //�����ĳ�ʼ��
u8 Get_Key(void);   								 //��ð���ֵ


#endif

