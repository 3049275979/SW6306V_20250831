#include <stdint.h>
#include "main.h"/*包含自己的I2C驱动库*/
//ASYNC_I2C_Transmit(addr,reg,pdata,len,pflag);
//ASYNC_I2C_Receive(addr,reg,pdata,len,pflag);
static float Temperature=0;
static float Humidity=0;

void Shtc3_Read_Result(void)
{
	uint16_t tem,hum;
	uint8_t buff[6];
	uint8_t reg[2];
	uint8_t pflag;
	static uint8_t state=0;
	static uint8_t shtc3time=0;
	switch(state)
	{
		case 0://开始转换
			reg[0]=0x5C;
			reg[1]=0x24;
			APP_I2C_Transmit(0xE0,0x00,reg,2);
//			/*shtc3测量*/
//			IIC_Start();
//			IIC_Send_Byte(0xE0);
//			IIC_Wait_Ack();
//			IIC_Send_Byte(0x5C);
//			IIC_Wait_Ack();
//			IIC_Send_Byte(0x24);
//			IIC_Wait_Ack();
//			IIC_Stop();
			state=2;
			break;
		case 1://等待转换延时时间
			shtc3time++;
			if(shtc3time>=5)
			{
				state=2;
				shtc3time=0;
			}
			break;
		case 2://读取数据
			APP_I2C_Receive(0xE1,0x00,buff,6);
			/*shtc3读取*/
//			IIC_Start();
//			IIC_Send_Byte(0xE1);
//			IIC_Wait_Ack();
//			buff[0]=IIC_Read_Byte(1);
//			buff[1]=IIC_Read_Byte(1);
//			buff[2]=IIC_Read_Byte(1);
//			buff[3]=IIC_Read_Byte(1);
//			buff[4]=IIC_Read_Byte(1);
//			buff[5]=IIC_Read_Byte(1);
//			IIC_Stop();
			hum = ((buff[0]<<8) | buff[1]);//湿度拼接
			tem = ((buff[3]<<8) | buff[4]);//温度拼接
			/*转换实际温度*/
			Temperature= (175.0f*(float)tem/65535.0f-45.0f) ;// T = -45 + 175 * tem / (2^16-1)
			Humidity= (100.0f*(float)hum/65535.0f);// RH = hum*100 / (2^16-1)
			hum=0;
			tem=0;
			state=0;
			break;
		default:
			break;
	}
}
//SHTC3唤醒
void Shtc3Wakeup(void)
{
	uint8_t reg[2];
	uint8_t pflag;
	reg[0]=0x35;
	reg[1]=0x17;
	APP_I2C_Transmit(0xE0,0x00,reg,2);
	/*shtc3唤醒*/
//	IIC_Start();
//	IIC_Send_Byte(0xE0);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x35);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x17);
//	IIC_Wait_Ack();
//	IIC_Stop();
}

//SHTC3休眠
void Shtc3Hibernate(void)
{
	uint8_t reg[2];
	uint8_t pflag;
	reg[0]=0xB0;
	reg[1]=0x98;
	APP_I2C_Transmit(0xE0,0x00,reg,2);
	/*进入睡眠模式*/
//	IIC_Start();
//	IIC_Send_Byte(0xE0);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0xB0);
//	IIC_Wait_Ack();
//	IIC_Send_Byte(0x98);
//	IIC_Wait_Ack();
//	IIC_Stop();
}

float GetShtc3Data(uint8_t data)
{
	switch(data)
	{
		case 0:
			return Temperature;
		case 1:
			return Humidity;
		default:
			break;
	}
	return 0;
}
