#include "includes.h"
/*
W25Q64 
8M字节
128块
每块是64K
每块又有16个扇区
每个扇区4k
每个扇区分为16页 
1页是256个字节
8M = 128*64K= 2048*4K = 2048*16*256byte
        块       扇区       页
*/

/*
1.GPIO口初始化
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
2.STM32 SPI控制器初始化
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
*/
/****************************************************
函数名：FLASH_Init
形参：
返回值：
函数功能：FLASH的初始化
****************************************************/
void FLASH_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
	//MOSI -- PB15 SCK -- PB13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//CS
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//MISO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//SPI协议的初始化
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2,&SPI_InitStructure);

	SPI_Cmd(SPI2,ENABLE);
}

/*
通过SPI发送数据
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
通过SPI接受数据
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
判断发送寄存器是否发送成功
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
1.等待发送区为空
2.
*/
/****************************************************
函数名：SPI2_Send_Receive_Byte
形参：  data -- 给FLASH发送的数据
返回值：flash返回的数据
函数功能：给FLASH发送数据和读取数据
****************************************************/
u8 SPI2_Send_Receive_Byte(u8 data)
{
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2,data);
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI2);
}

/****************************************************
函数名：FLASH_WriteEnable
形参：
返回值：
函数功能：写使能
****************************************************/
void FLASH_WriteEnable(void)
{
  /*!< Select the FLASH: Chip Select low */
  FLASH_CS_LOW();

  /*!< Send "Write Enable" instruction */
  SPI2_Send_Receive_Byte(CMD_WRITE_ENABLE);

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_CS_HIGH();
}
/****************************************************
函数名：FLASH_WaitForWriteEnd
形参：  
返回值：
函数功能：判断FLASH是否BUSY
****************************************************/
void FLASH_WaitForWriteEnd(void)
{
  uint8_t flashstatus = 0;

  /*!< Select the FLASH: Chip Select low */
  FLASH_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  SPI2_Send_Receive_Byte(CMD_READ_REGISTER1);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    flashstatus = SPI2_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  }
  while ((flashstatus & 0x01) == SET); /* Write in progress */

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_CS_HIGH();
}
/*
往FLASH中写数据（写一次的数据大小为小于256）
1.判断FLASH是否处于忙状态 1 忙  0 不忙
2.写使能
3.片选CS拉低
4.发送写命令
5.发送24位地址
6.往SPI中写小于4K长度的数据
7.CS拉高
8.判断FLASH状态是否忙
*/
/****************************************************
函数名：FLASH_WritePage
形参：  pBuffer -- 要存放的数
        WriteAddr -- 存数的地址
        NumByteToWrite -- 存数的个数
返回值：
函数功能：FLASH的页写功能
****************************************************/
void FLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the FLASH */
  FLASH_WriteEnable();

  /*!< Select the FLASH: Chip Select low */
  FLASH_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  SPI2_Send_Receive_Byte(CMD_PAGE_PROGRAM);
  /*!< Send WriteAddr high nibble address byte to write to */
  SPI2_Send_Receive_Byte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  SPI2_Send_Receive_Byte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  SPI2_Send_Receive_Byte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    SPI2_Send_Receive_Byte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  FLASH_WaitForWriteEnd();
}

//向FLASH中发送buffer的数据
/****************************************************
函数名：FLASH_WriteBuffer
形参：  pBuffer -- 要存放的数
        WriteAddr -- 存数的地址
        NumByteToWrite -- 存数的个数
返回值：
函数功能：往FLASH写数据
****************************************************/
void FLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % FLASH_SPI_PAGESIZE;//代表从一页中哪个地址开始写数据
  count = FLASH_SPI_PAGESIZE - Addr;//代表该页还可以写多少数据
  NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;//代表要存入的数据长度可以写多少页
  NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;//代表除了整页数据外，还可以写的数据长度

  if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      FLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      while(NumOfPage--)
      {
        FLASH_WritePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr +=  FLASH_SPI_PAGESIZE;
        pBuffer += FLASH_SPI_PAGESIZE;
      }
      FLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sFLASH_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
      {
        temp = NumOfSingle - count;

        FLASH_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        FLASH_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        FLASH_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sFLASH_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;

      FLASH_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        FLASH_WritePage(pBuffer, WriteAddr, FLASH_SPI_PAGESIZE);
        WriteAddr +=  FLASH_SPI_PAGESIZE;
        pBuffer += FLASH_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        FLASH_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}
/****************************************************
函数名：FLASH_ReadBuffer
形参：  pBuffer -- 读取的数据保存的数组
        ReadAddr -- 读取地址
        NumByteToRead -- 读取参数的个数
返回值：
函数功能：从FLASH中读数据
****************************************************/
void FLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  /*!< Select the FLASH: Chip Select low */
  FLASH_CS_LOW();

  /*!< Send "Read from Memory " instruction */
  SPI2_Send_Receive_Byte(CMD_READ_DATA);

  /*!< Send ReadAddr high nibble address byte to read from */
  SPI2_Send_Receive_Byte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  SPI2_Send_Receive_Byte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  SPI2_Send_Receive_Byte(ReadAddr & 0xFF);

  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the FLASH */
    *pBuffer = SPI2_Send_Receive_Byte(FLASH_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_CS_HIGH();
}

//读ID
/****************************************************
函数名：FLASH_ReadID
形参：
返回值：返回读到的ID号
函数功能：读取FLASH的ID号
****************************************************/
uint32_t FLASH_ReadID(void)
{
  uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /*!< Select the FLASH: Chip Select low */
  FLASH_CS_LOW();

  /*!< Send "RDID " instruction */
  SPI2_Send_Receive_Byte(CMD_READ_ID);

  /*!< Read a byte from the FLASH */
  Temp0 = SPI2_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp1 = SPI2_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Read a byte from the FLASH */
  Temp2 = SPI2_Send_Receive_Byte(FLASH_DUMMY_BYTE);

  /*!< Deselect the FLASH: Chip Select high */
  FLASH_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}

/****************************************************
函数名：FLASH_EraseSector
形参：  SectorAddr扇区地址
返回值：
函数功能：扇区擦除
****************************************************/
void FLASH_EraseSector(uint32_t SectorAddr)
{
  /*!< Send write enable instruction */
  FLASH_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  FLASH_CS_LOW();
  /*!< Send Sector Erase instruction */
  SPI2_Send_Receive_Byte(CMD_SECTOR_ERASE);
  /*!< Send SectorAddr high nibble address byte */
  SPI2_Send_Receive_Byte((SectorAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  SPI2_Send_Receive_Byte((SectorAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  SPI2_Send_Receive_Byte(SectorAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  FLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  FLASH_WaitForWriteEnd();
}

void FLASH_EraseBlock(uint32_t BlockAddr)
{
  /*!< Send write enable instruction */
  FLASH_WriteEnable();

  /*!< Sector Erase */
  /*!< Select the FLASH: Chip Select low */
  FLASH_CS_LOW();
  /*!< Send Sector Erase instruction */
  SPI2_Send_Receive_Byte(CMD_BLOCK_ERASE);
  /*!< Send SectorAddr high nibble address byte */
  SPI2_Send_Receive_Byte((BlockAddr & 0xFF0000) >> 16);
  /*!< Send SectorAddr medium nibble address byte */
  SPI2_Send_Receive_Byte((BlockAddr & 0xFF00) >> 8);
  /*!< Send SectorAddr low nibble address byte */
  SPI2_Send_Receive_Byte(BlockAddr & 0xFF);
  /*!< Deselect the FLASH: Chip Select high */
  FLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  FLASH_WaitForWriteEnd();
}

void FLASH_EraseBulk(void)
{
  /*!< Send write enable instruction */
  FLASH_WriteEnable();

  /*!< Bulk Erase */
  /*!< Select the FLASH: Chip Select low */
  FLASH_CS_LOW();
  /*!< Send Bulk Erase instruction  */
  SPI2_Send_Receive_Byte(CMD_FLASH__BE);
  /*!< Deselect the FLASH: Chip Select high */
  FLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  FLASH_WaitForWriteEnd();
}
/*
8M = 128*64K= 128*16*4K = 128*16*16*256byte
FLASH 大小8M 1flash = 128块
128块 1块=16扇区
2048扇区  1扇区 = 16页

        块       扇区       页
写1页空间的数据
1.读取FLASH的ID是否是我们需要的
2.擦除100块5扇区5页  addr 100*16*16*256+5*16*256+5*256 = (100*16*16+5*16+5)*256
                                                       = (25600+80+5)*256
                                                       = (25685)*256 
8388608
128块
2048个扇区
32768页
8388608个字节
0x00 0000 ~ 0xff ffff
A23 A22 A21 A20 A19 A18 A17 A16  //A22 ~ A16代表第几块地址0x64 0000

A15 A14 A13 A12 A11 A10 A9  A8   //A15 ~ A12代表第几个扇区地址 A11~A8代表第几个页地址                
                                   0x5500
A7  A6  A5  A4  A3  A2  A1  A0   //A7~A0代表页内地址0x00

2.往该页中写256个数据
3.读取该页的数据
4.显示该页的数据
*/
/****************************************************
函数名：Test_Flash
形参：
返回值：
函数功能：测试FLASH
****************************************************/
u8 flash_buffer[256];
void Test_Flash(void)
{
	u32 i;
	u32 flash_ID;
	u8 flash_flag = 0;
	do
	{
		if(flash_flag)
		{
			printf("读取到的ID不正确!\n\r");
			Delay_ms(500);
		}
		flash_ID = FLASH_ReadID();
		printf("%x\n\r",flash_ID);
		flash_flag = 1;
	}while(flash_ID != FLASH_W25Q64_ID);
	FLASH_EraseSector(0x645000);
	for(i=0;i<256;i++)
	{
		flash_buffer[i] = i;
	}
	printf("开始写FLASH\n\r");
	printf("正在写……\n\r");
	FLASH_WritePage(flash_buffer,0x645500,256);
	printf("写成功！\n\r");
	Clear_buffer(flash_buffer);
	printf("开始读FLASH\n\r");
	printf("正在读……\n\r");

	FLASH_ReadBuffer(flash_buffer,0x645505,256);
	
	printf("读成功\n\r");
	Display_buffer(flash_buffer);
}

/****************************************************
函数名：Clear_buffer
形参：   
返回值：
函数功能：给一个数组全部赋值为0
****************************************************/
void Clear_buffer(u8 *buffer)
{
	u32 i = 0;
	for(i=0;i<256;i++)
	{
		buffer[i] = 0;
	}
}
/****************************************************
函数名：Display_buffer
形参：  buffer要显示的数组的地址
返回值：
函数功能：串口打印一个数组
****************************************************/
void Display_buffer(u8 *buffer)
{
	u32 i,j;
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			printf("%x\t",*buffer++);
		}
		printf("\n\r");
	}
}


/*
1.W25Q64的数据手册
  1.1 该芯片是一个以flash为存储结构的存储芯片
  1.2 flash的大小（通过换算）
  1.3 FLASH硬件的介绍
  1.3 操作命令
  1.4 写过程
  1.5 读过程
2.写过程
  2.1 写使能
  2.2 cs拉低
  2.3 发送写命令
  2.3 写24位地址
  2.4 写n个长度的数据(n<256)
  2.5 cs变高
  2.6 写完一页判断是否忙
3.读过程
  3.1 CS拉低
  3.2 发送读命令
  3.3 发送24位地址
  3.4 读取那个长度的数据
  3.5 CS拉高
4.STM32的SPI操作
  4.1 SPI控制器的初始化函数的编写
  4.2 SPI读数据和写数据函数编写
  4.3 写使能函数编写
  4.4 读ID函数的编写
  4.5 页写函数的编写
  4.6 读函数的编写
  4.7 TEST函数的编写
  4.8 扇区擦除函数的编写
  4.9 宏定义
      4.9.1 命令的宏定义
	  4.9.2 ID的宏定义
	  4.9.3 page的宏定义
	  4.9.4 dumy参数的宏定义
	  4.9.5 CS拉高拉低的宏定义
5.TEST函数的编写
  5.1 查询ID
  5.2 擦除相关的扇区
  5.3 给一定的页进行写数据
  5.4 读取该页
  5.5 显示该页的函数
*/




