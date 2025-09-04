#ifndef __FLASH_DRV_H
#define __FLASH_DRV_H

#include <stdint.h>
#include <stdio.h>

#define MCU1_FLASH_START_ADDR     0x0800B000
#define MCU2_FLASH_START_ADDR     0x0800D000

uint8_t FlashProgramTest(uint32_t addr,uint8_t *data,uint16_t len);
void FLASH_BufferRead(uint32_t ReadAddr, uint8_t* pBuffer, uint32_t NumByteToRead);

#endif
