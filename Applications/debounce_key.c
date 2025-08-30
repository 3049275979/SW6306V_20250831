#include "debounce_key.h"

/*******************************用户按键配置区*********************************/

// 按键数组
static Key keys[KEY_COUNT];

// 初始化按键
void Key_Init(void) {
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        keys[i].state = KEY_STATE_RELEASED;
        keys[i].pressCount = 0;
        keys[i].shortPressed = 0;
        keys[i].longPressed = 0;
        keys[i].longReleased = 0;
    }
    // 设置实际引脚（根据硬件修改）
    keys[0].pin = KEY1_PIN;
    keys[1].pin = KEY2_PIN;
    keys[2].pin = KEY3_PIN;
    keys[3].pin = KEY4_PIN;
}
// 读取按键状态（需要根据实际硬件实现）
uint8_t Key_Read(uint8_t pin) {
    // 这里需要替换为实际的引脚读取代码
    // 返回1表示按下，0表示释放
    // 例如：return HAL_GPIO_ReadPin(pin) == GPIO_PIN_RESET;
//	if(KEY1_PIN)
//	{
//		if(LL_GPIO_IsInputPinSet(GPIOB,pin)==0)
//		{
//			return 1;
//		}
//	}
//	if(KEY2_PIN)
//	{
//		if(LL_GPIO_IsInputPinSet(GPIOB,pin)==0)
//		{
//			return 1;
//		}
//	}
//	if(KEY3_PIN)
//	{
//		if(LL_GPIO_IsInputPinSet(GPIOB,pin)==0)
//		{
//			return 1;
//		}
//	}
//	if(KEY4_PIN)
//	{
//		if(LL_GPIO_IsInputPinSet(GPIOB,pin)==0)
//		{
//			return 1;
//		}
//	}
	return LL_GPIO_IsInputPinSet(GPIOB,pin)==0;
}
// 按键扫描函数，每10ms调用一次
void Key_Scan(void) {
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        uint8_t keyPressed = Key_Read(keys[i].pin);
        
        switch (keys[i].state) {
            case KEY_STATE_RELEASED:
                if (keyPressed) {
                    // 按键被按下，进入按下状态
                    keys[i].pressCount = 0;
					keys[i].longPressed = 0;
                    keys[i].state = KEY_STATE_PRESSED;
                }
                break;
                
            case KEY_STATE_PRESSED:
                if (!keyPressed) {
                    // 按键释放，触发短按
                    keys[i].shortPressed = 1;
                    keys[i].state = KEY_STATE_RELEASED;
                } else {
                    // 增加按下计数
                    keys[i].pressCount++;
                    
                    if (keys[i].pressCount >= LONG_PRESS_THRESHOLD) {
                        // 达到长按时间阈值，触发长按
                        keys[i].longPressed = 1;
                        keys[i].state = KEY_STATE_LONG_PRESSED;
                    }
                }
                break;
                
            case KEY_STATE_LONG_PRESSED:
                if (!keyPressed) {
                    // 长按后松开，触发长按松开事件
                    keys[i].longReleased = 1;
					 keys[i].longPressed = 0;
                    keys[i].state = KEY_STATE_RELEASED;
                }
                break;
        }
    }
}
// 检查是否有短按事件发生
uint8_t Key_IsShortPressed(uint8_t keyIndex) {
    if (keyIndex >= KEY_COUNT) return 0;
    
    if (keys[keyIndex].shortPressed) {
        keys[keyIndex].shortPressed = 0; // 清除标志
        return 1;
    }
    return 0;
}

// 检查是否有长按事件发生
uint8_t Key_IsLongPressed(uint8_t keyIndex) {
    if (keyIndex >= KEY_COUNT) return 0;
    
    if (keys[keyIndex].longPressed) {
//        keys[keyIndex].longPressed = 0; // 清除标志
        return 1;
    }
    return 0;
}

// 检查是否有长按松开事件发生
uint8_t Key_IsLongReleased(uint8_t keyIndex) {
    if (keyIndex >= KEY_COUNT) return 0;
    
    if (keys[keyIndex].longReleased) {
        keys[keyIndex].longReleased = 0; // 清除标志
        return 1;
    }
    return 0;
}
// 检查按键事件并返回事件类型
uint8_t Check_Key_Events(void)
{
	// 检查按键事件
	if (Key_IsShortPressed(0)) {// 处理短按事件
			
		return 1;
	}
	if (Key_IsLongPressed(0)) {// 处理长按事件
			
		return 5;
	}
	if (Key_IsLongReleased(0))//长按松开
	{
		return 9;
	}
	if (Key_IsShortPressed(1)) {// 处理短按事件
			
		return 2;
	}
	if (Key_IsLongPressed(1)) {// 处理长按事件
			
		return 6;
	}
	if (Key_IsLongReleased(1))//长按松开
	{
		return 10;
	}
	if (Key_IsShortPressed(2)) {// 处理短按事件
			
		return 3;
	}
	if (Key_IsLongPressed(2)) {// 处理长按事件
			
		return 7;
	}
	if (Key_IsLongReleased(2))//长按松开
	{
		return 11;
	}
	if (Key_IsShortPressed(3)) {// 处理短按事件
			
		return 4;
	}
	if (Key_IsLongPressed(3)) {// 处理长按事件
			
		return 8;
	}
	if (Key_IsLongReleased(3))//长按松开
	{
		return 12;
	}
	return 0;
}
/*******************************用户配置区结束*********************************/

/*用例：
*
*/