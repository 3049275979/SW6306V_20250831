/*
按键支持库文件
提供轻量级非阻塞式按键消抖、边沿检测与多平台兼容性

技术来源：https://blog.csdn.net/hxj_csdn/article/details/104318108
*/

/*需要在define.h加入的宏定义：（未定义将采用默认设置）
DEBOUNCE_TIME_10MS
*/

#ifndef __DEBOUNCE_KEY_H__
#define __DEBOUNCE_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif
/*****************************用户配置区开始***********************************/
    #include "bsp_py32f030x6.h"//包含按键状态读取函数的库
/*******************************用户配置区结束*********************************/
// 按键定义
#define KEY_COUNT 4
#define KEY1_PIN  LL_GPIO_PIN_0// 替换为实际引脚定义
#define KEY2_PIN  LL_GPIO_PIN_1// 替换为实际引脚定义
#define KEY3_PIN  LL_GPIO_PIN_2// 替换为实际引脚定义
#define KEY4_PIN  LL_GPIO_PIN_3// 替换为实际引脚定义

// 长按时间阈值（10ms为单位，100=100 * 10ms=1s）
#define LONG_PRESS_THRESHOLD 100

// 按键状态枚举
typedef enum {
    KEY_STATE_RELEASED,    // 按键释放
    KEY_STATE_PRESSED,     // 按键按下（未达到长按时间）
    KEY_STATE_LONG_PRESSED // 按键长按
} KeyState;

// 按键数据结构
typedef struct {
    uint8_t pin;           // 按键对应的引脚
    KeyState state;        // 当前状态
    uint16_t pressCount;   // 按下计数（每10ms增加1）
    uint8_t shortPressed;  // 短按标志
    uint8_t longPressed;   // 长按标志
    uint8_t longReleased;  // 长按后松开标志
} Key;

void Key_Init(void);// 初始化按键
uint8_t Key_Read(uint8_t pin);// 读取按键状态（需要根据实际硬件实现）
void Key_Scan(void);// 按键扫描函数，每10ms调用一次
uint8_t Key_IsShortPressed(uint8_t keyIndex);// 检查是否有短按事件发生
uint8_t Key_IsLongPressed(uint8_t keyIndex);// 检查是否有长按事件发生
uint8_t Key_IsLongReleased(uint8_t keyIndex);// 检查是否有长按松开事件发生
uint8_t Check_Key_Events(void);// 检查按键事件并返回事件类型
/*******************************用户配置区结束*********************************/

#ifdef __cplusplus
extern "C" {
#endif
#endif