/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


#if defined(USE_FULL_ASSERT)
#include "py32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
#include "defines.h"
#include "coroOS.h"
#include "bsp_py32f030x6.h"
/* Private defines -----------------------------------------------------------*/
/* Exported variables prototypes ---------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

typedef struct pid_parameter
{
	float Target;		//Ŀ��ֵ
	float Current;		//ʵ��ֵ(����ֵ)
	float Error;		//ƫ��ֵ(��ǰƫ��)
	float Last_error;	//�ϴ�ƫ��ֵ
	float Integral;		//����ֵ
	float Kp;		//����,����,΢��ϵ��
	float Ki;
	float Kd;
	float Out;			//���ֵ
}pid;

void APP_ErrorHandler(void);

void Menu_AppInit(void);//��ʼ��
void MenuHmiTask(void);//��������
void MenuKeyData(void);//�˵���������
void DataGather(void);//���ݲɼ�����
void MenuOledprintf(void);//�˵�OLED��ʾ
void Mcu1DataUP(void);//�����ϴ�����
//�˵�UI
void Home1(void);//������1,0��ң��������
void Home2(void);//������2��1��̽�յ�����
void MCU1LEDSet(void);//�����˵���7��LED��������
void ElectricArc(void);////�����˵���15���绡����
//void MCU1LP2221Set(void);//�����˵���8��LP2221�������ã�5VUSB��ѹ���
//void Mcu1BuzzSet(void);//�����˵���9����������������
//void MCU1LEDMAXSet(void);//�����˵���10��LED�����������
//void Mcu1BuzzSetOFF(void);//�����˵���9����������������
//void Mcu1SaveSet(void);//�����˵���13����������
//�˵��ظ����õ�UI
void Home1_2(void);//�����˵���ʾ����
void Ds3231SetHome(void);//�����˵���8~15��ʱ������
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
