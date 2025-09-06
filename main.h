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
typedef union {
    float f;
    uint32_t u;
} FloatBits;//联合体
typedef struct pid_parameter
{
	float Target;		//目标值
	float Current;		//实际值(反馈值)
	float Error;		//偏差值(当前偏差)
	float Last_error;	//上次偏差值
	float Integral;		//积分值
	float Kp;		//比例,积分,微分系数
	float Ki;
	float Kd;
	float Out;			//输出值
}pid;
typedef struct
{
	int16_t Tim16CH1pwm;//遥控器LCD背光
	float Temp1Target;	//温控设定值
	uint8_t Sw6306InputPower;//最大输入功率设置/充电功率
	float INTIbusRatio;	//IBUS输入电流系数
}Mcu1SaveData;
typedef struct
{
	float FanPwm;		//风扇
	float LedPwm;		//LED
	float Temp1;		//LED温度
	float Temp2;		//单片机箱温度
	float Current;		//LED电流
	float Temp1Target;	//LED温度设定值
	float TempLOW;		//LED温度误差值
	float BatteryVol;	//电池电压
}Mcu2SaveData;

void APP_ErrorHandler(void);

void Menu_AppInit(void);//初始化
void MenuHmiTask(void);//测试任务
void MenuKeyData(void);//菜单按键处理
void DataGather(void);//数据采集任务
void MenuOledprintf(void);//菜单OLED显示
void Mcu1DataUP(void);//数据上传任务
//菜单UI
void Home1(void);//主界面1,0，遥控器数据
void Home2(void);//主界面2，1，探照灯数据
void MCU1LEDSet(void);//三级菜单，7，LED亮度设置
void ElectricArc(void);////三级菜单，15，PA8PWM
void SetTempControl(void);//三级菜单，16，温控设定值
void Mcu1SaveSet(void);//三级菜单，13，保存设置
void MCU1LCDbacSet(void);//三级菜单，20，LCD背光亮度设置
void MCU1Sw6306InputPowerSet(void);//三级菜单，22，充电功率设置
void MCU1INTIbusRatioSet(void);//三级菜单，24，IBUS输入电流系数
//void MCU1LP2221Set(void);//三级菜单，8，LP2221开关设置，5VUSB电压输出
//void Mcu1BuzzSet(void);//三级菜单，9，蜂鸣器音量设置
//void MCU1LEDMAXSet(void);//三级菜单，10，LED最大亮度设置
//void Mcu1BuzzSetOFF(void);//三级菜单，9，蜂鸣器开关设置
//void Mcu1SaveSet(void);//三级菜单，13，保存设置
//菜单重复调用的UI
void Home1_2(void);//二级菜单显示内容
void Ds3231SetHome(void);//三级菜单，8~15，时间设置
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
