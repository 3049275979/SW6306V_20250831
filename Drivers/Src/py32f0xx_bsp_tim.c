#include "py32f0xx_bsp_tim.h"

void BSP_TIM_config(void)
{
    LL_TIM_InitTypeDef TIMCountInit = {0};

	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM16);
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);
	
	TIMCountInit.ClockDivision       = LL_TIM_CLOCKDIVISION_DIV1;
    TIMCountInit.CounterMode         = LL_TIM_COUNTERMODE_UP;
    TIMCountInit.Prescaler           = 0;
    /* PWM period = 1000 */
    TIMCountInit.Autoreload          = 1000-1;
    TIMCountInit.RepetitionCounter   = 0;
    LL_TIM_Init(TIM16,&TIMCountInit);
	LL_TIM_EnableAllOutputs(TIM16);
    LL_TIM_EnableCounter(TIM16);
	
	LL_TIM_Init(TIM1,&TIMCountInit);
	LL_TIM_EnableAllOutputs(TIM1);
    LL_TIM_EnableCounter(TIM1);
}
    
void BSP_PWMChannelConfig(void)
{
	LL_GPIO_InitTypeDef GPIO_InitTypeDef;
	LL_TIM_OC_InitTypeDef TIM_OC_Initstruct;

	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_8;
	GPIO_InitTypeDef.Alternate = LL_GPIO_AF2_TIM16;
	LL_GPIO_Init(GPIOB, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_8;
	GPIO_InitTypeDef.Alternate = LL_GPIO_AF2_TIM1;
	LL_GPIO_Init(GPIOA, &GPIO_InitTypeDef);
	
	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_11;
	GPIO_InitTypeDef.Alternate = LL_GPIO_AF2_TIM1;
	LL_GPIO_Init(GPIOA, &GPIO_InitTypeDef);

	GPIO_InitTypeDef.Pin = LL_GPIO_PIN_13;
	GPIO_InitTypeDef.Alternate = LL_GPIO_AF13_TIM1;
	LL_GPIO_Init(GPIOA, &GPIO_InitTypeDef);

	TIM_OC_Initstruct.OCMode = LL_TIM_OCMODE_PWM1;
	TIM_OC_Initstruct.OCState = LL_TIM_OCSTATE_ENABLE;
	TIM_OC_Initstruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	TIM_OC_Initstruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
	/* Set channel compare values */
	TIM_OC_Initstruct.CompareValue = 100;//LCD背光亮度
	LL_TIM_OC_Init(TIM16, LL_TIM_CHANNEL_CH1, &TIM_OC_Initstruct);

	TIM_OC_Initstruct.CompareValue = 0;//电弧
	LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH1, &TIM_OC_Initstruct);
	
	TIM_OC_Initstruct.CompareValue = 0;//风扇转速
	LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH2, &TIM_OC_Initstruct);
	
	TIM_OC_Initstruct.OCMode = LL_TIM_OCMODE_PWM1;
	TIM_OC_Initstruct.OCState = LL_TIM_OCSTATE_ENABLE;
	TIM_OC_Initstruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	TIM_OC_Initstruct.OCIdleState = LL_TIM_OCIDLESTATE_HIGH;
	TIM_OC_Initstruct.CompareValue = 0;//LED亮度
	LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH4, &TIM_OC_Initstruct);
}
