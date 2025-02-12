/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */

/* Measured VREF */
static int32_t vref_mv = VDD_VALUE;

/* USER CODE END 0 */

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  LL_ADC_InitTypeDef ADC_InitStruct = {0};
  LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
  LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  /**ADC1 GPIO Configuration
  PC0   ------> ADC1_IN10
  */
  GPIO_InitStruct.Pin = ACC_ADC_IN_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
  LL_GPIO_Init(ACC_ADC_IN_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
  ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
  LL_ADC_Init(ADC1, &ADC_InitStruct);
  ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
  LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
  ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
  ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
  ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
  ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
  ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
  LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);

  /** Configure Regular Channel
  */
  LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_10);
  LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_10, LL_ADC_SAMPLINGTIME_239CYCLES_5);
  /* USER CODE BEGIN ADC1_Init 2 */
  LL_ADC_Enable(ADC1);
  HAL_Delay(2);
  LL_ADC_StartCalibration(ADC1);
  while(LL_ADC_IsCalibrationOnGoing(ADC1));

  /* Measure reference voltage at start */
	if(adc_measure(ADC_VREF_MV, &vref_mv) != 0){
		while(1);
	}
  /* USER CODE END ADC1_Init 2 */

}

/* USER CODE BEGIN 1 */
static struct {
    uint32_t path, channel, sample_time, stabilization_us;
} adc_channel[] = {
        {LL_ADC_PATH_INTERNAL_VREFINT, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_239CYCLES_5, 0},
        {LL_ADC_PATH_INTERNAL_TEMPSENSOR, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_239CYCLES_5, LL_ADC_DELAY_TEMPSENSOR_STAB_US},
        {LL_ADC_PATH_INTERNAL_NONE, LL_ADC_CHANNEL_10, LL_ADC_SAMPLINGTIME_239CYCLES_5, 0},
};

static void delay_us(uint32_t microseconds){
	microseconds *= 100;

	while(microseconds--){
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}
}

int32_t adc_get_measured_vref(void){
	return vref_mv;
}

int32_t adc_measure(adc_measure_parameter parameter, int32_t *value){
    int32_t val;

    /* Set channel */
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), adc_channel[parameter].path);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, adc_channel[parameter].channel);
    LL_ADC_SetChannelSamplingTime(ADC1, adc_channel[parameter].channel, adc_channel[parameter].sample_time);
    delay_us(adc_channel[parameter].stabilization_us);

    /* Start ADC conversion */
    LL_ADC_Enable(ADC1);

    /* Wait some extra time for internal channels buffers stabilisation */
    if(parameter == ADC_TEMPERATURE_C){
        delay_us(LL_ADC_DELAY_TEMPSENSOR_STAB_US);
    }

    /* Measure parameter */
    LL_ADC_ClearFlag_EOS(ADC1);
    LL_ADC_REG_StartConversionSWStart(ADC1);
    while (LL_ADC_IsActiveFlag_EOS(ADC1) == 0){}

    /* Grab result & disable ADC */
    val = (int32_t)LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_Disable(ADC1);

    /* Disable all path */
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE);

    /* Convert measured parameter */
    if(parameter == ADC_VREF_MV){
    	//reading Vdd by utilising the internal 1.21V VREF
    	*value = (int32_t)(1210 * 4096)/val;
    }
    else if(parameter == ADC_TEMPERATURE_C){
        *value = (int32_t)__LL_ADC_CALC_TEMPERATURE_TYP_PARAMS(4300, 1430, 25, vref_mv, val, LL_ADC_RESOLUTION_12B);
    }
    else if(parameter == ADC_VEHICLE_VOLTAGE){
        val = (val * vref_mv)/4096;
        *value = (val * (10000 + 2200))/2200;
    }
    else{
        *value = val;
    }

    return 0;
}
/* USER CODE END 1 */
