/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */
#include "console.h"
#include "obd2.h"
#include "lawicel_can.h"

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  if(x < in_min){
	  return out_min;
  }
  else if(x > in_max){
	  return out_max;
  }

  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/* USER CODE END 0 */

CAN_HandleTypeDef hcan2;

/* CAN2 init function */
void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */
  MS_CAN_TRANSCEIVER_DISABLE();
  MM_CAN_TRANSCEIVER_DISABLE();
  HS_CAN_TRANSCEIVER_DISABLE();
  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 9;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_6TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */
  	CAN_FilterTypeDef canFilterConfig;

  	/* Configure bank 15 for OBD2 responses 0x7E8, 0x7E9 */
	canFilterConfig.FilterBank = 15;
	canFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	canFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	canFilterConfig.FilterIdHigh = 0x07E8 << 5; // Filter IDs from 0x7E8 // Unused for IDMASK mode
	canFilterConfig.FilterIdLow = 0x0000;
	//canFilterConfig.FilterMaskIdHigh = 0x07FE << 5; // Filter IDs 0x7E8, 0x7E9 (Engine, Transmission)
	canFilterConfig.FilterMaskIdHigh = 0x0000 << 5; // ALL ID's is received to FIFO0
	canFilterConfig.FilterMaskIdLow = 0x0000;
	canFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	canFilterConfig.FilterActivation = ENABLE;
	canFilterConfig.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan2, &canFilterConfig);

//	/* Configure bank 16 for 0x201 messages that contains speed & RPM */
//	canFilterConfig.FilterBank = 16;
//	canFilterConfig.FilterIdHigh = 0x0201 << 5;
//	canFilterConfig.FilterIdLow = 0x0000;
//	canFilterConfig.FilterMaskIdHigh = 0x07FF << 5;
//	canFilterConfig.FilterMaskIdLow = 0x0000;
//	HAL_CAN_ConfigFilter(&hcan2, &canFilterConfig);
//
//	/* Configure bank 17 for 0x420 messages that contains coolant temp & intake pressure */
//	canFilterConfig.FilterBank = 17;
//	canFilterConfig.FilterIdHigh = 0x0420 << 5;
//	canFilterConfig.FilterIdLow = 0x0000;
//	canFilterConfig.FilterMaskIdHigh = 0x07FF << 5;
//	canFilterConfig.FilterMaskIdLow = 0x0000;
//	HAL_CAN_ConfigFilter(&hcan2, &canFilterConfig);

	HAL_CAN_Start(&hcan2);

	/* Enable FIFO0 pending ISR and TX mailbox empty ISR */
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY);

	/* Enable AUX & error ISR's */
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_OVERRUN |
										 CAN_IT_RX_FIFO0_FULL |
										 CAN_IT_WAKEUP |
										 CAN_IT_SLEEP_ACK |
										 CAN_IT_ERROR_WARNING |
										 CAN_IT_ERROR_PASSIVE |
										 CAN_IT_BUSOFF |
										 CAN_IT_LAST_ERROR_CODE |
										 CAN_IT_ERROR);

	HS_CAN_TRANSCEIVER_ENABLE();
  /* USER CODE END CAN2_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspInit 0 */

  /* USER CODE END CAN2_MspInit 0 */
    /* CAN2 clock enable */
    __HAL_RCC_CAN2_CLK_ENABLE();
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN2 interrupt Init */
    HAL_NVIC_SetPriority(CAN2_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspInit 1 */

  /* USER CODE END CAN2_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN2)
  {
  /* USER CODE BEGIN CAN2_MspDeInit 0 */

  /* USER CODE END CAN2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN2_CLK_DISABLE();
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN2 GPIO Configuration
    PB12     ------> CAN2_RX
    PB13     ------> CAN2_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN2_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN2_RX0_IRQn);
  /* USER CODE BEGIN CAN2_MspDeInit 1 */

  /* USER CODE END CAN2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef	RxHeader;
	uint8_t RxData[8];

	HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader, RxData);
	Can_LedBlinkOnPacketReceived();

//	console_print("%.8lu RX: ID=0x%X DLC=%lu %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\r\n",
//				HAL_GetTick(), RxHeader.StdId, RxHeader.DLC,
//				RxData[0], RxData[1], RxData[2], RxData[3], RxData[4], RxData[5], RxData[6], RxData[7]);

	/* Form lawicel frame and handle */
	lawicel_can_phy_frame_t frame = {0};
	frame.timestamp = RxHeader.Timestamp;
	frame.id = RxHeader.StdId;
	frame.dlc = RxHeader.DLC;
	memcpy(frame.data, RxData, sizeof(frame.data));
	lawicell_can_handle_received_phy_frame(&frame);

	// Check Engine Response ID
	if (RxHeader.StdId == 0x7E8 || RxHeader.StdId == 0x7E9) {
		obd2_rx_packet(RxData, GET_SIZE(RxData));
	}
	else if(RxHeader.StdId == 0x0F8){ //Focus ST/RS Only
		ecu.boost_bar = map(RxData[5], 0, 0xC0, 0, 200);
		//ecu.oil_temp = map(RxData[7], 0x70, 0xD0, 50, 150);
		ecu.oil_temp = (int16_t)RxData[7] - 60; //[-60|195]
		ecu.ptu_c = (int16_t)RxData[0] - 50; //[-50|205]
	}
	else if(RxHeader.StdId == 0x090){
		ecu.oil_pressure_bar = map(RxData[4], 0x60, 0x7F, 0, 50);
		ecu.rpm = ((RxData[4] & 0x0F) * 256 + RxData[5]) * 2;
	}
	else if(RxHeader.StdId == 0x130){
		ecu.speed_kmh = ((uint16_t)RxData[6] * 256 + RxData[7]) / 100;
	}
	else if(RxHeader.StdId == 0x2F0){
		ecu.coolant_c = (int16_t)RxData[5] - 60;
		ecu.intake_c = (int16_t)RxData[7] - 120;
	}
	else if(RxHeader.StdId == 0x340){
		ecu.ambient_c = (int16_t)RxData[7] - 60;
	}
	else if(RxHeader.StdId == 0x380){
		ecu.fuel = (uint16_t)RxData[0] * 100 / 255;
	}
}

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu RX (FIFO=0) Full!\r\n", HAL_GetTick());
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan){
	CAN_RxHeaderTypeDef	RxHeader;
	uint8_t RxData[8];

	HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO1, &RxHeader, RxData);

//	console_print("%.8lu RX: ID=0x%X DLC=%lu %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X\r\n",
//					HAL_GetTick(), RxHeader.StdId, RxHeader.DLC,
//					RxData[0], RxData[1], RxData[2], RxData[3], RxData[4], RxData[5], RxData[6], RxData[7]);
}

void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu RX (FIFO=1) Full!\r\n", HAL_GetTick());
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu CAN ERROR=0x%.8X\r\n", HAL_GetTick(), HAL_CAN_GetError(&hcan2));
	HAL_CAN_ResetError(&hcan2);
	Error_LedShortBlink();
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu TX (MBX=0) OK!\r\n", HAL_GetTick());
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu TX (MBX=1) OK!\r\n", HAL_GetTick());
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu TX (MBX=2) OK!\r\n", HAL_GetTick());
}

void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu TX (MBX=0) Abort!\r\n", HAL_GetTick());
}

void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu TX (MBX=1) Abort!\r\n", HAL_GetTick());
}

void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu TX (MBX=2) Abort!\r\n", HAL_GetTick());
}

void HAL_CAN_SleepCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu Sleep Callback!\r\n", HAL_GetTick());
}

void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef *hcan){
	console_print("%.8lu RX Wake-up Callback!\r\n", HAL_GetTick());
}

/* USER CODE END 1 */
