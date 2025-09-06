#include "12864oled_drv.h"
#include "stdlib.h"
#include "string.h" 
#include "oledfont.h" 
#include "myspi_drv.h"

//数组每个bit存储OLED每个像素点的颜色值(1-亮(白色),0-灭(黑色))
//每个数组元素表示1列8个像素点，一共128列
static unsigned char OLED_buffer[1024] = { 0 };


/*****************************************************************************
 * @name       :void Write_IIC_Command(uint8_t IIC_Command)
 * @date       :2018-09-13 
 * @function   :Write a byte of command to oled screen
 * @parameters :IIC_Command:command to be written
 * @retvalue   :None
******************************************************************************/
void Write_IIC_Command(uint8_t IIC_Command)
{
//	OLED_DC_Clr();
//	OLED_CS_Clr();
//	SPI1_Send_Byte(&IIC_Command,1);
//	OLED_CS_Set();
//	OLED_DC_Set();
	OLED_DC_Clr();
	OLED_CS_Clr();
	MySPI_SwapByte(IIC_Command);				 		  
	OLED_CS_Set();
	OLED_DC_Set();
}

/*****************************************************************************
 * @name       :void Write_IIC_Data(uint8_t IIC_Data)
 * @date       :2018-09-13 
 * @function   :Write a byte of data to oled screen
 * @parameters :IIC_Data:data to be written
 * @retvalue   :None
******************************************************************************/
void Write_IIC_Data(uint8_t IIC_Data)
{
//	OLED_DC_Set();
//	OLED_CS_Clr();
//	SPI1_Send_Byte(&IIC_Data,1);
//	OLED_CS_Set();
//	OLED_DC_Set();
	OLED_DC_Set();
	OLED_CS_Clr();
	MySPI_SwapByte(IIC_Data);				 		  
	OLED_CS_Set();
	OLED_DC_Set();
}


/*******************************************************************
 * @name       :void OLED_WR_Byte(unsigned dat,unsigned cmd)
 * @date       :2018-08-27
 * @function   :Write a byte of content to the OLED screen
 * @parameters :dat:Content to be written
                cmd:0-write command
								    1-write data
 * @retvalue   :None
********************************************************************/
void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
	{
		 Write_IIC_Data(dat);
	}
	else
	{
		Write_IIC_Command(dat);
	}
}

/*******************************************************************
 * @name       :void OLED_Set_Pos(unsigned char x, unsigned char y) 
 * @date       :2018-08-27
 * @function   :Set coordinates in the OLED screen
 * @parameters :x:x coordinates
                y:y coordinates
 * @retvalue   :None
********************************************************************/
void OLED_Set_Pos(unsigned char x, unsigned char y) 
{
 	OLED_WR_Byte(YLevel+y/PAGE_SIZE,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}  

/*******************************************************************
 * @name       :void OLED_Display_On(void) 
 * @date       :2018-08-27
 * @function   :Turn on OLED display
 * @parameters :None
 * @retvalue   :None
********************************************************************/ 	  
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

/*******************************************************************
 * @name       :void OLED_Display_Off(void)
 * @date       :2018-08-27
 * @function   :Turn off OLED display
 * @parameters :None
 * @retvalue   :None
********************************************************************/    
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}

/*******************************************************************
 * @name       :void OLED_Set_Pixel(unsigned char x, unsigned char y,unsigned char color)
 * @date       :2018-08-27
 * @function   :set the value of pixel to RAM
 * @parameters :x:the x coordinates of pixel
                y:the y coordinates of pixel
								color:the color value of the point
								      1-white
											0-black
 * @retvalue   :None
********************************************************************/ 
void OLED_Set_Pixel(unsigned char x, unsigned char y,unsigned char color)
{
	if(color)
	{
		OLED_buffer[(y/PAGE_SIZE)*WIDTH+x]|= (1<<(y%PAGE_SIZE))&0xff;
	}
	else
	{
		OLED_buffer[(y/PAGE_SIZE)*WIDTH+x]&= ~((1<<(y%PAGE_SIZE))&0xff);
	}
}		   			 

/*******************************************************************
 * @name       :void OLED_Display(void)
 * @date       :2018-08-27
 * @function   :Display in OLED screen
 * @parameters :None
 * @retvalue   :None
********************************************************************/  
void OLED_Display(void)
{
	uint8_t i,n;		    
	for(i=0;i<(HEIGHT/8);i++)  
	{  
		OLED_WR_Byte (YLevel+i,OLED_CMD);    //设置页地址（0~3）
		OLED_WR_Byte (XLevelL,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (XLevelH,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<WIDTH;n++)
		{
			OLED_WR_Byte(OLED_buffer[i*WIDTH+n],OLED_DATA); 
		}
	}   //更新显示
}

/*******************************************************************
 * @name       :void OLED_Clear(unsigned dat)  
 * @date       :2018-08-27
 * @function   :clear OLED screen
 * @parameters :dat:0-Display full black
                    1-Display full white
 * @retvalue   :None
********************************************************************/ 
void OLED_Clear(uint8_t dat)  
{  
	if(dat)
	{
		memset(OLED_buffer,0xff,sizeof(OLED_buffer));
	}
	else
	{
		memset(OLED_buffer,0,sizeof(OLED_buffer));
	}
//	OLED_Display();
}
void LcdGpioEnable(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_4;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*主机输入MISO GPIO Config*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//	GPIO_InitStruct.Alternate = LL_GPIO_AF0_SWJ;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/*时钟线SPI SCLK GPIO Config*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
//	/*主机输入MISO GPIO Config*/
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
	OLED_RST_Clr();
	LL_mDelay(20);//等待LCD上电稳定
	OLED_RST_Set();
}
void LcdGpioDisable(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_4;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*时钟线SPI SCLK GPIO Config*/
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*主机输入MISO GPIO Config*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/*主机输出MOSI GPIO Config*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
//OLED的初始化
void OLED_Init(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0}; 
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_5|LL_GPIO_PIN_4;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
//	GPIO_InitStruct.Alternate = LL_GPIO_AF0_SWJ;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Alternate = LL_GPIO_AF13_TIM1;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	OLED_RST_Clr();
	LL_mDelay(20);//等待LCD上电稳定
	OLED_RST_Set();

//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	GPIO_InitStruct.Pin = GPIO_PIN_15;
//	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//	GPIO_InitTypeDef GPIO_InitStruct = {0};
//	__HAL_RCC_GPIOF_CLK_ENABLE();                               /* GPIOF时钟使能 */
//	
//	/**I2C引脚配置
//	PF0     ------> I2C1_SCL
//	PF1     ------> I2C1_SDA
//	*/
//	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;                     /* 推挽 */
//	GPIO_InitStruct.Pull = GPIO_PULLUP;                         /* 上拉 */
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);                     /* GPIO初始化 */
//	HAL_Delay(200);
	/**************初始化SSD1306*****************/	
	//	OLED_Display_Off(); //power off
	//OLED初始化
//	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
//	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
//	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
//	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
//	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
//	OLED_WR_Byte(0x01,OLED_CMD);// Set SEG Output Current Brightness
//	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
//	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
//	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
//	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
//	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
//	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
//	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
//	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
//	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
//	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
//	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
//	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
//	OLED_WR_Byte(0x12,OLED_CMD);
//	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
//	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
//	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
//	OLED_WR_Byte(0x02,OLED_CMD);//
//	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
//	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable	
//	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
//	OLED_Display_On(); // power on
//	OLED_Clear(0);
//	OLED_Display();
	//LCD初始化
	OLED_WR_Byte(0xe2,OLED_CMD);//软件复位
	LL_mDelay(20);//等待LCD上电稳定
	OLED_WR_Byte(0xae|0x00,OLED_CMD);//LCD关闭显示
	OLED_WR_Byte(0xa2|0x00,OLED_CMD);//偏压比
	OLED_WR_Byte(0x20|0x03,OLED_CMD);//设置V0电阻比例
	OLED_WR_Byte(0xa0|(0x00 ? 0x01:0x00),OLED_CMD);//垂直镜像
	OLED_WR_Byte(0xc0|(0x00 ? 0x00:0x08),OLED_CMD);//水平镜像
	OLED_WR_Byte(0xa6|(0x00 ? 0x01:0x00),OLED_CMD);//反显
	OLED_WR_Byte(0x81,OLED_CMD);//液晶驱动电压调整（对比度）
	OLED_WR_Byte(0x20,OLED_CMD);
	OLED_WR_Byte(0xa4|0x00,OLED_CMD);//关全部显示
	OLED_WR_Byte(0xf8,OLED_CMD);OLED_WR_Byte(0xf8,OLED_CMD);//升压比
	OLED_WR_Byte(0x28|0x04,OLED_CMD);//开升压器
	OLED_WR_Byte(0x28|0x04|0x02,OLED_CMD);//开电压调整器
	OLED_WR_Byte(0x28|0x04|0x02|0x01,OLED_CMD);//开电压跟随器
	OLED_WR_Byte(0x40|0x00,OLED_CMD);//设置0页起始行
	OLED_Clear(0);//OLED12832清零
	OLED_Display();//OLED12832更新显存
	OLED_Display_On();//开显示
}

/*******************************************************************
 * @name       :void GUI_DrawPoint(uint8_t x,uint8_t y,uint8_t color)
 * @date       :2018-08-27
 * @function   :draw a point in LCD screen
 * @parameters :x:the x coordinate of the point
                y:the y coordinate of the point
								color:the color value of the point
								      1-white
											0-black
 * @retvalue   :None
********************************************************************/
void GUI_DrawPoint(uint8_t x,uint8_t y,uint8_t color)
{
	OLED_Set_Pixel(x,y,color);
//	OLED_Display();
}

/*******************************************************************
 * @name       :void GUI_Fill(uint8_t sx,uint8_t sy,uint8_t ex,uint8_t ey,uint8_t color)
 * @date       :2018-08-27 
 * @function   :fill the specified area
 * @parameters :sx:the bebinning x coordinate of the specified area
                sy:the bebinning y coordinate of the specified area
								ex:the ending x coordinate of the specified area
								ey:the ending y coordinate of the specified area
								color:the color value of the the specified area
								      1-white
											0-black
 * @retvalue   :None
********************************************************************/
void GUI_Fill(uint8_t sx,uint8_t sy,uint8_t ex,uint8_t ey,uint8_t color)
{  	
	uint8_t i,j;			
	uint8_t width=ex-sx+1; 		//得到填充的宽度
	uint8_t height=ey-sy+1;		//高度
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
				OLED_Set_Pixel(sx+j, sy+i,color);
		}		
	}
//	OLED_Display();
}

/*******************************************************************
 * @name       :void GUI_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color)
 * @date       :2018-08-27 
 * @function   :Draw a line between two points
 * @parameters :x1:the bebinning x coordinate of the line
                y1:the bebinning y coordinate of the line
								x2:the ending x coordinate of the line
								y2:the ending y coordinate of the line
								color:the color value of the line
								      1-white
											0-black
 * @retvalue   :None
********************************************************************/
void GUI_DrawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{ 
		OLED_Set_Pixel(uRow,uCol,color);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
//	OLED_Display();
} 

/*****************************************************************************
 * @name       :void GUI_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color)
 * @date       :2018-08-27 
 * @function   :Draw a rectangle
 * @parameters :x1:the bebinning x coordinate of the rectangle
                y1:the bebinning y coordinate of the rectangle
								x2:the ending x coordinate of the rectangle
								y2:the ending y coordinate of the rectangle
								color:the color value of the rectangle
								      1-white
											0-black							  
 * @retvalue   :None
******************************************************************************/
void GUI_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color)
{
	GUI_DrawLine(x1,y1,x2,y1,color);
	GUI_DrawLine(x1,y1,x1,y2,color);
	GUI_DrawLine(x1,y2,x2,y2,color);
	GUI_DrawLine(x2,y1,x2,y2,color);
}  

/*****************************************************************************
 * @name       :void GUI_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color)
 * @date       :2018-08-27
 * @function   :Filled a rectangle
 * @parameters :x1:the bebinning x coordinate of the filled rectangle
                y1:the bebinning y coordinate of the filled rectangle
								x2:the ending x coordinate of the filled rectangle
								y2:the ending y coordinate of the filled rectangle
								color:the color value of the rectangle
								      1-white
											0-black	
 * @retvalue   :None
******************************************************************************/  
void GUI_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2,uint8_t color)
{
	GUI_Fill(x1,y1,x2,y2,color);
}
 
/*****************************************************************************
 * @name       :static void _draw_circle_8(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y, uint8_t color)
 * @date       :2018-08-27 
 * @function   :8 symmetry circle drawing algorithm (internal call)
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
								x:the x coordinate relative to the Circular center 
								y:the y coordinate relative to the Circular center 
								color:the color value of the rectangle
								      1-white
											0-black	
 * @retvalue   :None
******************************************************************************/  
static void _draw_circle_8(uint8_t xc, uint8_t yc, uint8_t x, uint8_t y, uint8_t color)
{
	OLED_Set_Pixel(xc + x, yc + y, color);
	OLED_Set_Pixel(xc - x, yc + y, color);
	OLED_Set_Pixel(xc + x, yc - y, color);
	OLED_Set_Pixel(xc - x, yc - y, color);
	OLED_Set_Pixel(xc + y, yc + x, color);
	OLED_Set_Pixel(xc - y, yc + x, color);
	OLED_Set_Pixel(xc + y, yc - x, color);
	OLED_Set_Pixel(xc - y, yc - x, color);
}

/*****************************************************************************
 * @name       :void GUI_DrawCircle(uint8_t xc, uint8_t yc, uint8_t color, uint8_t r)
 * @date       :2018-08-27
 * @function   :Draw a circle of specified size at a specified location
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
								r:Circular radius
								color:the color value of the rectangle
								      1-white
											0-black	
 * @retvalue   :None
******************************************************************************/  
void GUI_DrawCircle(uint8_t xc, uint8_t yc, uint8_t color, uint8_t r)
{
	int x = 0, y = r,d;
	d = 3 - 2 * r;
	while (x <= y) 
	{
		_draw_circle_8(xc, yc, x, y, color);
		if (d < 0) 
		{
				d = d + 4 * x + 6;
		}
		else 
		{
				d = d + 4 * (x - y) + 10;
				y--;
		}
		x++;
	}
//	OLED_Display();
}

/*****************************************************************************
 * @name       :void GUI_FillCircle(uint8_t xc, uint8_t yc, uint8_t color, uint8_t r)
 * @date       :2018-08-27
 * @function   :Fill a circle of specified size at a specified location
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
								r:Circular radius
								color:the color value of the rectangle
								      1-white
											0-black	
 * @retvalue   :None
******************************************************************************/  
void GUI_FillCircle(uint8_t xc, uint8_t yc, uint8_t color, uint8_t r)
{
	int x = 0, y = r, yi, d;
	d = 3 - 2 * r;
	while (x <= y) 
	{
			for (yi = x; yi <= y; yi++)
			{
				_draw_circle_8(xc, yc, x, yi, color);
			}
			if (d < 0) 
			{
				d = d + 4 * x + 6;
			} 
			else 
			{
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
	}
//	OLED_Display();
}

/**********************************************************************************
 * @name       :void GUI_DrawTriangel(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t color)
 * @date       :2018-08-27 
 * @function   :Draw a triangle at a specified position
 * @parameters :x0:the bebinning x coordinate of the triangular edge 
                y0:the bebinning y coordinate of the triangular edge 
								x1:the vertex x coordinate of the triangular
								y1:the vertex y coordinate of the triangular
								x2:the ending x coordinate of the triangular edge 
								y2:the ending y coordinate of the triangular edge 
								color:the color value of the rectangle
								      1-white
											0-black	
 * @retvalue   :None
***********************************************************************************/ 
void GUI_DrawTriangel(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t color)
{
	GUI_DrawLine(x0,y0,x1,y1,color);
	GUI_DrawLine(x1,y1,x2,y2,color);
	GUI_DrawLine(x2,y2,x0,y0,color);
}

/*****************************************************************************
 * @name       :static void _swap(uint8_t *a, uint8_t *b)
 * @date       :2018-08-27
 * @function   :Exchange two numbers(internal call)
 * @parameters :a:the address of the first number 
                b:the address of the second number
 * @retvalue   :None
******************************************************************************/  
static void _swap(uint8_t *a, uint8_t *b)
{
	uint16_t tmp;
  tmp = *a;
	*a = *b;
	*b = tmp;
}

/*****************************************************************************
 * @name       :static void _draw_h_line(uint8_t x0,uint8_t x1,uint8_t y,uint8_t color)
 * @date       :2018-08-27
 * @function   :draw a horizontal line in RAM(internal call)
 * @parameters :x0:the bebinning x coordinate of the horizontal line
                x1:the ending x coordinate of the horizontal line
								y:the y coordinate of the horizontal line
								color:the color value of the rectangle
								      1-white
											0-black	
 * @retvalue   :None
******************************************************************************/
static void _draw_h_line(uint8_t x0,uint8_t x1,uint8_t y,uint8_t color)
{
	uint8_t i=0;
	for(i=x0;i<=x1;i++)
	{
		OLED_Set_Pixel(i, y, color);
	}
}

/*****************************************************************************
 * @name       :void GUI_FillTriangel(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t color)
 * @date       :2018-08-27 
 * @function   :filling a triangle at a specified position
 * @parameters :x0:the bebinning x coordinate of the triangular edge 
                y0:the bebinning y coordinate of the triangular edge 
								x1:the vertex x coordinate of the triangular
								y1:the vertex y coordinate of the triangular
								x2:the ending x coordinate of the triangular edge 
								y2:the ending y coordinate of the triangular edge 
								color:the color value of the rectangle
								      1-white
											0-black	
 * @retvalue   :None
******************************************************************************/ 
void GUI_FillTriangel(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t color)
{
	uint8_t a, b, y, last;
	int dx01, dy01, dx02, dy02, dx12, dy12;
	long sa = 0;
	long sb = 0;
 	if (y0 > y1) 
	{
    _swap(&y0,&y1); 
		_swap(&x0,&x1);
 	}
 	if (y1 > y2) 
	{
    _swap(&y2,&y1); 
		_swap(&x2,&x1);
 	}
  if (y0 > y1) 
	{
    _swap(&y0,&y1); 
		_swap(&x0,&x1);
  }
	if(y0 == y2) 
	{ 
		a = b = x0;
		if(x1 < a)
    {
			a = x1;
    }
    else if(x1 > b)
    {
			b = x1;
    }
    if(x2 < a)
    {
			a = x2;
    }
		else if(x2 > b)
    {
			b = x2;
    }
		_draw_h_line(a,b,y0,color);
    return;
	}
	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;
	
	if(y1 == y2)
	{
		last = y1; 
	}
  else
	{
		last = y1-1; 
	}
	for(y=y0; y<=last; y++) 
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
    sb += dx02;
    if(a > b)
    {
			_swap(&a,&b);
		}
		_draw_h_line(a,b,y,color);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) 
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if(a > b)
		{
			_swap(&a,&b);
		}
		_draw_h_line(a,b,y,color);
	}
//	OLED_Display();
}

/*****************************************************************************
 * @name       :void GUI_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size,uint8_t mode)
 * @date       :2018-08-27 
 * @function   :Display a single English character
 * @parameters :x:the bebinning x coordinate of the Character display position
                y:the bebinning y coordinate of the Character display position
								chr:the ascii code of display character(0~94)
								Char_Size:the size of display character(8,16)
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/ 
void GUI_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size,uint8_t mode)
{      	
	  unsigned char c=0,i=0,tmp,j=0;	
		c=chr-' ';//得到偏移后的值			
		if(x>WIDTH-1){x=0;y=y+2;}
		if(Char_Size ==16)
		{
			for(i=0;i<16;i++)
			{
			  if(mode)
				{
					tmp = F8X16[c*16+i];
				}
				else
				{
					tmp = ~(F8X16[c*16+i]);
				}
				for(j=0;j<8;j++)
				{
					if(tmp&(0x80>>j))
					{
						OLED_Set_Pixel(x+j, y+i,1);
					}
					else
					{
						OLED_Set_Pixel(x+j, y+i,0);
					}
				}
			}
		}
		else if(Char_Size==8)
		{	
				for(i=0;i<8;i++)
				{
				  if(mode)
					{
						tmp = F6x8[c][i];
					}
					else
					{
						tmp = ~(F6x8[c][i]);
					}
					for(j=0;j<8;j++)
					{
						if(tmp&(0x80>>j))
						{
							OLED_Set_Pixel(x+j, y+i,1);
						}
						else
						{
							OLED_Set_Pixel(x+j, y+i,0);
						}
					}
			}
	}
	else
	{
		return;
	}
}

/*****************************************************************************
 * @name       :void GUI_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size,uint8_t mode)
 * @date       :2018-08-27 
 * @function   :Display English string
 * @parameters :x:the bebinning x coordinate of the English string
                y:the bebinning y coordinate of the English string
								chr:the start address of the English string
								Char_Size:the size of display character
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/   	  
void GUI_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size,uint8_t mode)
{
	unsigned char j=0,csize;
	if(Char_Size == 16)
  {
	  csize = Char_Size/2;
	}
  else if(Char_Size == 8)
  {
	  csize = Char_Size/2+2;
	}
	else
	{
		return;
	}
	while (chr[j]!='\0')
	{	
		GUI_ShowChar(x,y,chr[j],Char_Size,mode);
		x+=csize;
		if(x>120)
		{
			x=0;
			y+=Char_Size;
		}
		j++;
	}
//	OLED_Display();
}

/*****************************************************************************
 * @name       :u32 mypow(uint8_t m,uint8_t n)
 * @date       :2018-08-27 
 * @function   :get the nth power of m (internal call)
 * @parameters :m:the multiplier
                n:the power
 * @retvalue   :the nth power of m
******************************************************************************/ 
static uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

/*****************************************************************************
 * @name       :void GUI_ShowNum(uint8_t x,uint8_t y,u32 num,uint8_t len,uint8_t Size,uint8_t mode)
 * @date       :2018-08-27 
 * @function   :Display number
 * @parameters :x:the bebinning x coordinate of the number
                y:the bebinning y coordinate of the number
								num:the number(0~4294967295)
								len:the length of the display number
								Size:the size of display number
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/  			 
void GUI_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t Size,uint8_t mode)
{         	
	uint8_t t,temp;
	uint8_t enshow=0,csize;
  if(Size == 16)
  {
	  csize = Size/2;
	}
  else if(Size == 8)
  {
	  csize = Size/2+2;
	} 	
	else
	{
		return;
	}
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				GUI_ShowChar(x+csize*t,y,' ',Size,mode);
				continue;
			}else enshow=1; 
		 	 
		}
	 	GUI_ShowChar(x+csize*t,y,temp+'0',Size,mode); 
	}
//	OLED_Display();
}  

/*****************************************************************************
 * @name       :void GUI_ShowFont16(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode)
 * @date       :2018-08-27 
 * @function   :Display a single 16x16 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								s:the start address of the Chinese character
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/ 
void GUI_ShowFont16(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode)
{
	uint8_t i,j,k,tmp;
	uint16_t num;
	num = sizeof(cfont16)/sizeof(typFNT_GB16);
  for(i=0;i<num;i++)
	{
		if((cfont16[i].Index[0]==*s)&&(cfont16[i].Index[1]==*(s+1)))
		{
			for(j=0;j<32;j++)
			{
				if(mode)
				{
					tmp = cfont16[i].Msk[j];
				}
				else
				{
					tmp = ~(cfont16[i].Msk[j]);
				}
				for(k=0;k<8;k++)
				{
					if(tmp&(0x80>>k))
					{
						OLED_Set_Pixel(x+(j%2)*8+k, y+j/2,1);
					}
					else
					{
						OLED_Set_Pixel(x+(j%2)*8+k, y+j/2,0);
					}
				}
			}	
			break;
		}	
	}
}

/*****************************************************************************
 * @name       :void GUI_ShowFont24(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode)
 * @date       :2018-08-27 
 * @function   :Display a single 24x24 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								s:the start address of the Chinese character
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/ 
void GUI_ShowFont24(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode)
{
	uint8_t i,j,k,tmp;
	uint16_t num;
	num = sizeof(cfont24)/sizeof(typFNT_GB24);
  for(i=0;i<num;i++)
	{
		if((cfont24[i].Index[0]==*s)&&(cfont24[i].Index[1]==*(s+1)))
		{
			for(j=0;j<72;j++)
			{
				if(mode)
				{
					tmp = cfont24[i].Msk[j];
				}
				else
				{
					tmp = ~(cfont24[i].Msk[j]);
				}
				for(k=0;k<8;k++)
				{
					if(tmp&(0x80>>k))
					{
						OLED_Set_Pixel(x+(j%3)*8+k, y+j/3,1);
					}
					else
					{
						OLED_Set_Pixel(x+(j%3)*8+k, y+j/3,0);
					}
				}
			}	
			break;
		}	
	}
}

/*****************************************************************************
 * @name       :void GUI_ShowFont32(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode)
 * @date       :2018-08-27 
 * @function   :Display a single 32x32 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								s:the start address of the Chinese character
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/ 
void GUI_ShowFont32(uint8_t x,uint8_t y,uint8_t *s,uint8_t mode)
{
	uint8_t i,j,k,tmp;
	uint16_t num;
	num = sizeof(cfont32)/sizeof(typFNT_GB32);
  for(i=0;i<num;i++)
	{
		if((cfont32[i].Index[0]==*s)&&(cfont32[i].Index[1]==*(s+1)))
		{
			for(j=0;j<128;j++)
			{
				if(mode)
				{
					tmp = cfont32[i].Msk[j];
				}
				else
				{
					tmp = ~(cfont32[i].Msk[j]);
				}
				for(k=0;k<8;k++)
				{
					if(tmp&(0x80>>k))
					{
						OLED_Set_Pixel(x+(j%4)*8+k, y+j/4,1);
					}
					else
					{
						OLED_Set_Pixel(x+(j%4)*8+k, y+j/4,0);
					}
				}
			}	
			break;
		}	
	}
}

/*****************************************************************************
 * @name       :void GUI_ShowCHinese(uint8_t x,uint8_t y,uint8_t hsize,uint8_t *str,uint8_t mode)
 * @date       :2018-08-27 
 * @function   :Display Chinese strings
 * @parameters :x:the bebinning x coordinate of the Chinese strings
                y:the bebinning y coordinate of the Chinese strings
								size:the size of Chinese strings
								str:the start address of the Chinese strings
								mode:0-white background and black character
								     1-black background and white character
 * @retvalue   :None
******************************************************************************/	   		   
void GUI_ShowCHinese(uint8_t x,uint8_t y,uint8_t hsize,uint8_t *str,uint8_t mode)
{ 
	while(*str!='\0')
	{
//		if(*str>=0x81)//文字为汉字
//		{
			if(hsize == 16)
			{
				GUI_ShowFont16(x,y,str,mode);
			}
			else if(hsize == 24)
			{
				GUI_ShowFont24(x,y,str,mode);
			}
			else if(hsize == 32)
			{
				GUI_ShowFont32(x,y,str,mode);
			}
			else
			{
				return;
			}
			x+=hsize;
			if(x>WIDTH-hsize)
			{
				x=0;
				y+=hsize;
			}
			str+=2;
//		}else //文字为ASCLL字符
//		{
//			GUI_ShowString(x,y,str,hsize,mode);
//			str++;
//		}
	}	
//	OLED_Display();
}

/*
	x:横坐标，0-127
	y:0-64
    *str:要显示的字符串（中英文混合字符串，前提是数组中包含的汉字）
	hsize：8，16，24，32字符大小
	mode：1正显，0反显
*/
void OLED_Print(uint8_t x, uint8_t y, uint8_t hsize,uint8_t *str,uint8_t mode)
{
	uint8_t k,csize;
	uint32_t length;
	length = strlen((char*)str);//取字符串总长
	for(k=0; k<length; k++)
	{
		if(*(str+k) <= 127)//小于128是ASCII符号
		{
			if(hsize == 16)
			{
				csize = hsize/2;
			}
			else if(hsize == 8)
			{
				csize = hsize/2+2;
			}
			else
			{
				return;
			}
			if(*(str+k)!='\0')
			{
				GUI_ShowChar(x,y,*(str+k),hsize,mode);
				x+=csize;
				if(x>120)
				{
					x=0;
					y+=hsize;
					if(y>48)
					{
						y=0;
					}
				}
			}
		}else if(*(str+k) > 127)//大于127，为汉字，前后两个组成汉字内码
		{
			//printf("汉字\r\n");
			if(hsize == 16)
			{
				GUI_ShowFont16(x,y,str+k,mode);
			}
			else if(hsize == 24)
			{
				GUI_ShowFont24(x,y,str+k,mode);
			}
			else if(hsize == 32)
			{
				GUI_ShowFont32(x,y,str+k,mode);
			}
			else
			{
				return;
			}
			x+=hsize/2;
			if(x>WIDTH-hsize)
			{
				x=0;
				y+=hsize;
				if(y>48)
				{
					y=0;
				}
			}
		}
	}
}

/*****************************************************************************
 * @name       :void GUI_DrawBMP(uint8_t x,uint8_t y,uint8_t width, uint8_t height, uint8_t BMP[], uint8_t mode)
 * @date       :2018-08-27 
 * @function   :Display a BMP monochromatic picture
 * @parameters :x:the bebinning x coordinate of the BMP monochromatic picture
                y:the bebinning y coordinate of the BMP monochromatic picture
								width:the width of BMP monochromatic picture
								height:the height of BMP monochromatic picture
								BMP:the start address of BMP monochromatic picture array
								mode:0-white background and black character
								     1-black background and white character								
 * @retvalue   :None
******************************************************************************/ 
void GUI_DrawBMP(uint8_t x,uint8_t y,uint8_t width, uint8_t height, uint8_t BMP[], uint8_t mode)
{ 	
 uint8_t i,j,k;
 uint8_t tmp;
 for(i=0;i<height;i++)
 {
		for(j=0;j<(width+7)/8;j++)
		{
		    if(mode)
				{
					tmp = BMP[i*((width+7)/8)+j];
				}
				else
				{
					tmp = ~BMP[i*((width+7)/8)+j];
				}
				for(k=0;k<8;k++)
				{
					if(tmp&(0x80>>k))
					{
						OLED_Set_Pixel(x+j*8+k, y+i,1);
					}
					else
					{
						OLED_Set_Pixel(x+j*8+k, y+i,0);
					}
				}
		}
 } 
// OLED_Display();
}
