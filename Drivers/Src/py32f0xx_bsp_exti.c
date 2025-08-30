#include "py32f0xx_bsp_exti.h"

void BSP_EXTI_Config()
{
    LL_EXTI_InitTypeDef EXTI_InitStruct;
    
    EXTI_InitStruct.Line = INT_EXTI;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);
    LL_EXTI_SetEXTISource(INT_EXTIPORT,INT_EXTIPIN);
    
    NVIC_SetPriority(INT_IRQn, 1);
    NVIC_EnableIRQ(INT_IRQn);
	
	EXTI_InitStruct.Line = LL_EXTI_LINE_0;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);
    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB,LL_EXTI_CONFIG_LINE0);
    
    NVIC_SetPriority(EXTI0_1_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
	
	EXTI_InitStruct.Line = LL_EXTI_LINE_1;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);
    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB,LL_EXTI_CONFIG_LINE1);
    
    NVIC_SetPriority(EXTI0_1_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_1_IRQn);
	
	EXTI_InitStruct.Line = LL_EXTI_LINE_2;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);
    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB,LL_EXTI_CONFIG_LINE2);
    
    NVIC_SetPriority(EXTI2_3_IRQn, 1);
    NVIC_EnableIRQ(EXTI2_3_IRQn);
    
    EXTI_InitStruct.Line = KEY_EXTI;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);
    LL_EXTI_SetEXTISource(KEY_EXTIPORT,KEY_EXTIPIN);
    
    NVIC_SetPriority(KEY_IRQn, 1);
    NVIC_EnableIRQ(KEY_IRQn);
 /**
   * Enable interrupt:
   * - EXTI0_1_IRQn for PA/PB/PC[0,1]
   * - EXTI2_3_IRQn for PA/PB/PC[2,3]
   * - EXTI4_15_IRQn for PA/PB/PC[4,15]
  */
}

extern uint16_t cd_sleep;
uint8_t inttrig,keytrig;
void EXTI4_15_IRQHandler(void)
{
    if(LL_EXTI_IsActiveFlag(INT_EXTI))
    {
        LL_EXTI_ClearFlag(INT_EXTI);
        cd_sleep = SLEEP_DELAY;//刷新睡眠倒计时
        inttrig = 1;
    }
}

void EXTI0_1_IRQHandler(void)
{
    if(LL_EXTI_IsActiveFlag(LL_EXTI_LINE_0)||LL_EXTI_IsActiveFlag(LL_EXTI_LINE_1))
    {
        LL_EXTI_ClearFlag(LL_EXTI_LINE_0);
		LL_EXTI_ClearFlag(LL_EXTI_LINE_1);
        cd_sleep = SLEEP_DELAY;//刷新睡眠倒计时
        keytrig  = 1;
    }
}

void EXTI2_3_IRQHandler(void)
{
    if(LL_EXTI_IsActiveFlag(KEY_EXTI)||LL_EXTI_IsActiveFlag(LL_EXTI_LINE_2))
    {
        LL_EXTI_ClearFlag(KEY_EXTI);
		LL_EXTI_ClearFlag(LL_EXTI_LINE_2);
        cd_sleep = SLEEP_DELAY;//刷新睡眠倒计时
        keytrig  = 1;
    }
}