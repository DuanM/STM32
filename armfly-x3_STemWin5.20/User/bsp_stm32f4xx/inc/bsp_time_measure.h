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

#ifndef _BSP_TIME_MEASURE_H
#define _BSP_TIME_MEASURE_H



void TIM6_MeasureConfig(void);
void TIM6_MeasureStart(void);
//void bsp_DelayUS(uint16_t _usTime);
float TIM6_MeasureStop(void);


#endif
