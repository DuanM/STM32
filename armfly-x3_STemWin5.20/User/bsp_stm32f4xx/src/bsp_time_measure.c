/*
*********************************************************************************************************
*
*	模块名称 : 程序运行时间测量模块
*	文件名称 : bsp_time_measure.c
*	版    本 : V1.0
*	说    明 : 程序运行时间测量
*	修改记录 :
*		版本号  日期         作者    说明
*		v1.0    2012-12-17  Eric2013  ST固件库V1.0.2版本。
*	
*   QQ超级群：216681322
*   UCOS&UCGUI论坛：http://bbs.armfly.com/thread.php?fid=12
*   BLOG: http://blog.sina.com.cn/u/2565749395
*
*********************************************************************************************************
*/
#include "stm32f4xx.h"
#include "bsp_time_measure.h"


/*
*********************************************************************************************************
*	函 数 名: TIM6_MeasureConfig
*	功能说明: TIM6CLK = 84 MHz, 如果Prescaler =N(不分频)
*             TIM6 counter clock = 84 MHz/(N+1)
*      		  TIM6 counter clock = 84 MHz/(6+1) = 12MHz
*             
*			  这里最大的测量时间是5000us。如果超出采用嘀嗒定时器测量
*	形    参：无
*	返 回 值: 无		        
*********************************************************************************************************
*/
void TIM6_MeasureConfig(void)
{
   TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;   

   /* 开启时钟 */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
   
   /* 定时器配置 */        	
   TIM_BaseInitStructure.TIM_Period = 60000 - 1;
   TIM_BaseInitStructure.TIM_Prescaler = 6;
   TIM_BaseInitStructure.TIM_ClockDivision = 0;
   TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInit(TIM6, &TIM_BaseInitStructure); 									    
}

/*
*********************************************************************************************************
*	函 数 名: TIM6_MeasureStart
*	功能说明: 启动测量
*	形    参：无
*	返 回 值: 无		        
*********************************************************************************************************
*/
void TIM6_MeasureStart(void)
{
	TIM6->CNT = 0x00;
	TIM_Cmd(TIM6, ENABLE);  									    
}

/*
*********************************************************************************************************
*	函 数 名: TIM6_MeasureStop
*	功能说明: 启动测量
*	形    参：无
*	返 回 值: 返回测量时间单位 0.083us		        
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
*	函 数 名: bsp_DelayUS
*	功能说明: 微妙延时函数 ，主要实现us延时，不要超出65536
*	形    参：无
*	返 回 值: 无		        
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
//			/* 注意这里没有做处理，如果超出65536 */
//			if(tempnew > tempold)
//			{
//				tickcnts += tempnew - tempold;
//			}	    
//			tempold = tempnew;
//			/* 时间超过/等于要延迟的时间,则退出 */
//			if(tickcnts >= _usTime)break; 
//		}  					
//	
//	}
//
//	TIM_Cmd(TIM6, DISABLE);   									    
//}
