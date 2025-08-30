#include "py32f0xx_bsp_gpio.h"

void BSP_GPIO_Config()
{   
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA | LL_IOP_GRP1_PERIPH_GPIOB | LL_IOP_GRP1_PERIPH_GPIOF);
        
    //PF4配置为模拟输入
    GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG ;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    
    //用户配置口线
    //输入区
    GPIO_InitStruct.Pin = INT_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(INT_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = KEY_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);
	
	LL_GPIO_ResetOutputPin(GPIOA,LL_GPIO_PIN_12);//12V使能引脚
    GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    LL_GPIO_ResetOutputPin(GPIOA,LL_GPIO_PIN_12);//12V失能
}

/**
  * @brief  ADC calibration function
  * @param  None
  * @retval None
  */
static void APP_AdcCalibrate(void)
{
#if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0; 
#endif 

  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    /* Enable calibration */
    LL_ADC_StartCalibration(ADC1);

#if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif 

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
#if (USE_TIMEOUT == 1)
      /* Check if calibration is timeout */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if(Timeout-- == 0)
        {

        }
      }
#endif 
    }

    /* Delay between ADC calibration end and ADC enable: minimum 4 ADC Clock cycles */
//    LL_mDelay(1);
  }
}

/**
  * @brief  ADC configuration function
  * @param  None
  * @retval None
  */
void BSP_AdcConfig(void)
{
	LL_ADC_Reset(ADC1);
	/* Enable ADC1 clock */
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
	/* ADC calibration */
	APP_AdcCalibrate();
	/* Enable GPIOA clock */
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

	/* Configure pin 4 as analog input */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);

	/* Configure pin 5 as analog input */
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ANALOG);

	/* ADC channel and clock source should be configured when ADEN=0, others should be configured when ADSTART=0 */
	/* Configure internal conversion channel */
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);

	/* Set ADC clock */
	LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV4);

	/* Set 12-bit resolution */
	LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

	/* Set data alignment to right */
	LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

	/* Set low power mode */
	LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_AUTOWAIT);

	/* Set channel conversion time */
	LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_239CYCLES_5);

	/* ADC regular group conversion trigger by software */
	LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

	/* Set DMA mode as disabled */
	LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);

	/* Set overrun management mode to data overwritten */
	LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

	/* Set discontinuous mode to disabled */
	LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

	/* Set ADC conversion mode to single mode: one conversion per trigger */
	LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

}

/**
  * @brief  ADC Convert the selected channel
  * @param  channel: channel is selected to be converted
  * @retval None
  */
uint32_t BSP_AdcConvert(uint32_t channel)
{
  uint16_t AdcConvertValue;

  /* Clear all channels */
  WRITE_REG(ADC1->CHSELR,0);
  
  /* Set channel as conversion channel */
  LL_ADC_REG_SetSequencerChannels(ADC1, channel);
  
  /* Enable ADC */
    LL_ADC_Enable(ADC1);

  /* Start ADC conversion */
  LL_ADC_REG_StartConversion(ADC1);
  
  /* Wait ADC conversion complete */
  while(LL_ADC_IsActiveFlag_EOC(ADC1)==0);
  
  /* Clear eoc flag */
  LL_ADC_ClearFlag_EOC(ADC1);
  
  /* Get conversion value */
  AdcConvertValue = LL_ADC_REG_ReadConversionData12(ADC1);
  
  /* Clear all channels */
  WRITE_REG(ADC1->CHSELR,0);
  
  return (uint32_t)AdcConvertValue; 
}
