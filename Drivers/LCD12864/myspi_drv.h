#ifndef _MYSPI_DRV_H_
#define _MYSPI_DRV_H_

#include <stdint.h>
#include "hardware_config.h"

#define MYSPI_SCL_Clr() GPIOB->BRR = LL_GPIO_PIN_6//SCL
#define MYSPI_SCL_Set() GPIOB->BSRR = LL_GPIO_PIN_6

#define MYSPI_MOSI_Clr() GPIOB->BRR = LL_GPIO_PIN_7//MOSI
#define MYSPI_MOSI_Set() GPIOB->BSRR = LL_GPIO_PIN_7

#define MYSPI_MISO_IN() (GPIOB->IDR>>14&0x01)//��GPIOA_PIN_14���ŵ�ֵ

//#define MYSPI_CS_Clr()  GPIOB->BRR = GPIO_PIN_6//CS��ʼ�ź�
//#define MYSPI_CS_Set()  GPIOB->BSRR = GPIO_PIN_6//��ֹ�ź�

/**
  * @brief ��ʼMySpi_Gpio
  */
void MySpiGpioInit(void);
//��������һ���ֽڣ����ҴӴӻ��õ�һ���ֽ�
uint8_t MySPI_SwapByte(uint8_t ByteSend);

#endif
