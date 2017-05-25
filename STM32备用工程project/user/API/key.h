#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f10x.h"
#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)		//读取按键1  up       上

#define KEY2 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)		//读取按键2     right   右
#define KEY3 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)		//读取按键3    down     下
#define KEY4 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)		//读取按键4   left      左
void Key_Config(void);
void Set_All(void);
void Close_All(void);

typedef enum                                         //枚举按键的状态
{
	Key_None = 0,Key_Up = 1,Key_Down,Key_Left,Key_Right
}KEY_Type;

extern KEY_Type Key_Switch; 
                         //声明一个枚举变量
void GPIO_KEY_Init(void);                            //按键的初始化
u8 Get_Key(void);   								 //获得按键值


#endif

