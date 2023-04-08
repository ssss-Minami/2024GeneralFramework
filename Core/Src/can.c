/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "pid.h"
#include "main.h"
uint8_t Can_RxData[8];
uint8_t Can_TxData[8];
CAN_TxHeaderTypeDef Can_cmdHeader[8];            //为方便使用，[0]空出
CAN_RxHeaderTypeDef Can_recHeader[8], sCan_RxHeader;
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 6;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void Can_MessageConfig(void)
{
	for(int i=0; i<8; i++)           //无特殊情况批量设�????
	{
		Can_cmdHeader[i].ExtId =   0x0;
		Can_cmdHeader[i].IDE = CAN_ID_STD;
		Can_cmdHeader[i].RTR = CAN_RTR_DATA;
		Can_cmdHeader[i].DLC = 8;
		Can_recHeader[i].ExtId = 0x0;
		Can_recHeader[i].IDE = CAN_ID_STD;
		Can_recHeader[i].RTR = CAN_RTR_DATA;
		Can_recHeader[i].DLC = 8;
	}
	Can_cmdHeader[Motor_LeftFront_ID].StdId = 0x200;
	Can_recHeader[Motor_LeftFront_ID].StdId = 0x201;

	Can_cmdHeader[Motor_LeftRear_ID].StdId = 0x200;
	Can_recHeader[Motor_LeftRear_ID].StdId = 0x202;

	Can_cmdHeader[Motor_RightRear_ID].StdId = 0x200;
	Can_recHeader[Motor_RightRear_ID].StdId = 0x203;

	Can_cmdHeader[Motor_RightFront_ID].StdId = 0x200;
	Can_recHeader[Motor_RightFront_ID].StdId = 0x204;

	Can_cmdHeader[Motor_Pitch_ID].StdId = 0x1FF;
    Can_recHeader[Motor_Pitch_ID].StdId = 0x205;           //pitch id=1

	Can_cmdHeader[Motor_Yaw_ID].StdId = 0x1FF;
    Can_recHeader[Motor_Yaw_ID].StdId = 0x206;             //yaw id=2

	Can_cmdHeader[Motor_AmmoFeed_ID].StdId = 0x1FF;
    Can_recHeader[Motor_AmmoFeed_ID].StdId = 0x207;               //c610 id =7



}
void Can_Filter1Config(void)
{
	CAN_FilterTypeDef Filter_1;
	Filter_1.FilterActivation = ENABLE;
	Filter_1.FilterMode = CAN_FILTERMODE_IDMASK;
	Filter_1.FilterScale = CAN_FILTERSCALE_16BIT;
	Filter_1.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	Filter_1.FilterIdHigh = 0x0000;
	Filter_1.FilterIdLow = 0x0000;
	Filter_1.FilterMaskIdHigh = 0x0000;
	Filter_1.FilterMaskIdLow = 0x0000;                      //全部接收
	Filter_1.FilterBank = 0;
	HAL_CAN_ConfigFilter(&hcan1, &Filter_1);
}
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == CAN1)
	{
		HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &sCan_RxHeader, Can_RxData);
		switch(sCan_RxHeader.StdId)
		{
		case 0x201:
			Motor[1].speed = (uint16_t)(Can_RxData[2]<<8) + Can_RxData[3];
			Motor[1].angle = ((Can_RxData[0]<<8) + Can_RxData[1]);
			Motor[1].current = (uint16_t)(Can_RxData[4]<<8) + Can_RxData[5];
			Motor[1].temp = Can_RxData[6];
		break;
		case 0x202:
			Motor[2].speed = (uint16_t)(Can_RxData[2]<<8) + Can_RxData[3];
			Motor[2].angle = ((Can_RxData[0]<<8) + Can_RxData[1]);
			Motor[2].current = (uint16_t)(Can_RxData[4]<<8) + Can_RxData[5];
			Motor[2].temp = Can_RxData[6];
		break;
		case 0x203:
			Motor[3].speed = (uint16_t)(Can_RxData[2]<<8) + Can_RxData[3];
			Motor[3].angle = ((Can_RxData[0]<<8) + Can_RxData[1]);
			Motor[3].current = (uint16_t)(Can_RxData[4]<<8) + Can_RxData[5];
			Motor[3].temp = Can_RxData[6];
		break;
		case 0x204:
			Motor[4].speed = (uint16_t)(Can_RxData[2]<<8) + Can_RxData[3];
			Motor[4].angle = ((Can_RxData[0]<<8) + Can_RxData[1]);
			Motor[4].current = (uint16_t)(Can_RxData[4]<<8) + Can_RxData[5];
			Motor[4].temp = Can_RxData[6];
		break;
		case 0x205:
			Motor[Motor_Pitch_ID].speed = (uint16_t)(Can_RxData[2]<<8) + Can_RxData[3];
			Motor[Motor_Pitch_ID].angle = ((Can_RxData[0]<<8) + Can_RxData[1]);
			Motor[Motor_Pitch_ID].current = (uint16_t)(Can_RxData[4]<<8) + Can_RxData[5];
			Motor[Motor_Pitch_ID].temp = Can_RxData[6];
		break;

		case 0x206:
			Motor[Motor_Yaw_ID].speed = (uint16_t)(Can_RxData[2]<<8) + Can_RxData[3];
			Motor[Motor_Yaw_ID].angle = ((Can_RxData[0]<<8) + Can_RxData[1]);
			Motor[Motor_Yaw_ID].current = (uint16_t)(Can_RxData[4]<<8) + Can_RxData[5];
			Motor[Motor_Yaw_ID].temp = Can_RxData[6];

		break;

		case 0x207:
		     Motor[Motor_AmmoFeed_ID].speed = (uint16_t)(Can_RxData[2]<<8) + Can_RxData[3];
		break;
		}


	}
}
/* USER CODE END 1 */
