#include <math.h>
#include "menu.h"


/****************************************************************

//	TIM3->CCR4=Tim3CH4pwm;//LED
//	TIM1->CCR4=Tim1CH4pwm;//������
//	
//	V1 = (float)3.3/4096*GetAdcData(1);
//	V2 = 3.3-V1;
//	R2 = 100000.0f*(V1/V2);
//	Temp1 = 1.0f/(1.0f/(273.15f+25.0f)+log(R2/100000.0f)/3950.0f)-273.15f;//���������¶�ֵ
//	
//	sprintf((char*)ch,"%.02fV,%.01f��",(float)3.32/4095*GetAdcData(2)*2,
//			Temp1);
//	OLED_Print(0,0,16,ch,1);
//	
//	sprintf((char*)ch,"%lld,%d,%d,%d,%d,%d",GetSysRunTime()/1000,Key1,Key2,Key3,Key4,Ecb02cKey);
//	OLED_Print(0,16,16,ch,1);
//	
//	sprintf((char*)ch,"%.f",tim1_CH3_pwm);
//	OLED_Print(0,32,16,ch,1);
//	current_operation_index=table[func_index].current_operation;	//ִ�е�ǰ����������Ӧ�Ĺ��ܺ���
//	current_operation_index();//ִ�е�ǰ��������
//	
//	sprintf((char*)CharStr1," %.02f��",Temp1);//ת��Ϊ�ַ���
//	strcpy((char*)CharStr2,(char*)Mune1.current);//�����ַ���
//	strcat((char*)CharStr2,(char*)CharStr1);//�ϲ��ַ���
//	OLED_Print(0,0,16,CharStr2,1);//LCD��ʾ�ַ���
//	sprintf((char*)CharStr1,"%.02fV",(float)3.32/4095*GetAdcData(2)*2);//ת��Ϊ�ַ���
//	OLED_Print(0,16,16,CharStr1,1);//LCD��ʾ�ַ���

****************************************************************/
