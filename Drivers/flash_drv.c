#include "py32f0xx_hal.h"
#include "flash_drv.h"

/****
addr:起始地址
data:写入数据的地址
len:多少个字节
*****/
uint8_t FlashProgramTest(uint32_t addr,uint8_t *data,uint16_t len)
{
	uint32_t PAGEError = 0;
	uint32_t PAGEEaddr = 0;
	uint16_t PAGEElen = 0;
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
	//解锁闪存
	HAL_FLASH_Unlock();
	//擦除闪存
	PAGEElen=(len+FLASH_PAGE_SIZE-1)/ FLASH_PAGE_SIZE; //计算需要擦除几页，向上取整
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGEERASE;   		//按页擦除
	EraseInitStruct.PageAddress = addr;            					//起始地址
	EraseInitStruct.NbPages  =   PAGEElen;  						//擦除几页
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)  //是否擦除成功
	{
		printf("擦除失败1！！！");
		return 1;
	}
	////检查闪存是否为空
	while (PAGEEaddr < PAGEElen)
	{
		if (0xFFFFFFFF != HW32_REG(addr + PAGEEaddr))
		{
			printf("擦除失败2！！！");
			return 2;
		}
		PAGEEaddr += 4;
	}
	
	uint32_t flash_program_start = addr ;      //起始地址
	uint32_t flash_program_end = (addr + len); //结束地址
	uint32_t *src = (uint32_t *)data;          //数据起始地址

	while (flash_program_start < flash_program_end)
	{//写入数据
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, flash_program_start, src) == HAL_OK)
		{
			flash_program_start += FLASH_PAGE_SIZE; //更新起始地址
			src += FLASH_PAGE_SIZE / 4;             //更新数据指针
		}
	}
	//锁定闪存
	HAL_FLASH_Lock();
	return 3;
}
/**
  * @brief  读取FLASH数据
  * @param 	pBuffer，存储读出数据的指针
  * @param   ReadAddr，读取地址
  * @param   NumByteToRead，读取数据长度
  * @retval 无
**/
void FLASH_BufferRead(uint32_t ReadAddr, uint8_t* pBuffer, uint32_t NumByteToRead)
{
  uint32_t addr = 0;
	uint32_t *src=(uint32_t *)pBuffer;
  while (addr < NumByteToRead)
  {
    *(src++)=HW32_REG(ReadAddr + addr);
    addr += 4;
  }
}

/**********************************************
//解锁闪存
HAL_FLASH_Unlock();
//擦除闪存
FlashErase();
//检查闪存是否为空
FlashBlank();
//写入数据
FlashProgram();
//锁定闪存
HAL_FLASH_Lock();
验证数据
FlashVerify();

**********************************************/
