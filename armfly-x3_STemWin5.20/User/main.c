/*
*********************************************************************************************************
*	                                  
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : 例程。
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
#include "bsp.h"			/* 底层硬件驱动 */


extern void MainTask(void);
/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	int i=0;
	
	bsp_Init();	/* 为了是main函数看起来更简洁些，我们将硬件初始化的代码封装到这个函数 */
	while(1){
		for(i=1;i<5;i++){
			bsp_LedToggle(i);
			bsp_DelayMS(1000);
		}
		for(i=1;i<5;i++){
			bsp_LedToggle(i);
			bsp_DelayMS(1000);
		}
	}
//	MainTask();
}



