#ifndef _12864OLED_DRV_H_
#define _12864OLED_DRV_H_

#include "stdint.h"
#include "hardware_config.h"

//-----------------OLED端口定义---------------- 

#define OLED_DC_Clr()  GPIOB->BRR = LL_GPIO_PIN_5//DC
#define OLED_DC_Set()  GPIOB->BSRR = LL_GPIO_PIN_5

#define OLED_CS_Clr()  GPIOA->BRR = LL_GPIO_PIN_14//CS
#define OLED_CS_Set()  GPIOA->BSRR = LL_GPIO_PIN_14

#define OLED_RST_Clr()  GPIOB->BRR = LL_GPIO_PIN_4//RST
#define OLED_RST_Set()  GPIOB->BSRR = LL_GPIO_PIN_4


#define IIC_SLAVE_ADDR 0x78		//定义IIC从设备地址
#define OLED_CMD  0				//写命令
#define OLED_DATA 1				//写数据

//--------------OLED参数定义---------------------
#define PAGE_SIZE    8
#define XLevelL		   0x00
#define XLevelH		   0x10
#define YLevel       0xB0
#define	Brightness	 0x01 
#define WIDTH 	     128
#define HEIGHT 	     64	

/**
***********************************************************
* @brief OLED初始化
* @param 
* @return 
***********************************************************
*/
void OLED_Init(void);
void LcdGpioEnable(void);
void LcdGpioDisable(void);
void OLED_Clear(uint8_t dat);
void OLED_Display(void);

void GUI_DrawPoint(uint8_t x, uint8_t y, uint8_t color);
void GUI_Fill(uint8_t sx,uint8_t sy,uint8_t ex,uint8_t ey,uint8_t color);
void GUI_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color);
void GUI_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color);
void GUI_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color);
void GUI_DrawCircle(uint8_t xc, uint8_t yc, uint8_t color, uint8_t r);
void GUI_FillCircle(uint8_t xc, uint8_t yc, uint8_t color, uint8_t r);
void GUI_DrawTriangel(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t color);
void GUI_FillTriangel(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t color);
void GUI_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size,uint8_t mode);
void GUI_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t Size,uint8_t mode);
void GUI_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size,uint8_t mode);
void GUI_ShowFont16(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode);
void GUI_ShowFont24(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode);
void GUI_ShowFont32(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode);
void GUI_ShowCHinese(uint8_t x,uint8_t y,uint8_t hsize,uint8_t *str,uint8_t mode);
void OLED_Print(uint8_t x, uint8_t y, uint8_t hsize,uint8_t *str,uint8_t mode);
void GUI_DrawBMP(uint8_t x,uint8_t y,uint8_t width, uint8_t height, uint8_t BMP[], uint8_t mode); 

#endif
