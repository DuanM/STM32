#ifndef _____zi_____h______
#define _____zi_____h______
 extern unsigned char zf[];
 extern unsigned char zf_index[];

 extern unsigned char hz[];
 extern unsigned char hz_index[];
 extern const unsigned char asc2_1206[95][12];
 extern const unsigned char asc2_1608[95][16];
 extern const unsigned char asc2_2412[95][36];
 /*****************************************************************************
*****函数名：  ASCII_NUM()
*****参数：    无
*****功能：    返回ASCII字符个数
*****作者：    陈志发
*****创建时间：2013-11-16
*****备注：创建这个函数是解决KEIL中sizeof()必须和结构变量同一个文件才准确识别的BUGE
*****************************************************************************/  
extern unsigned int ASCII_NUM(void);

 /*****************************************************************************
*****函数名：  GB16_NUM()
*****参数：    无
*****功能：    返回GB16字符个数
*****作者：    陈志发
*****创建时间：2013-11-16
*****备注：创建这个函数是解决KEIL中sizeof()必须和结构变量同一个文件才准确识别的BUGE
*****************************************************************************/  
extern unsigned int GB16_NUM(void) ;
 #endif
