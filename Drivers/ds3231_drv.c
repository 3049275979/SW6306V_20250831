#include "ds3231_drv.h" 
 
 //写
uint8_t IIC_DS3231_ByteWrite(uint8_t WriteAddr , uint8_t date)
{
	uint8_t buf[2]={0};
	buf[0] = WriteAddr;
	buf[1] = date;
	APP_I2C_Transmit(DS3231_ADDRESS_Write,0x00,buf,2);
	return 0;
}
//读
uint8_t IIC_DS3231_ByteRead(uint8_t ReadAddr,uint8_t* Receive)
{
	APP_I2C_Transmit(DS3231_ADDRESS_Write,0x00,&ReadAddr,1);
	APP_I2C_Receive(DS3231_ADDRESS_Read,0x00,Receive,1);
	return 0;
}
//设置DS3231年月日星期
uint8_t DS3231_setDate(uint8_t year,uint8_t mon,uint8_t day,uint8_t way)
{
	uint8_t temp_H , temp_L;
	temp_L = year%10;
	temp_H = year/10;
	year = (temp_H << 4) + temp_L;
	if(IIC_DS3231_ByteWrite(DS3231_YEAR_REG,year)) //set year
	{
			printf("set year error\r\n");
			return 1;
	}	
	temp_L = mon%10;
	temp_H = mon/10;
	mon = (temp_H << 4) + temp_L;	
	if(IIC_DS3231_ByteWrite(DS3231_MONTH_REG,mon)) //set mon
	{
		printf("set month error\r\n");
		return 2;
	}
	temp_L = day%10;
	temp_H = day/10;
	day = (temp_H << 4) + temp_L;		
	if(IIC_DS3231_ByteWrite(DS3231_MDAY_REG,day)) //set day
	{
		printf("set day error\r\n");
		return 3;
	}
	temp_L = way%10;
	temp_H = way/10;
	day = (temp_H << 4) + temp_L;		
	if(IIC_DS3231_ByteWrite(DS3231_WDAY_REG,day)) //set way
	{
		printf("set day error\r\n");
		return 4;
	}
	return 0;
}
//设置DS3231时分秒
uint8_t DS3231_setTime(uint8_t hour , uint8_t min , uint8_t sec)
{
	uint8_t temp_H , temp_L;
	temp_L = hour%10;
	temp_H = hour/10;
	hour = (temp_H << 4) + temp_L;
	if(IIC_DS3231_ByteWrite(DS3231_HOUR_REG,hour)) //set hour
		return 1;
	temp_L = min%10;
	temp_H = min/10;
	min = (temp_H << 4) + temp_L;
	if(IIC_DS3231_ByteWrite(DS3231_MIN_REG,min)) //SET min
		return 2;	
	temp_L = sec%10;
	temp_H = sec/10;
	sec = (temp_H << 4) + temp_L;	
	if(IIC_DS3231_ByteWrite(DS3231_SEC_REG,sec))		//SET sec
		return 3;
	return 0;
}

uint8_t bcdToDec(uint8_t byte)
{
	unsigned char temp_H , temp_L;
	temp_L = byte & 0x0f;
	temp_H = (byte & 0xf0) >> 4;
	return ( temp_H * 10 )+ temp_L;
}
//读取DS3231时分秒星期
uint8_t DS3231_gettime(DateTime* ans)
{
	uint8_t receive = 0;
	if(IIC_DS3231_ByteRead(DS3231_HOUR_REG,&receive))
		return 1;
	ans->hour = bcdToDec(receive);
	if(IIC_DS3231_ByteRead(DS3231_MIN_REG,&receive))
		return 2;
	ans->minute = bcdToDec(receive);
	if(IIC_DS3231_ByteRead(DS3231_SEC_REG,&receive))
		return 3;
	ans->second = bcdToDec(receive);
	if(IIC_DS3231_ByteRead(DS3231_WDAY_REG,&receive))
		return 4;
	ans->dayOfWeek = bcdToDec(receive);
	return 0;
}
//读取DS3231年月日
uint8_t DS3231_getdate(DateTime* ans)
{
	unsigned char receive = 0;
	if(IIC_DS3231_ByteRead(DS3231_YEAR_REG,&receive))
		return 1;
	ans->year = bcdToDec(receive) + 2000;
	if(IIC_DS3231_ByteRead(DS3231_MONTH_REG,&receive))
		return 2;
	ans->month = bcdToDec(receive);
	if(IIC_DS3231_ByteRead(DS3231_MDAY_REG,&receive))
		return 3;
	ans->dayofmonth = bcdToDec(receive);
	return 0;
}
//设置DS3231闹钟1时分
void DS3231_alarm1ksetTime(uint8_t hour , uint8_t min)
{
	uint8_t temp_H = 0, temp_L = 0;
	temp_H |= 1<<7;		//开启小时、分钟和秒匹配时报警
	IIC_DS3231_ByteWrite(DS3231_AL1WDAY_REG,temp_H); //set day
	IIC_DS3231_ByteWrite(DS3231_STATUS_REG,temp_L); ////状态寄存器清零
	temp_L |= 1<<0;//开启闹钟1中断
	temp_L |= 1<<2;//开启中断
	IIC_DS3231_ByteWrite(DS3231_CONTROL_REG,temp_L); //控制寄存器开启闹钟2中断
	temp_L = hour%10;
	temp_H = hour/10;
	hour = (temp_H << 4) + temp_L;
	IIC_DS3231_ByteWrite(DS3231_AL1HOUR_REG,hour); //set hour
	temp_L = min%10;
	temp_H = min/10;
	min = (temp_H << 4) + temp_L;
	IIC_DS3231_ByteWrite(DS3231_AL1MIN_REG,min); //SET min	
}
//设置DS3231闹钟2时分
void DS3231_alarm2ksetTime(uint8_t hour , uint8_t min)
{
	uint8_t temp_H = 0, temp_L = 0;
	temp_H |= 1<<7;		//开启小时、分钟和秒匹配时报警
	IIC_DS3231_ByteWrite(DS3231_AL2WDAY_REG,temp_H); //set day
	IIC_DS3231_ByteWrite(DS3231_STATUS_REG,temp_L); ////状态寄存器清零
	temp_L |= 1<<1;//开启闹钟2中断
	temp_L |= 1<<2;//开启中断
	IIC_DS3231_ByteWrite(DS3231_CONTROL_REG,temp_L); //控制寄存器开启闹钟2中断
	temp_L = hour%10;
	temp_H = hour/10;
	hour = (temp_H << 4) + temp_L;
	IIC_DS3231_ByteWrite(DS3231_AL2HOUR_REG,hour); //set hour
	temp_L = min%10;
	temp_H = min/10;
	min = (temp_H << 4) + temp_L;
	IIC_DS3231_ByteWrite(DS3231_AL2MIN_REG,min); //SET min	
}

