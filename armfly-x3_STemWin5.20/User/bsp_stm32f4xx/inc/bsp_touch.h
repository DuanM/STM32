/*
*********************************************************************************************************
*
*	ģ������ : ����ʽ����������ģ��
*	�ļ����� : bsp_touch.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
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

#ifndef __BSP_TOUCH_H
#define __BSP_TOUCH_H

typedef struct
{
	uint16_t XYChange;	/* X, Y �Ƿ񽻻�  */

	uint16_t usMaxAdc;	/* ���������ADCֵ��������Ч���ж�. ��СADC = 0  */
	int16_t usAdcNowX;
	int16_t usAdcNowY;

	uint8_t Enable;		/* �������ʹ�ܱ�־ */

}TOUCH_T;

extern TOUCH_T g_tTP;

/* �����¼� */
enum
{
	TOUCH_NONE = 0,		/* �޴��� */
	TOUCH_DOWN = 1,		/* ���� */
	TOUCH_MOVE = 2,		/* �ƶ� */
	TOUCH_RELEASE = 3	/* �ͷ� */
};

/* ���ⲿ���õĺ������� */
void TOUCH_InitHard(void);

void TOUCH_SCAN(void);

#endif

