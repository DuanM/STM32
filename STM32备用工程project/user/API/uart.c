#include "stm32f10x.h"
#include "uart.h"
#include "string.h"
#include "led.h"
#include "stdio.h"

/****************************************************
��������UART1_Init
�βΣ���
����ֵ����
�������ܣ�����1�ĳ�ʼ��  PA9 -- TX  PA10 -- RX
****************************************************/


void Uart1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_Initstruct;
	//��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);

//����A9
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;					//����A9
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;		//�������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);								//��ʼ���ܽ�
	
	//����A10
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//��������
  GPIO_Init(GPIOA,&GPIO_InitStruct);

	
	USART_Initstruct.USART_BaudRate = 115200;
	USART_Initstruct.USART_WordLength = USART_WordLength_8b;
	USART_Initstruct.USART_Parity = USART_Parity_No;
	USART_Initstruct.USART_StopBits = USART_StopBits_1;						//ֹͣλΪ1
	USART_Initstruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    //��Ӳ��������
	USART_Initstruct.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(USART1,&USART_Initstruct);
	
	//����ʹ��
	USART_Cmd(USART1,ENABLE);

}



/****************************************************
��������USART1_Echo
�βΣ���
����ֵ����
�������ܣ�����OPEN����LED��
****************************************************/
u8 str[50];
void USART1_Echo(void)
{
	int i=0;

	do
	{
		while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)!=SET);
		str[i]=USART_ReceiveData(USART1);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
		USART_SendData(USART1,USART_ReceiveData(USART1));
	}while(str[i++]!='#');
	str[i-1]='\0';

	if(strcmp((const char*)str,"11")==0)
		Led_Light1();
if(strcmp((const char*)str,"21")==0)
		Led_Light2();

}


//fputc

int fputc(int c,FILE *fp)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
	USART_SendData(USART1,c);
	return c;
}
