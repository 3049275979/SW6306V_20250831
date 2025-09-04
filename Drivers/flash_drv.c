#include "py32f0xx_hal.h"
#include "flash_drv.h"

/****
addr:��ʼ��ַ
data:д�����ݵĵ�ַ
len:���ٸ��ֽ�
*****/
uint8_t FlashProgramTest(uint32_t addr,uint8_t *data,uint16_t len)
{
	uint32_t PAGEError = 0;
	uint32_t PAGEEaddr = 0;
	uint16_t PAGEElen = 0;
	FLASH_EraseInitTypeDef EraseInitStruct = {0};
	//��������
	HAL_FLASH_Unlock();
	//��������
	PAGEElen=(len+FLASH_PAGE_SIZE-1)/ FLASH_PAGE_SIZE; //������Ҫ������ҳ������ȡ��
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGEERASE;   		//��ҳ����
	EraseInitStruct.PageAddress = addr;            					//��ʼ��ַ
	EraseInitStruct.NbPages  =   PAGEElen;  						//������ҳ
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)  //�Ƿ�����ɹ�
	{
		printf("����ʧ��1������");
		return 1;
	}
	////��������Ƿ�Ϊ��
	while (PAGEEaddr < PAGEElen)
	{
		if (0xFFFFFFFF != HW32_REG(addr + PAGEEaddr))
		{
			printf("����ʧ��2������");
			return 2;
		}
		PAGEEaddr += 4;
	}
	
	uint32_t flash_program_start = addr ;      //��ʼ��ַ
	uint32_t flash_program_end = (addr + len); //������ַ
	uint32_t *src = (uint32_t *)data;          //������ʼ��ַ

	while (flash_program_start < flash_program_end)
	{//д������
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, flash_program_start, src) == HAL_OK)
		{
			flash_program_start += FLASH_PAGE_SIZE; //������ʼ��ַ
			src += FLASH_PAGE_SIZE / 4;             //��������ָ��
		}
	}
	//��������
	HAL_FLASH_Lock();
	return 3;
}
/**
  * @brief  ��ȡFLASH����
  * @param 	pBuffer���洢�������ݵ�ָ��
  * @param   ReadAddr����ȡ��ַ
  * @param   NumByteToRead����ȡ���ݳ���
  * @retval ��
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
//��������
HAL_FLASH_Unlock();
//��������
FlashErase();
//��������Ƿ�Ϊ��
FlashBlank();
//д������
FlashProgram();
//��������
HAL_FLASH_Lock();
��֤����
FlashVerify();

**********************************************/
