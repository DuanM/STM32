#include "includes.h"

//lcd屏的画笔颜色和背景
u16 POINT_COLOR=0x0000;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 
//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(u16 regval)
{ 
	LCD->LCD_REG=regval;//写入要写的寄存器序号	 
}
//写LCD数据
//data:要写入的值
void LCD_WR_DATA(u16 data)
{										    	   
	LCD->LCD_RAM=data;		 
}
//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{										    	   
	return LCD->LCD_RAM;		 
}					   
//写寄存器
//LCD_Reg:寄存器地址
//LCD_RegValue:要写入的数据
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD->LCD_REG = LCD_Reg;		//写入要写的寄存器序号		 
	LCD->LCD_RAM = LCD_RegValue;//写入数据	    		 
}	   
//读寄存器
//LCD_Reg:寄存器地址
//返回值:读到的数据
u16 LCD_ReadReg(u8 LCD_Reg)
{										   
	LCD_WR_REG(LCD_Reg);		//写入要读的寄存器序号
	Delay_us(5);		  
	return LCD_RD_DATA();		//返回读到的值
}   
//开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
 	LCD->LCD_REG=lcddev.wramcmd;	  
}	


//LCD写GRAM
//RGB_Code:颜色值
void LCD_WriteRAM(u16 RGB_Code)
{							    
	LCD->LCD_RAM = RGB_Code;//写十六位GRAM
}
/***************************************************************************************
2^26 =0X0400 0000 = 64MB,每个 BANK 有4*64MB = 256MB
64MB:FSMC_Bank1_NORSRAM1:0X6000 0000 ~ 0X63FF FFFF
64MB:FSMC_Bank1_NORSRAM2:0X6400 0000 ~ 0X67FF FFFF
64MB:FSMC_Bank1_NORSRAM3:0X6800 0000 ~ 0X6BFF FFFF
64MB:FSMC_Bank1_NORSRAM4:0X6C00 0000 ~ 0X6FFF FFFF

选择BANK1-BORSRAM4 连接 TFT，地址范围为0X6C00 0000 ~ 0X6FFF FFFF
FSMC_A23 接LCD的RS(寄存器/数据选择)脚
寄存器基地址 = 0X6C00 0000
RAM基地址 = 0X6D00 0000 = 0X6C00 0000+2^23*2 = 0X6C00 0000 + 0X100 0000 = 0X6D00 0000
当选择不同的地址线时，地址要重新计算  
****************************************************************************************/

void LCD_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能FSMC时钟*/
    
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOD \
	 					  |RCC_APB2Periph_GPIOE |RCC_APB2Periph_GPIOG,\
	                       ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* 配置LCD背光控制管脚*/

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* 配置LCD复位控制管脚*/
    //用的是RESET管脚


	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	/* 配置FSMC相对应的数据线,FSMC-D0~D15: PD 14 15 0 1 8 9 10,PE 7 8 9 10 11 12 13 14 15*/	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1\
	                              |GPIO_Pin_8| GPIO_Pin_9\
	                              |GPIO_Pin_10|GPIO_Pin_14\
	                              |GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8\
	 							| GPIO_Pin_9 | GPIO_Pin_10\
	 						    | GPIO_Pin_11 | GPIO_Pin_12\
	 						    | GPIO_Pin_13 | GPIO_Pin_14\
	   						    | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* 配置FSMC相对应的控制线
	* PD4-FSMC_NOE   :LCD-RD
	* PD5-FSMC_NWE   :LCD-WR
	* PG12-FSMC_NE4  :LCD-CS
	* PE2-FSMC_A23   :LCD-DC
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	//PD4-FSMC_NOE   :LCD-RD

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	//PD5-FSMC_NWE   :LCD-WR
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOG, &GPIO_InitStructure);  //PG12-FSMC_NE4  :LCD-CS

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOG, &GPIO_InitStructure);  //PE2-FSMC_A10   :LCD-DC发送的数据代表是命令还是显示数据

 		    
}

void LCD_FSMC_Config(void)
{
	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  WRITE_USER; 
	FSMC_NORSRAMTimingInitTypeDef  READ_USER; 
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

	READ_USER.FSMC_AddressSetupTime = 0x01;	 //地址建立时间
    READ_USER.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    READ_USER.FSMC_DataSetupTime = 0x0f;		 //数据建立时间
    READ_USER.FSMC_BusTurnAroundDuration = 0x00;
    READ_USER.FSMC_CLKDivision = 0x00;
    READ_USER.FSMC_DataLatency = 0x00;
    READ_USER.FSMC_AccessMode = FSMC_AccessMode_A;	 // 

	WRITE_USER.FSMC_AddressSetupTime = 0x00;	 //地址建立时间
    WRITE_USER.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    WRITE_USER.FSMC_DataSetupTime = 0x03;		 //数据建立时间
    WRITE_USER.FSMC_BusTurnAroundDuration = 0x00;//
    WRITE_USER.FSMC_CLKDivision = 0x00;//
    WRITE_USER.FSMC_DataLatency = 0x00;//
    WRITE_USER.FSMC_AccessMode = FSMC_AccessMode_A;	 // 
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;  //选择外接存储区域的地址
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;//配置数据线和地址线是否复用
//    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;//配置外接存储器的类型
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//设置FSMC接口的数据宽度
	
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;//配置访问模式
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;//配置等待信号的极性
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;//配置是否使用非对齐方式
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//配置等待信号什么时间产生
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;//配置是否使用等待信号
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;//配置是否允许突发写操作
	
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;//配置写操作使能
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable ;//配置是否使用扩展模式
    
	
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &READ_USER;//读时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &WRITE_USER;  //写时序
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* 使能 FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);

}
void LCD_init_RGB(void)
{
	Delay_ms(50); // delay 50 ms 
	
 	LCD_WriteReg(0x0000,0x0001);
	 
	Delay_ms(50); // delay 50 ms 
  	lcddev.id = LCD_ReadReg(0x0000);  
	 
  	if(lcddev.id<0XFF||lcddev.id==0XFFFF||lcddev.id==0X9300)//读到ID不正确,新增lcddev.id==0X9300判断，因为9341在未被复位的情况下会被读成9300
	{	
 		//尝试9341 ID的读取		
		LCD_WR_REG(0XD3);				   
		LCD_RD_DATA(); 				//dummy read 	
 		LCD_RD_DATA();   	    	//读到0X00
  		lcddev.id=LCD_RD_DATA();   	//读取93								   
 		lcddev.id<<=8;
		lcddev.id|=LCD_RD_DATA();  	//读取41 	   			   
 		if(lcddev.id!=0X9341)		//非9341,尝试是不是6804
		{	
 			LCD_WR_REG(0XBF);				   
			LCD_RD_DATA(); 			//dummy read 	 
	 		LCD_RD_DATA();   	    //读回0X01			   
	 		LCD_RD_DATA(); 			//读回0XD0 			  	
	  		lcddev.id=LCD_RD_DATA();//这里读回0X68 
			lcddev.id<<=8;
	  		lcddev.id|=LCD_RD_DATA();//这里读回0X04	   	  
 		} 
		if(lcddev.id!=0X9341&&lcddev.id!=0X6804)lcddev.id=0x9341;//新增，用于识别9341 	     
	}
	 
	if(lcddev.id==0X9341)	//9341初始化  使用的是扩展模式
	{	 
		LCD_WR_REG(0xCF);      
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0xC1); 
		LCD_WR_DATA(0X30); 
		
		LCD_WR_REG(0xED);  
		LCD_WR_DATA(0x64); 
		LCD_WR_DATA(0x03); 
		LCD_WR_DATA(0X12); 
		LCD_WR_DATA(0X81);
		
		LCD_WR_REG(0xE8);  
		LCD_WR_DATA(0x85); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x7A); 
		
		LCD_WR_REG(0xCB);  
		LCD_WR_DATA(0x39); 
		LCD_WR_DATA(0x2C); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x34); 
		LCD_WR_DATA(0x02);
		
		LCD_WR_REG(0xF7);  
		LCD_WR_DATA(0x20);
		
		LCD_WR_REG(0xEA);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00);
		
		LCD_WR_REG(0xC0);    //Power control 
		LCD_WR_DATA(0x1B);   //VRH[5:0] 
		
		LCD_WR_REG(0xC1);    //Power control 
		LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
		
		LCD_WR_REG(0xC5);    //VCM control 
		LCD_WR_DATA(0x30); 	 //3F
		LCD_WR_DATA(0x30); 	 //3C
		
		LCD_WR_REG(0xC7);    //VCM control2 
		LCD_WR_DATA(0XB7); 
		
		LCD_WR_REG(0x36);    // Memory Access Control 
		LCD_WR_DATA(0x48); 
		
		LCD_WR_REG(0x3A);   
		LCD_WR_DATA(0x55); 
		
		LCD_WR_REG(0xB1);   
		LCD_WR_DATA(0x00);   
		LCD_WR_DATA(0x1A); 
		
		LCD_WR_REG(0xB6);    // Display Function Control 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0xA2); 
		
		LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
		LCD_WR_DATA(0x00); 
		
		LCD_WR_REG(0x26);    //Gamma curve selected 
		LCD_WR_DATA(0x01); 
		
		LCD_WR_REG(0xE0);    //Set Gamma 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x2A); 
		LCD_WR_DATA(0x28); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x0E); 
		LCD_WR_DATA(0x08); 
		LCD_WR_DATA(0x54); 
		LCD_WR_DATA(0XA9); 
		LCD_WR_DATA(0x43); 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 
		
		LCD_WR_REG(0XE1);    //Set Gamma 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x15); 
		LCD_WR_DATA(0x17); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x11); 
		LCD_WR_DATA(0x06); 
		LCD_WR_DATA(0x2B); 
		LCD_WR_DATA(0x56); 
		LCD_WR_DATA(0x3C); 
		LCD_WR_DATA(0x05); 
		LCD_WR_DATA(0x10); 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x3F); 
		LCD_WR_DATA(0x0F); 
		
		LCD_WR_REG(0x2B); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x01);
		LCD_WR_DATA(0x3f);
		
		LCD_WR_REG(0x2A); 
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0x00);
		LCD_WR_DATA(0xef);
		
		LCD_WR_REG(0x11); //Exit Sleep
		Delay_ms(120);
		LCD_WR_REG(0x29); //display on	
	}
}

void LCD_Init(void)
{
	LCD_GPIO_Config();
	
	LCD_FSMC_Config();
	
	LCD_init_RGB();
		
	/* 开背光 */
	LCD_DisplayOn();
	
	LCD_Display_Dir(0);		 	//0默认为竖屏   1  默认为横屏
	
	LCD_Clear(WHITE);	
}


//Xpos:横坐标

//Ypos:纵坐标

void LCD_SetCursor(u16 Xpos, u16 Ypos)
{    
      if(lcddev.id==0X9341)
      {               
		  LCD_WR_REG(lcddev.setxcmd);
		  LCD_WR_DATA(Xpos>>8);
		  LCD_WR_DATA(Xpos&0XFF);  
		  LCD_WR_REG(lcddev.setycmd);
		  LCD_WR_DATA(Ypos>>8);
		  LCD_WR_DATA(Ypos&0XFF);
      }
}


//设置LCD的自动扫描方向
//一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
//dir:0~7,代表8个方向(具体定义见lcd.h) 	   
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u8 dirreg=0;
	u16 temp;  
	if(lcddev.dir==1&&lcddev.id!=0X6804)//横屏时，对6804不改变扫描方向！
	{			   
		switch(dir)//方向转换
		{
			case 0:dir=6;break;
			case 1:dir=7;break;
			case 2:dir=4;break;
			case 3:dir=5;break;
			case 4:dir=1;break;
			case 5:dir=0;break;
			case 6:dir=3;break;
			case 7:dir=2;break;	     
		}
	}
	if(lcddev.id==0x9341||lcddev.id==0X6804)//9341/6804,很特殊
	{
		switch(dir)
		{
			case L2R_U2D://从左到右,从上到下
				regval|=(0<<7)|(0<<6)|(0<<5); 
				break;
			case L2R_D2U://从左到右,从下到上
				regval|=(1<<7)|(0<<6)|(0<<5); 
				break;
			case R2L_U2D://从右到左,从上到下
				regval|=(0<<7)|(1<<6)|(0<<5); 
				break;
			case R2L_D2U://从右到左,从下到上
				regval|=(1<<7)|(1<<6)|(0<<5); 
				break;	 
			case U2D_L2R://从上到下,从左到右
				regval|=(0<<7)|(0<<6)|(1<<5); 
				break;
			case U2D_R2L://从上到下,从右到左
				regval|=(0<<7)|(1<<6)|(1<<5); 
				break;
			case D2U_L2R://从下到上,从左到右
				regval|=(1<<7)|(0<<6)|(1<<5); 
				break;
			case D2U_R2L://从下到上,从右到左
				regval|=(1<<7)|(1<<6)|(1<<5); 
				break;	 
		}
		dirreg=0X36;
 		regval|=0X08;//BGR   
		if(lcddev.id==0X6804)regval|=0x02;//6804的BIT6和9341的反了	   
		LCD_WriteReg(dirreg,regval);
 		if((regval&0X20)||lcddev.dir==1)
		{
			if(lcddev.width<lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}else  
		{
			if(lcddev.width>lcddev.height)//交换X,Y
			{
				temp=lcddev.width;
				lcddev.width=lcddev.height;
				lcddev.height=temp;
 			}
		}  
		LCD_WR_REG(lcddev.setxcmd); 
		LCD_WR_DATA(0);
		LCD_WR_DATA(0);
		LCD_WR_DATA((lcddev.width-1)>>8);
		LCD_WR_DATA((lcddev.width-1)&0XFF);
		
		LCD_WR_REG(lcddev.setycmd); 
		LCD_WR_DATA(0);LCD_WR_DATA(0);
		LCD_WR_DATA((lcddev.height-1)>>8);LCD_WR_DATA((lcddev.height-1)&0XFF);  
  	}
} 
//设置LCD显示方向
//dir:0,竖屏；1,横屏
void LCD_Display_Dir(u8 dir)
{
	if(dir==0)			//竖屏
	{
		lcddev.dir=0;	//竖屏
		lcddev.width=240;
		lcddev.height=320;
		if(lcddev.id==0X9341||lcddev.id==0X6804)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
			if(lcddev.id==0X6804)
			{
				lcddev.width=320;
				lcddev.height=480;
			}
		}
	}
	else 				//横屏
	{	  				
		lcddev.dir=1;	//横屏
		lcddev.width=320;
		lcddev.height=240;
		if(lcddev.id==0X9341)
		{
			lcddev.wramcmd=0X2C;
	 		lcddev.setxcmd=0X2A;
			lcddev.setycmd=0X2B;  	 
		}
	} 
	LCD_Scan_Dir(DFT_SCAN_DIR);	//默认扫描方向
}

//画点
//x,y:坐标
//POINT_COLOR:此点的颜色
void LCD_DrawPoint(u16 x,u16 y,u16 c)
{	 
	LCD_SetCursor(x,y);		//设置光标位置 
	LCD_WriteRAM_Prepare();	//开始写入GRAM
	LCD->LCD_RAM=c; 
}

void LCD_DisplayOn(void)//开显示
{
	/* 开背光 */
	GPIO_SetBits(GPIOB, GPIO_Pin_0);	
}
void LCD_DisplayOff(void)//关显示
{
	/*关背光 */
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

//清屏函数
//color:要清屏的填充色
void LCD_Clear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	totalpoint*=lcddev.height; 			//得到总点数
	LCD_SetCursor(0x00,0x0000);	//设置光标位置 
	LCD_WriteRAM_Prepare();     		//开始写入GRAM	 	  
	for(index=0;index<totalpoint;index++)
	{
		LCD->LCD_RAM=color;	   
	}
}  



/**************************************************************************
***** 函数名：  show_8x16(u32 x ,u32 y,u16 backColor,u16 color,const u8 *ch)
***** 功能：    绘制大小为16x16的字
***** 参数：    x：横坐标   y：纵坐标     color文字颜色  
   	            backColor  文字背景颜色  ch：字模数组
***** 返回值：	无
***** 创建者:	陈志发
***** 创建时间： 2011-03-30
***** 最后更新:2011-03-30
****************************************************************************/
void  show_8x16(u32 x ,u32 y,u16 backColor,u16 color,const u8 *ch)
{
	int x0,y0;
	u16 dat;
	for(y0=0;y0<16;y0++)                   //换行
	{				//0110 1110
		dat=*ch;    //汉字宽度为16，由两个字节组成一个16位数据
		ch++;                            //每次取两个字节
		for(x0=0;x0<8;x0++)             //换列
		{
			//画点
			if(dat&0x80)                  //从最高开描点，只须判断一位
				LCD_DrawPoint(x+x0,y+y0,color);    //描字符颜色
			else
			{
				LCD_DrawPoint(x+x0,y+y0,backColor);  //描背景颜色
			}
			dat<<=1;                      //将次高位，提升为最高位     
		}
	}
}

/**************************************************************************
***** 函数名：  show_16x16(u32 x ,u32 y,u16 backColor,u16 color,const u8 *ch)
***** 功能：    绘制大小为16x16的字
***** 参数：    x：横坐标   y：纵坐标     color文字颜色  
   	            backColor  文字背景颜色  ch：字模数组
***** 返回值：	无
***** 创建者:	陈志发
***** 创建时间： 2011-03-30
***** 最后更新:2011-03-30
****************************************************************************/
void  show_16x16(u32 x ,u32 y,u16 backColor,u16 color,const u8 *ch)
{
	int x0,y0;
	u16 dat;
	for(y0=0;y0<16;y0++)                   //换行
	{                              //0x10,0x02 0x1000 | 0x02 = 0x1002
		dat=(*ch<<8) | *(ch+1);    //汉字宽度为16，由两个字节组成一个16位数据
		ch+=2;                            //每次取两个字节
		for(x0=0;x0<16;x0++)             //换列
		{
			//画点
			if(dat&0x8000)                  //从最高开描点，只须判断一位
				LCD_DrawPoint(x+x0,y+y0,color);    //描字符颜色
			else
			{
				LCD_DrawPoint(x+x0,y+y0,backColor);  //描背景颜色
			}
			dat<<=1;                      //将次高位，提升为最高位     
		}
	}
}
/**************************************************************************
***** 函数名：  void  show_xXx(u32 x ,u32 y,u16 backColor,u16 color,const u8 *ch ,u16 size_line,u16 size_bits)
***** 功能：    绘制大小为size_line*size_bits的汉字
***** 参数：    x：横坐标   y：纵坐标     color文字颜色  
   	            backColor  文字背景颜色  ch：字模数组
***** 返回值：	无
***** 创建者:	
***** 创建时间： 
***** 最后更新:
****************************************************************************/
void  show_xXx(u32 x ,u32 y,u16 backColor,u16 color,const u8 *ch ,u16 size_line,u16 size_bits)
{
	int x0,y0;
	u32 dat;
	int i;
	for(y0=0;y0<size_line;y0++)                   //换行
	{                              //0x10,0x02 0x1000 | 0x02 = 0x1002
		    //汉字宽度为16，由两个字节组成一个16位数据
		for(i=0;i<size_bits/8;i++)                           //每次取两个字节
		{
			dat = *ch++;
			for(x0=0;x0<8;x0++)             //换列
			{
				//画点
				if(dat&0x80)                  //从最高开描点，只须判断一位
					LCD_DrawPoint(x+(x0+i*8),y+y0,color);    //描字符颜色
				else
				{
					LCD_DrawPoint(x+(x0+i*8),y+y0,backColor);  //描背景颜色
				}
				dat<<=1;                      //将次高位，提升为最高位     
			}
			
		}
	}
}

/**************************************************************************
***** 函数名：  void Draw_Text_8_16_Str(u32 x,u32 y,u16 backColor,u16 color,const unsigned char *chs)
***** 功能：    绘制大小为16×8，16x16的字符串
***** 参数：    x：横坐标   y：纵坐标     color文字颜色  
   	            backColor  文字背景颜色  ch：字模数组
***** 返回值：	无
***** 创建者:	
***** 创建时间： 
***** 最后更新:
****************************************************************************/
void Draw_Text_8_16_Str(u32 x,u32 y,u16 backColor,u16 color,const unsigned char *chs)
{
	u32 i=0;
	while (*chs != '\0')
	{
		if (*chs > 0xa0)				//汉字内码都是大于0xa0
		{
			for (i=0 ;i < GB16_NUM();i++)
			{	
				if(x>235){x=0;y=y+16;}

				if ((*chs == hz_index[i*2]) && (*(chs+1) == hz_index[i*2+1]))
				{
					show_16x16(x,y,backColor,color,&hz[32*(i)]);
					x +=16;
					break;
				}
			}
			chs+=2;
		}

		else	 //如果是字符执行以下
		{	if(x>230){x=0;y=y+16;}
			for (i=0 ;i < ASCII_NUM();i++)
			{
				if (*chs == zf_index[i])
				{
					show_8x16(x,y,backColor,color,&zf[16*i]);
					x += 8;
					break;
				}
			}
			chs++;
		}	 
	}
}
#define FONT_SIZE   (16)
#define HZ_INDEX(hz)    ((hz[0] - 0xa1) * 94 + (hz[1] - 0xa1))  
#define DOTS_BYTES  (FONT_SIZE * FONT_SIZE / 8) 

unsigned char dots[32]; 
u32 zk_addr1 = 0;
void Draw_Text_16_16_Str_zk(u32 x,u32 y,u16 backColor,u16 color,const unsigned char *chs)
{
//	u32 i=0;
	while (*chs != '\0')
	{
		zk_addr1 = HZ_INDEX(chs) * DOTS_BYTES;//找到Flash的数据地址
//zk_addr1 = ((chs[0] - 0xa1) * 94 + (chs[1] - 0xa1)) * 32;


//		printf("zk_addr1 = %d\r\n",zk_addr1);
		FLASH_ReadBuffer(dots,zk_addr1,32);//读出Flash中的数据
//		for(i = 0;i<32;i++)
//		{
//				printf("dots[%d] = %x\r\n",i,dots[i]);
//		}
		if(x>235)
		{
			x=0;
			y=y+16;
		}
		show_16x16(x,y,backColor,color,dots);
		x +=16;
		chs+=2;
	}
}


////在指定位置显示一个字符
////x,y:起始坐标
////num:要显示的字符:" "--->"~"
////size:字体大小 12/16/24
////mode:叠加方式(1)还是非叠加方式(0)
//void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
//{  							  
//    u8 temp,t1,t;
//	u16 y0=y;
//	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
// 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
//	for(t=0;t<csize;t++)
//	{   
//		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
//		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
//		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
//		else return;								//没有的字库
//		for(t1=0;t1<8;t1++)
//		{			    
//			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
//			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
//			temp<<=1;
//			y++;
//			if(y>=lcddev.height)return;		//超区域了
//			if((y-y0)==size)
//			{
//				y=y0;
//				x++;
//				if(x>=lcddev.width)return;	//超区域了
//				break;
//			}
//		}  	 
//	}  	    	   	 	  
//}   
////m^n函数
////返回值:m^n次方.
//u32 LCD_Pow(u8 m,u8 n)
//{
//	u32 result=1;	 
//	while(n--)result*=m;    
//	return result;
//}			 
////显示数字,高位为0,则不显示
////x,y :起点坐标	 
////len :数字的位数
////size:字体大小
////color:颜色 
////num:数值(0~4294967295);	 
//void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
//{         	
//	u8 t,temp;
//	u8 enshow=0;						   
//	for(t=0;t<len;t++)
//	{
//		temp=(num/LCD_Pow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
//				continue;
//			}else enshow=1; 
//		 	 
//		}
//	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
//	}
//} 
////显示数字,高位为0,还是显示
////x,y:起点坐标
////num:数值(0~999999999);	 
////len:长度(即要显示的位数)
////size:字体大小
////mode:
////[7]:0,不填充;1,填充0.
////[6:1]:保留
////[0]:0,非叠加显示;1,叠加显示.
//void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
//{  
//	u8 t,temp;
//	u8 enshow=0;						   
//	for(t=0;t<len;t++)
//	{
//		temp=(num/LCD_Pow(10,len-t-1))%10;
//		if(enshow==0&&t<(len-1))
//		{
//			if(temp==0)
//			{
//				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
//				else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
// 				continue;
//			}else enshow=1; 
//		 	 
//		}
//	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
//	}
//} 
////显示字符串
////x,y:起点坐标
////width,height:区域大小  
////size:字体大小
////*p:字符串起始地址		  
//void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
//{         
//	u8 x0=x;
//	width+=x;
//	height+=y;
//    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
//    {       
//        if(x>=width){x=x0;y+=size;}
//        if(y>=height)break;//退出
//        LCD_ShowChar(x,y,*p,size,0);
//        x+=size/2;
//        p++;
//    }  
//}


/**************************************************************************
***** 函数名：   Paint_Bmp()
***** 功能：     显示指定大小图片
***** 参数：     x0 开始位置，y0开始位置  ，high高度，width 宽度 ，bmp 图片数据数组
***** 返回值：	 无
***** 创建者:	
***** 创建时间： 
***** 最后更新:  
****************************************************************************/
void Paint_Bmp(int x0,int y0,int width,int high,const unsigned char bmp[])
{
	int x,y;
	u32 c;
	int p = 0;
	
    for( y = y0 ; y < (high+y0) ; y++ )
    {
    	for( x = x0 ; x < (width+x0) ; x++ )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;
			if ( ( (x) < LCD_WIDTH) && ( (y) < LCD_HEIGHT) )
			LCD_DrawPoint(x,y,c);  
    		p = p + 2 ;
    	}
    }
}

/**************************************************************************
***** 函数名：   Clear_Bmp()
***** 功能：     清屏图片
***** 参数：     x0 开始位置，y0开始位置  ，high高度，width 宽度 ，bmp 图片数据数组
***** 返回值：	 无
***** 创建者:	
***** 创建时间： 
***** 最后更新:  
****************************************************************************/
void Clear_Bmp(u16 y0,u16 wordhight,const unsigned char *ch)
{
	int i,y,a;
for(i=0;i<240;i++)
{
	show_16x16(i,100,WHITE,RED,hz);//字符串
	for(y=y0;y<(y0+wordhight);y++)
	{
		a=ch[y*240*2+i*2]<<8|ch[y*240*2+i*2+1];
		LCD_DrawPoint(i,y,a);
	}
}
	Delay_us(10000);
}


/**************************************************************************
***** 函数名：   my_itoa()
***** 功能：     字符转换
***** 参数：     num 数字    *str 字符串    radix      进制
***** 返回值：	 字符
***** 创建者:	
***** 创建时间： 
***** 最后更新:  
****************************************************************************/


char *my_itoa(int num,char  *str,int radix)
{
	const char table[]="0123456789ABCDEF";
	char *ptr = str;
	char *start = NULL;			//清空指针
	char temp;
	
	char  negative = 0;			
	if(num == 0)
	{
	  *ptr++ = '0';
		//*ptr=\'0';
		return str;
	
	}
	if(num<0)

	{
	*ptr++='0';
	num = num *(-1);
	negative = 1;

	}

	while(num)
	{
	*ptr++ = table[num%radix];
	num/=radix;
	}
	//*ptr = '0';
	start = (negative ? str+1:str);
	//ptr--;
	while(start<ptr)
	{
  	temp=*start;
		*start = *ptr;
		*ptr=temp;
		start++;
		ptr--;
	}
	
	return str;
}


