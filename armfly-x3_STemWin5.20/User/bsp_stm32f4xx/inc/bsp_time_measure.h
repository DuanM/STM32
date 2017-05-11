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

#ifndef _BSP_TIME_MEASURE_H
#define _BSP_TIME_MEASURE_H



void TIM6_MeasureConfig(void);
void TIM6_MeasureStart(void);
//void bsp_DelayUS(uint16_t _usTime);
float TIM6_MeasureStop(void);


#endif
