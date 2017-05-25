#ifndef _FLASH_H_
#define _FLASH_H_
#include "stm32f10x.h"


#define CMD_WRITE_ENABLE   0x06
#define CMD_WRITE_DISABLE  0x04
#define CMD_READ_REGISTER1 0x05
#define CMD_READ_REGISTER2 0x35
#define CMD_PAGE_PROGRAM   0x02
#define CMD_SECTOR_ERASE   0x20
#define CMD_BLOCK_ERASE    0xD8
#define CMD_FLASH__BE      0xC7

#define CMD_READ_ID        0x9F
#define CMD_READ_DATA      0x03

#define Dummy_Byte         0xA5

#define FLASH_CS_LOW()       GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define FLASH_CS_HIGH()      GPIO_SetBits(GPIOB,GPIO_Pin_12)

#define FLASH_W25Q64_ID   0xEF4017
#define FLASH_DUMMY_BYTE  0xFF

#define FLASH_SPI_PAGESIZE 0x100



void FLASH_Init(void);
u8 SPI2_Send_Receive_Byte(u8 data);
void FLASH_WriteEnable(void);
void FLASH_WaitForWriteEnd(void);
void FLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void FLASH_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void FLASH_ReadBuffer(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t FLASH_ReadID(void);
void FLASH_EraseSector(uint32_t SectorAddr);
void Clear_buffer(u8 *buffer);
void Display_buffer(u8 *buffer);

void Test_Flash(void);
void DMA1_Config(void);
void DMA_Start(u16 lenth);

void FLASH_EraseBlock(uint32_t BlockAddr);
void FLASH_EraseBulk(void);

#endif 


