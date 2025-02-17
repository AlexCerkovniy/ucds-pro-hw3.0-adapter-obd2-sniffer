/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "console.h"
#include "obd2.h"
#include "fast_fifo.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define GET_SIZE(arr) 			(sizeof(arr) / sizeof((arr)[0]))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Can_LedBlinkOnPacketReceived(void);
void Error_LedShortBlink(void);
void SysTick_Interrupt(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ACC_ADC_IN_Pin LL_GPIO_PIN_0
#define ACC_ADC_IN_GPIO_Port GPIOC
#define LED_RED_Pin LL_GPIO_PIN_2
#define LED_RED_GPIO_Port GPIOB
#define LED_BLUE_Pin LL_GPIO_PIN_10
#define LED_BLUE_GPIO_Port GPIOB
#define LED_GREEN_Pin LL_GPIO_PIN_11
#define LED_GREEN_GPIO_Port GPIOB
#define HS_CAN2_RX_Pin LL_GPIO_PIN_12
#define HS_CAN2_RX_GPIO_Port GPIOB
#define HS_CAN2_TX_Pin LL_GPIO_PIN_13
#define HS_CAN2_TX_GPIO_Port GPIOB
#define USB_SENSE_Pin LL_GPIO_PIN_9
#define USB_SENSE_GPIO_Port GPIOA
#define MS_STB_Pin LL_GPIO_PIN_3
#define MS_STB_GPIO_Port GPIOB
#define MM_STB_Pin LL_GPIO_PIN_4
#define MM_STB_GPIO_Port GPIOB
#define MM_CAN2_RX_Pin LL_GPIO_PIN_5
#define MM_CAN2_RX_GPIO_Port GPIOB
#define MM_CAN2_TX_Pin LL_GPIO_PIN_6
#define MM_CAN2_TX_GPIO_Port GPIOB
#define HS_STB_Pin LL_GPIO_PIN_7
#define HS_STB_GPIO_Port GPIOB
#define MS_CAN1_RX_Pin LL_GPIO_PIN_8
#define MS_CAN1_RX_GPIO_Port GPIOB
#define MS_CAN1_TX_Pin LL_GPIO_PIN_9
#define MS_CAN1_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define PIDS_UPDATE_PERIOD				(500)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
