/*
*********************************************************************************************************
*
*	ģ������ : ��������ʱ�����ģ��
*	�ļ����� : bsp_time_measure.c
*	��    �� : V1.0
*	˵    �� : ��������ʱ�����
*	�޸ļ�¼ :
*		�汾��  ����         ����    ˵��
*		v1.0    2012-12-17  Eric2013  ST�̼���V1.0.2�汾��
*	
*   QQ����Ⱥ��216681322
*   UCOS&UCGUI��̳��http://bbs.armfly.com/thread.php?fid=12
*   BLOG: http://blog.sina.com.cn/u/2565749395
*
*********************************************************************************************************
*/
#include "stm32f4xx.h"
#include "bsp_time_measure.h"


/*
*********************************************************************************************************
*	�� �� ��: TIM6_MeasureConfig
*	����˵��: TIM6CLK = 84 MHz, ���Prescaler =N(����Ƶ)
*             TIM6 counter clock = 84 MHz/(N+1)
*      		  TIM6 counter clock = 84 MHz/(6+1) = 12MHz
*             
*			  �������Ĳ���ʱ����5000us���������������શ�ʱ������
*	��    �Σ���
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void TIM6_MeasureConfig(void)
{
   TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;   

   /* ����ʱ�� */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
   
   /* ��ʱ������ */        	
   TIM_BaseInitStructure.TIM_Period = 60000 - 1;
   TIM_BaseInitStructure.TIM_Prescaler = 6;
   TIM_BaseInitStructure.TIM_ClockDivision = 0;
   TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM6, &TIM_BaseInitStructure); 									    
}

/*
*********************************************************************************************************
*	�� �� ��: TIM6_MeasureStart
*	����˵��: ��������
*	��    �Σ���
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
void TIM6_MeasureStart(void)
{
	TIM6->CNT = 0x00;
	TIM_Cmd(TIM6, ENABLE);  									    
}

/*
*********************************************************************************************************
*	�� �� ��: TIM6_MeasureStop
*	����˵��: ��������
*	��    �Σ���
*	�� �� ֵ: ���ز���ʱ�䵥λ 0.083us		        
*********************************************************************************************************
*/
float TIM6_MeasureStop(void)
{
	float temp;

	TIM_Cmd(TIM6, DISABLE);
	temp = TIM6->CNT;
	temp = temp / 12;
	return temp;    									    
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_DelayUS
*	����˵��: ΢����ʱ���� ����Ҫʵ��us��ʱ����Ҫ����65536
*	��    �Σ���
*	�� �� ֵ: ��		        
*********************************************************************************************************
*/
//void bsp_DelayUS(uint16_t _usTime)
//{
//    uint16_t tempold, tempnew;
//	uint16_t tickcnts = 0;
//	
//	TIM6->CNT = 0x00;
//	TIM_Cmd(TIM6, ENABLE);
//	tempold = TIM6->CNT; 
//
//	while(1)
//	{
//		tempnew = TIM6->CNT;
//		if(tempnew != tempold)
//		{
//			/* ע������û���������������65536 */
//			if(tempnew > tempold)
//			{
//				tickcnts += tempnew - tempold;
//			}	    
//			tempold = tempnew;
//			/* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
//			if(tickcnts >= _usTime)break; 
//		}  					
//	
//	}
//
//	TIM_Cmd(TIM6, DISABLE);   									    
//}
