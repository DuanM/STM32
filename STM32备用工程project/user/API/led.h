#ifndef _LED_H_



#define _LED_H_


void Led_Config(void);
void Led_Light1(void);
void Led_Light2(void);
void Led_Light3(void);
void Led_Light4(void);

#define LED1(x) x?GPIO_ResetBits(GPIOA,GPIO_Pin_5):GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define LED2(x) x?GPIO_ResetBits(GPIOA,GPIO_Pin_6):GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define LED3(x) x?GPIO_ResetBits(GPIOB,GPIO_Pin_5):GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define LED4(x) x?GPIO_ResetBits(GPIOE,GPIO_Pin_5):GPIO_SetBits(GPIOE,GPIO_Pin_5)


void Led_Dark(void);
#endif
