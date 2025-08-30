#ifndef __DS3231_DRV_H
#define	__DS3231_DRV_H
 
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"/*包含自己的I2C驱动库*/
 
 #define DS3231_ADDRESS	      0x68 //I2C Slave address
#define	DS3231_ADDRESS_Write	0xD0
#define	DS3231_ADDRESS_Read		0xD1

/* DS3231 Registers. Refer Sec 8.2 of application manual */
#define DS3231_SEC_REG        0x00    // 秒
#define DS3231_MIN_REG        0x01    //
#define DS3231_HOUR_REG       0x02
#define DS3231_WDAY_REG       0x03
#define DS3231_MDAY_REG       0x04
#define DS3231_MONTH_REG      0x05
#define DS3231_YEAR_REG       0x06

#define DS3231_AL1SEC_REG     0x07
#define DS3231_AL1MIN_REG     0x08
#define DS3231_AL1HOUR_REG    0x09
#define DS3231_AL1WDAY_REG    0x0A

#define DS3231_AL2MIN_REG     0x0B
#define DS3231_AL2HOUR_REG    0x0C
#define DS3231_AL2WDAY_REG    0x0D

#define DS3231_CONTROL_REG          0x0E//控制寄存器
#define DS3231_STATUS_REG           0x0F//状态寄存器
#define DS3231_AGING_OFFSET_REG     0x10
#define DS3231_TMP_UP_REG           0x11
#define DS3231_TMP_LOW_REG          0x12

#define EverySecond     0x01
#define EveryMinute     0x02
#define EveryHour       0x03


typedef struct DateTImeStruct{
	uint8_t second;			//秒
	uint8_t minute;			//分
	uint8_t hour;				//时
	uint8_t dayofmonth;		//日
	uint8_t month;				//月
	uint16_t year;			//年
	uint8_t dayOfWeek;  /*Su=1 Mo=2 Tu=3 We=4 Th=5 Fr=6 Sa=7 */
}DateTime;

//void DS3231_Init(void);

uint8_t IIC_DS3231_ByteWrite(uint8_t WriteAddr , uint8_t date);//写
uint8_t IIC_DS3231_ByteRead(uint8_t ReadAddr,uint8_t* Receive);//读
uint8_t bcdToDec(uint8_t byte);
uint8_t DS3231_setDate(uint8_t year,uint8_t mon,uint8_t day,uint8_t way);
uint8_t DS3231_setTime(uint8_t hour , uint8_t min , uint8_t sec);
uint8_t DS3231_getdate(DateTime* ans);
uint8_t DS3231_gettime(DateTime* ans);
void DS3231_alarm1ksetTime(uint8_t hour , uint8_t min);//设置DS3231闹钟2时分
void DS3231_alarm2ksetTime(uint8_t hour , uint8_t min);//设置DS3231闹钟2时分
 
#endif 
 
 
 
