#include <math.h>
#include "menu.h"


/****************************************************************

//	TIM3->CCR4=Tim3CH4pwm;//LED
//	TIM1->CCR4=Tim1CH4pwm;//蜂鸣器
//	
//	V1 = (float)3.3/4096*GetAdcData(1);
//	V2 = 3.3-V1;
//	R2 = 100000.0f*(V1/V2);
//	Temp1 = 1.0f/(1.0f/(273.15f+25.0f)+log(R2/100000.0f)/3950.0f)-273.15f;//热敏电阻温度值
//	
//	sprintf((char*)ch,"%.02fV,%.01f℃",(float)3.32/4095*GetAdcData(2)*2,
//			Temp1);
//	OLED_Print(0,0,16,ch,1);
//	
//	sprintf((char*)ch,"%lld,%d,%d,%d,%d,%d",GetSysRunTime()/1000,Key1,Key2,Key3,Key4,Ecb02cKey);
//	OLED_Print(0,16,16,ch,1);
//	
//	sprintf((char*)ch,"%.f",tim1_CH3_pwm);
//	OLED_Print(0,32,16,ch,1);
//	current_operation_index=table[func_index].current_operation;	//执行当前索引号所对应的功能函数
//	current_operation_index();//执行当前操作函数
//	
//	sprintf((char*)CharStr1," %.02f℃",Temp1);//转换为字符串
//	strcpy((char*)CharStr2,(char*)Mune1.current);//复制字符串
//	strcat((char*)CharStr2,(char*)CharStr1);//合并字符串
//	OLED_Print(0,0,16,CharStr2,1);//LCD显示字符串
//	sprintf((char*)CharStr1,"%.02fV",(float)3.32/4095*GetAdcData(2)*2);//转换为字符串
//	OLED_Print(0,16,16,CharStr1,1);//LCD显示字符串

****************************************************************/
