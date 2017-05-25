//#include "includes.h"

//void CLOCK_RUN(void)
//{
////  unsigned char str[24]={'2','0','0','0','-','0','0','-','0','0',' ','0','0',':','0','0',':','0','0'};
////	
//////	SysTickConfig();
////////	printf("12316");
//////	Delay_ms(20);
////	LCD_Init();
////	LCD_Clear(WHITE);
//	RTC_Init();

//	str[0]= calendar.w_year/1000+'0';
//	str[1] = calendar.w_year%1000/100+'0';
//	str[2] = calendar.w_year%1000%100/10+'0';
//	str[3] = calendar.w_year%1000%100%10+'0';
//		
//	str[5] = calendar.w_month/10 + '0';
//	str[6] = calendar.w_month%10+'0';
//		
//	str[8] = calendar.w_date/10 +'0';
//	str[9] = calendar.w_date%10+'0';
//		
//	str[11] = calendar.hour/10+'0';
//	str[12] = calendar.hour%10+'0';
//	str[14] = calendar.min/10+'0';
//	str[15] = calendar.min%10+'0';
//	str[17] = calendar.sec/10+'0';
//	str[18] = calendar.sec%10+'0';
//	str[19] = '\0';
//	
//		switch(calendar.week)								   //ÏÔÊ¾ÐÇÆÚ
//	{
//		case 0:
//			Draw_Text_8_16_Str(80,45,WHITE,RED,"Sunday   ");
//			break;
//		case 1:
//			Draw_Text_8_16_Str(80,45,WHITE,RED,"Monday   ");
//			break;
//		case 2:
//			Draw_Text_8_16_Str(80,45,WHITE,RED,"Tuesday  ");
//			break;
//		case 3:
//			Draw_Text_8_16_Str(80,45,WHITE,RED,"Wednesday");
//			break;
//		case 4:
//			Draw_Text_8_16_Str(80,45,WHITE,RED,"Thursday ");
//			break;
//		case 5:
//			Draw_Text_8_16_Str(80,45,WHITE,RED,"Friday   ");
//			break;
//		case 6:
//			Draw_Text_8_16_Str(80,45,WHITE,RED,"Saturday ");
//			break;  
//	}
////	printf("%d\r\n",calendar.sec);
//	Draw_Text_8_16_Str(40,20,WHITE,RED,str);
////Delay_ms(1000);

//}

