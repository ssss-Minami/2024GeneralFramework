/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void PWM_INIT();
void SPEED_INIT(int speed);
void SPEED_SET(int speed);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IST8310_Reset_Pin GPIO_PIN_6
#define IST8310_Reset_GPIO_Port GPIOG
#define LED_R_Pin GPIO_PIN_12
#define LED_R_GPIO_Port GPIOH
#define IS8310_Ready_Pin GPIO_PIN_3
#define IS8310_Ready_GPIO_Port GPIOG
#define LED_G_Pin GPIO_PIN_11
#define LED_G_GPIO_Port GPIOH
#define LED_B_Pin GPIO_PIN_10
#define LED_B_GPIO_Port GPIOH
#define CS1_Accel_Pin GPIO_PIN_4
#define CS1_Accel_GPIO_Port GPIOA
#define Ammo_Counter_Pin GPIO_PIN_12
#define Ammo_Counter_GPIO_Port GPIOB
#define CS1_Gyro_Pin GPIO_PIN_0
#define CS1_Gyro_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define Motor_LeftFront_ID 1
#define Motor_LeftRear_ID 2
#define Motor_RightRear_ID 3
#define Motor_RightFront_ID 4
#define Motor_Pitch_ID 5
#define Motor_Yaw_ID 6
#define Motor_AmmoFeed_ID 7            //拨弹电机
#define Chassis_Angle_ID 0

#define AmmoBooster_Speed 2200
#define IST8310_I2C_ADDR 0x0E

#define  RC_FRAME_LENGTH 18
#define  SBUS_RX_BUF_NUM 36
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
