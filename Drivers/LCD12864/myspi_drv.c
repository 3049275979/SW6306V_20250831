
#include "myspi_drv.h"
/**
  * @brief 初始MySpi_Gpio
  */
void MySpiGpioInit(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
	
	/*时钟线SPI SCLK GPIO Config*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*主机输入MISO GPIO Config*/
//	GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
//    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
//    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/*主机输出MOSI GPIO Config*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
//	/*SPI CS GPIO Confgi*/
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStruct.Pin = GPIO_PIN_6;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
//	MYSPI_CS_Set();  //初始化好给片选引脚拉高
	MYSPI_SCL_Clr();  //时钟引脚拉低
}

//主机发送一个字节，并且从从机得到一个字节
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	uint8_t i, ByteReceive = 0x00;
	
	for (i = 0; i < 8; i ++)
	{
		if(ByteSend&0x80)
		   MYSPI_MOSI_Set();
		else 
		   MYSPI_MOSI_Clr();
		ByteSend<<=1;
		MYSPI_SCL_Set();
		if (MYSPI_MISO_IN() == 1){ByteReceive |= (0x80 >> i);}
		MYSPI_SCL_Clr();
	}
	
	return ByteReceive;
}

