#include "includes.h"
/*
W25Q64 
8M�ֽ�
128��
ÿ����64K
ÿ������16������
ÿ������4k
ÿ��������Ϊ16ҳ 
1ҳ��256���ֽ�
8M = 128*64K= 2048*4K = 2048*16*256byte
        ��       ����       ҳ
*/

/*
1.GPIO�ڳ�ʼ��
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
2.STM32 SPI��������ʼ��
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
*/
/****************************************************
��������FLASH_Init
�βΣ�
����ֵ��
�������ܣ�FLASH�ĳ�ʼ��
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
	
	//SPIЭ��ĳ�ʼ��
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
ͨ��SPI��������
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
ͨ��SPI��������
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
�жϷ��ͼĴ����Ƿ��ͳɹ�
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);
1.�ȴ�������Ϊ��
2.
*/
/****************************************************
��������SPI2_Send_Receive_Byte
�βΣ�  data -- ��FLASH���͵�����
����ֵ��flash���ص�����
�������ܣ���FLASH�������ݺͶ�ȡ����
****************************************************/
u8 SPI2_Send_Receive_Byte(u8 data)
{
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI2,data);
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI2);
}

/****************************************************
��������FLASH_WriteEnable
�βΣ�
����ֵ��
�������ܣ�дʹ��
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
��������FLASH_WaitForWriteEnd
�βΣ�  
����ֵ��
�������ܣ��ж�FLASH�Ƿ�BUSY
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
��FLASH��д���ݣ�дһ�ε����ݴ�СΪС��256��
1.�ж�FLASH�Ƿ���æ״̬ 1 æ  0 ��æ
2.дʹ��
3.ƬѡCS����
4.����д����
5.����24λ��ַ
6.��SPI��дС��4K���ȵ�����
7.CS����
8.�ж�FLASH״̬�Ƿ�æ
*/
/****************************************************
��������FLASH_WritePage
�βΣ�  pBuffer -- Ҫ��ŵ���
        WriteAddr -- �����ĵ�ַ
        NumByteToWrite -- �����ĸ���
����ֵ��
�������ܣ�FLASH��ҳд����
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

//��FLASH�з���buffer������
/****************************************************
��������FLASH_WriteBuffer
�βΣ�  pBuffer -- Ҫ��ŵ���
        WriteAddr -- �����ĵ�ַ
        NumByteToWrite -- �����ĸ���
����ֵ��
�������ܣ���FLASHд����
****************************************************/
void FLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % FLASH_SPI_PAGESIZE;//�����һҳ���ĸ���ַ��ʼд����
  count = FLASH_SPI_PAGESIZE - Addr;//�����ҳ������д��������
  NumOfPage =  NumByteToWrite / FLASH_SPI_PAGESIZE;//����Ҫ��������ݳ��ȿ���д����ҳ
  NumOfSingle = NumByteToWrite % FLASH_SPI_PAGESIZE;//���������ҳ�����⣬������д�����ݳ���

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
��������FLASH_ReadBuffer
�βΣ�  pBuffer -- ��ȡ�����ݱ��������
        ReadAddr -- ��ȡ��ַ
        NumByteToRead -- ��ȡ�����ĸ���
����ֵ��
�������ܣ���FLASH�ж�����
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

//��ID
/****************************************************
��������FLASH_ReadID
�βΣ�
����ֵ�����ض�����ID��
�������ܣ���ȡFLASH��ID��
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
��������FLASH_EraseSector
�βΣ�  SectorAddr������ַ
����ֵ��
�������ܣ���������
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
FLASH ��С8M 1flash = 128��
128�� 1��=16����
2048����  1���� = 16ҳ

        ��       ����       ҳ
д1ҳ�ռ������
1.��ȡFLASH��ID�Ƿ���������Ҫ��
2.����100��5����5ҳ  addr 100*16*16*256+5*16*256+5*256 = (100*16*16+5*16+5)*256
                                                       = (25600+80+5)*256
                                                       = (25685)*256 
8388608
128��
2048������
32768ҳ
8388608���ֽ�
0x00 0000 ~ 0xff ffff
A23 A22 A21 A20 A19 A18 A17 A16  //A22 ~ A16����ڼ����ַ0x64 0000

A15 A14 A13 A12 A11 A10 A9  A8   //A15 ~ A12����ڼ���������ַ A11~A8����ڼ���ҳ��ַ                
                                   0x5500
A7  A6  A5  A4  A3  A2  A1  A0   //A7~A0����ҳ�ڵ�ַ0x00

2.����ҳ��д256������
3.��ȡ��ҳ������
4.��ʾ��ҳ������
*/
/****************************************************
��������Test_Flash
�βΣ�
����ֵ��
�������ܣ�����FLASH
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
			printf("��ȡ����ID����ȷ!\n\r");
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
	printf("��ʼдFLASH\n\r");
	printf("����д����\n\r");
	FLASH_WritePage(flash_buffer,0x645500,256);
	printf("д�ɹ���\n\r");
	Clear_buffer(flash_buffer);
	printf("��ʼ��FLASH\n\r");
	printf("���ڶ�����\n\r");

	FLASH_ReadBuffer(flash_buffer,0x645505,256);
	
	printf("���ɹ�\n\r");
	Display_buffer(flash_buffer);
}

/****************************************************
��������Clear_buffer
�βΣ�   
����ֵ��
�������ܣ���һ������ȫ����ֵΪ0
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
��������Display_buffer
�βΣ�  bufferҪ��ʾ������ĵ�ַ
����ֵ��
�������ܣ����ڴ�ӡһ������
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
1.W25Q64�������ֲ�
  1.1 ��оƬ��һ����flashΪ�洢�ṹ�Ĵ洢оƬ
  1.2 flash�Ĵ�С��ͨ�����㣩
  1.3 FLASHӲ���Ľ���
  1.3 ��������
  1.4 д����
  1.5 ������
2.д����
  2.1 дʹ��
  2.2 cs����
  2.3 ����д����
  2.3 д24λ��ַ
  2.4 дn�����ȵ�����(n<256)
  2.5 cs���
  2.6 д��һҳ�ж��Ƿ�æ
3.������
  3.1 CS����
  3.2 ���Ͷ�����
  3.3 ����24λ��ַ
  3.4 ��ȡ�Ǹ����ȵ�����
  3.5 CS����
4.STM32��SPI����
  4.1 SPI�������ĳ�ʼ�������ı�д
  4.2 SPI�����ݺ�д���ݺ�����д
  4.3 дʹ�ܺ�����д
  4.4 ��ID�����ı�д
  4.5 ҳд�����ı�д
  4.6 �������ı�д
  4.7 TEST�����ı�д
  4.8 �������������ı�д
  4.9 �궨��
      4.9.1 ����ĺ궨��
	  4.9.2 ID�ĺ궨��
	  4.9.3 page�ĺ궨��
	  4.9.4 dumy�����ĺ궨��
	  4.9.5 CS�������͵ĺ궨��
5.TEST�����ı�д
  5.1 ��ѯID
  5.2 ������ص�����
  5.3 ��һ����ҳ����д����
  5.4 ��ȡ��ҳ
  5.5 ��ʾ��ҳ�ĺ���
*/




