/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "can.h"
#include "pid.h"
#include "kalman.h"
#include "tim.h"
#include "spi.h"
#include "IMU.h"
#include "remote.h"
#include "Chassis.h"
#include "MahonyAHRS.h"
#include "math.h"
#include "string.h"
#include "referee.h"
#include "Serial.h"
#include <stdlib.h>
#include "SolveTrajectory.h"
#include "WatchDog.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define REMOTE_OR_KEYBORAD 0//选择遥控器或键鼠控制
extern uint8_t Message[];
float angle_yaw,angle_pitch,pitch,yaw,aim[3];
int16_t temp_yaw, temp_pitch, temp_ammofeed;
uint8_t Motor_Status,Motor_Status_last;
_send_packetinfo sd;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for SendMessage */
osThreadId_t SendMessageHandle;
const osThreadAttr_t SendMessage_attributes = {
  .name = "SendMessage",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ReceiveMessage */
osThreadId_t ReceiveMessageHandle;
const osThreadAttr_t ReceiveMessage_attributes = {
  .name = "ReceiveMessage",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ChangeTarget */
osThreadId_t ChangeTargetHandle;
const osThreadAttr_t ChangeTarget_attributes = {
  .name = "ChangeTarget",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for IMU_Read */
osThreadId_t IMU_ReadHandle;
const osThreadAttr_t IMU_Read_attributes = {
  .name = "IMU_Read",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for start_USB_CDC */
osThreadId_t start_USB_CDCHandle;
const osThreadAttr_t start_USB_CDC_attributes = {
  .name = "start_USB_CDC",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void start_SendMessage(void *argument);
void startReceiveMessage(void *argument);
void fun_ChangeTarget(void *argument);
void StartIMU_Read(void *argument);
void StartTask08(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of SendMessage */
  SendMessageHandle = osThreadNew(start_SendMessage, NULL, &SendMessage_attributes);

  /* creation of ReceiveMessage */
  ReceiveMessageHandle = osThreadNew(startReceiveMessage, NULL, &ReceiveMessage_attributes);

  /* creation of ChangeTarget */
  ChangeTargetHandle = osThreadNew(fun_ChangeTarget, NULL, &ChangeTarget_attributes);

  /* creation of IMU_Read */
  IMU_ReadHandle = osThreadNew(StartIMU_Read, NULL, &IMU_Read_attributes);

  /* creation of start_USB_CDC */
  start_USB_CDCHandle = osThreadNew(StartTask08, NULL, &start_USB_CDC_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_start_SendMessage */
/**
* @brief Function implementing the SendMessage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_SendMessage */
void start_SendMessage(void *argument)
{
  /* USER CODE BEGIN start_SendMessage */

  /* Infinite loop */
  for(;;)
  {
	  /***云台外环PID***/
	  PID_Origin(&PID_Motor_Angle[Motor_Pitch_ID], Motor[Motor_Pitch_ID].angle, Motor[Motor_Pitch_ID].target_angle);
	  PID_Origin(&PID_Motor_Angle[Motor_Yaw_ID], imu_data.angle[0], Motor[Motor_Yaw_ID].target_angle);
	  /***云台内环和拨弹轮PID***/
	  PID_Incr(&PID_Motor_Speed[Motor_AmmoFeed_ID],Motor[Motor_AmmoFeed_ID].speed,Motor[Motor_AmmoFeed_ID].target_speed);
      PID_Incr(&PID_Motor_Speed[Motor_Yaw_ID], Motor[Motor_Yaw_ID].speed, PID_Motor_Angle[Motor_Yaw_ID].Output);
//      PID_Incr(&PID_Motor_Speed[Motor_Yaw_ID], Motor[Motor_Yaw_ID].speed, Motor[Motor_Yaw_ID].target_speed);
	  PID_Incr(&PID_Motor_Speed[Motor_Pitch_ID],Motor[Motor_Pitch_ID].speed,PID_Motor_Angle[Motor_Pitch_ID].Output);

	  temp_yaw += PID_Motor_Speed[Motor_Yaw_ID].Output;
	  temp_pitch += PID_Motor_Speed[Motor_Pitch_ID].Output;
	  temp_ammofeed += PID_Motor_Speed[Motor_AmmoFeed_ID].Output;

	  Can_TxData[0] = (temp_pitch>>8);
	  Can_TxData[1] = temp_pitch;

	  Can_TxData[2] = (temp_yaw>>8);
	  Can_TxData[3] = temp_yaw;

	  Can_TxData[4] = (temp_ammofeed>>8);
	  Can_TxData[5] = temp_ammofeed;

	  HAL_CAN_AddTxMessage(&hcan1, &Can_cmdHeader[Motor_Pitch_ID], Can_TxData, (uint32_t*)CAN_TX_MAILBOX0);
	  //osDelay(1);
	  /***底盘全向移动***/
	  Chassis_Move();
	  //Chassis_Ctrl();
	  Can_TxData[0] = Chassis_ctrl[0]>>8;
	  Can_TxData[1] = Chassis_ctrl[0];
 	  Can_TxData[2] = Chassis_ctrl[1]>>8;
 	  Can_TxData[3] = Chassis_ctrl[1];
 	  Can_TxData[4] = Chassis_ctrl[2]>>8;
 	  Can_TxData[5] = Chassis_ctrl[2];
 	  Can_TxData[6] = Chassis_ctrl[3]>>8;
 	  Can_TxData[7] = Chassis_ctrl[3];

	  HAL_CAN_AddTxMessage(&hcan1,&Can_cmdHeader[Motor_LeftFront_ID],Can_TxData,(uint32_t*)CAN_TX_MAILBOX0);
	  osDelay(1);
  }
  /* USER CODE END start_SendMessage */
}

/* USER CODE BEGIN Header_startReceiveMessage */
/**
* @brief Function implementing the ReceiveMessage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_startReceiveMessage */
void startReceiveMessage(void *argument)
{
  /* USER CODE BEGIN startReceiveMessage */
	uint8_t ammo_count=0, ammo_temp = 0;
	GPIO_PinState pinstate = GPIO_PIN_SET;
  /* Infinite loop */
  for(;;)
  {
	  /***�??测已发弹�??***/
	  if(HAL_GPIO_ReadPin(Ammo_Counter_GPIO_Port, Ammo_Counter_Pin) != pinstate)
	  {
		  pinstate = !pinstate;
		  ammo_temp++;
	  }
	  if(ammo_temp >=2)
	  {
		  ammo_count++;
		  ammo_temp = 0;
	  }

    osDelay(20);
  }
  /* USER CODE END startReceiveMessage */
}

/* USER CODE BEGIN Header_fun_ChangeTarget */
/**
* @brief Function implementing the ChangeTarget thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fun_ChangeTarget */
void fun_ChangeTarget(void *argument)
{
  /* USER CODE BEGIN fun_ChangeTarget */
  /* Infinite loop */
	for(;;)
  {
		Motor_Status_last = Motor_Status;
		Motor_Status = 1;
		Dog_Status_update(&remote_WatchDog);//遥控器看门狗状�?�更�?
		Dog_Status_update(&referee_WatchDog);//图传看门狗状态更�?
		for(uint8_t i=1;i<8;i++)
		{
			Dog_Status_update(&motor_WatchDog[i]);
			if(!motor_WatchDog[i].status)
				Motor_Status = 0;
		}
		if(!Motor_Status_last)
			if(Motor_Status)
			{
				SPEED_INIT(1);
				SPEED_SET(1000);
			}
		if(referee_WatchDog.status)
		{
			/**********************键鼠控制******************************/
			speed_x_commend = 0.9*(RC_Ctl.keyboard.W - RC_Ctl.keyboard.S);
			speed_y_commend = 0.9*(RC_Ctl.keyboard.D - RC_Ctl.keyboard.A);

			Chassis_angleTransform();

			if(receinfo->tracking && RC_Ctl.keyboard.r)
			{
				GimbalControlInit( angle_pitch, angle_yaw,receinfo->yaw, receinfo->v_yaw,receinfo->r1,receinfo->r2,receinfo->z_2, 18, 0.076);
				GimbalControlTransform(receinfo->x, receinfo->y, receinfo->z,receinfo->vx,receinfo->vy,receinfo->vz,1, &pitch, &yaw
						,&aim[0],&aim[1],&aim[2]);
				Motor[Motor_Yaw_ID].target_angle = yaw;
				Motor[Motor_Pitch_ID].target_angle = (uint16_t)(-pitch*4096/3.1415926535f + 3400);
			}

			Motor[Motor_Yaw_ID].target_angle -= (RC_Ctl.keyboard.x >> 4)*0.001;
			Motor[Motor_Pitch_ID].target_angle -= (RC_Ctl.keyboard.y >> 4);

			if(Motor[6].target_angle > 3.1415926535f) Motor[6].target_angle -= 2*3.1415926535f;
			if(Motor[6].target_angle < -3.1415926535f) Motor[6].target_angle += 2*3.1415926535f;

			if(Motor[Motor_Pitch_ID].target_angle > 3700) Motor[Motor_Pitch_ID].target_angle = 3700;
			if(Motor[Motor_Pitch_ID].target_angle < 2900) Motor[Motor_Pitch_ID].target_angle = 2900;

			if(Ref_Info.Power_Heat_Data.shooter_heat0 <= 220)
				Motor[Motor_AmmoFeed_ID].target_speed = 1800*(RC_Ctl.keyboard.l);
			else Motor[Motor_AmmoFeed_ID].target_speed = 0;

			if(RC_Ctl.keyboard.SHIFT)
			{
				PID_Motor_Angle[6].Ki = 0.25;
				PID_Motor_Angle[6].Err_sum_Max = 300;
				Chassis_Spin();
			}
			else {
				PID_Motor_Angle[6].Ki = 0;
				PID_Motor_Angle[6].Err_sum_Max = 100;
				Chassis_Follow();
			}
		}
		else if(remote_WatchDog.status)
		{
			/****************遥控器控�??*******************/
			/***相对云台的�?�度输入***/
			speed_x_commend = RC_Ctl.rc.ch2*0.002;
			speed_y_commend = RC_Ctl.rc.ch1*0.002;
			/***云台到底盘的速度转换****/
			Chassis_angleTransform();
			/***自瞄***/
			if(receinfo->tracking && RC_Ctl.rc.sw1 == 1)
			{
				GimbalControlInit( angle_pitch, angle_yaw,receinfo->yaw, receinfo->v_yaw,receinfo->r1,receinfo->r2,receinfo->z_2, 18, 0.076);
				GimbalControlTransform(receinfo->x, receinfo->y, receinfo->z,receinfo->vx,receinfo->vy,receinfo->vz,1, &pitch, &yaw
						,&aim[0],&aim[1],&aim[2]);
				Motor[Motor_Yaw_ID].target_angle = yaw;
				Motor[Motor_Pitch_ID].target_angle = (uint16_t)(-pitch*4096/3.1415926535f + 3400);
			}
			/***云台控制输入**/
			Motor[Motor_Yaw_ID].target_angle -= (RC_Ctl.rc.ch3>>5)*0.001;
			Motor[Motor_Pitch_ID].target_angle += (RC_Ctl.rc.ch4>>6);

			if(Motor[6].target_angle > 3.1415926535f) Motor[6].target_angle -= 2*3.1415926535f;
			if(Motor[6].target_angle < -3.1415926535f) Motor[6].target_angle += 2*3.1415926535f;

			if(Motor[Motor_Pitch_ID].target_angle > 3700)
				Motor[Motor_Pitch_ID].target_angle = 3700;
			if(Motor[Motor_Pitch_ID].target_angle < 2900)
				Motor[Motor_Pitch_ID].target_angle = 2900;
			/*****拨弹轮控制输�??******/
			if((RC_Ctl.rc.wheel) && Ref_Info.Power_Heat_Data.shooter_heat0 <= 220)
			{
				Motor[Motor_AmmoFeed_ID].target_speed = 1800;
			}
			else
			{
				Motor[Motor_AmmoFeed_ID].target_speed = 0;
			}
			/****底盘状�?��?�择****/
			if(RC_Ctl.rc.sw2 == 2)
			{
				PID_Motor_Angle[6].Ki = 0.1;
				PID_Motor_Angle[6].Err_sum_Max = 300;
				Chassis_Spin();//小陀�??
			}
			else if(RC_Ctl.rc.sw2 == 1)
			{
				PID_Motor_Angle[6].Ki = 0;
				PID_Motor_Angle[6].Err_sum_Max = 100;
				Chassis_Follow();//底盘跟随
			}
			else {
				PID_Motor_Angle[6].Ki = 0;
				PID_Motor_Angle[6].Err_sum_Max = 50;
				omega = 0;//底盘不跟�??
			}
		}
		/**掉线保护***/
		if((!remote_WatchDog.status && !referee_WatchDog.status) || !Motor_Status)//遥控器和图传均掉线时pid超参数与输出全部�?0
		{
			for(int i=0;i<8;i++)
			{
				PID_Clear(&PID_Motor_Angle[i]);
				PID_Clear(&PID_Motor_Speed[i]);
			}
			temp_yaw = 0;
			temp_pitch = 0;
			temp_ammofeed = 0;
			Chassis_ctrl[0] = 0;
			Chassis_ctrl[1] = 0;
			Chassis_ctrl[2] = 0;
			Chassis_ctrl[3] = 0;
		}
		else if(PID_Motor_Speed[1].Kp == 0)//有至少一个控制器在线时重新初始化
			PID_Init();
		osDelay(5);
  }


  /* USER CODE END fun_ChangeTarget */
}

/* USER CODE BEGIN Header_StartIMU_Read */
/**
* @brief Function implementing the IMU_Read thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartIMU_Read */
void StartIMU_Read(void *argument)
{
  /* USER CODE BEGIN StartIMU_Read */
  /* Infinite loop */
  for(;;)
  {
	  /***读取imu***/
	 BMI088_read_Gyro(&imu_data);
	 BMI088_read_Accel(&imu_data);
//	 IST8310_read(&imu_data);
	 /***量程转换***/
	 for(int i=0;i<3;i++)
	 {

			 imu_gyro[i] = (imu_data.gyro[i])/65.536*(PI/180);

			 imu_accel[i] = imu_data.accel[i]*0.0008974f;
//		 imu_mag[i] = imu_data.mag[i]*0.3;

	 }
	 /***减去零偏值（零偏�??标定获取�??***/
	 imu_gyro[1] -= (11.5390333f / 65.536)*(PI/180);
	 imu_gyro[2] -= (10.4231017f / 65.536)*(PI/180);
	 imu_accel[1] -= (141.763613f * 0.0008974);

	 /***均�?�滤�??***/
	 MahonyAHRSupdateIMU(imu_data.angle_q, imu_gyro[0], imu_gyro[1], imu_gyro[2], imu_accel[0], imu_accel[1], imu_accel[2]);
	 imu_data.angle[0] = atan2f(2.0f*(imu_data.angle_q[0]*imu_data.angle_q[3]+imu_data.angle_q[1]*imu_data.angle_q[2]), 2.0f*(imu_data.angle_q[0]*imu_data.angle_q[0]+imu_data.angle_q[1]*imu_data.angle_q[1])-1.0f);
	 imu_data.angle[1] = asinf(-2.0f*(imu_data.angle_q[1]*imu_data.angle_q[3]-imu_data.angle_q[0]*imu_data.angle_q[2]));
	 imu_data.angle[2] = atan2f(2.0f*(imu_data.angle_q[0]*imu_data.angle_q[1]+imu_data.angle_q[2]*imu_data.angle_q[3]),2.0f*(imu_data.angle_q[0]*imu_data.angle_q[0]+imu_data.angle_q[3]*imu_data.angle_q[3])-1.0f);

	 angle_yaw = imu_data.angle[0];
	 angle_pitch = -(Motor[Motor_Pitch_ID].angle - 3400)*3.1415926353f/4096;

    osDelay(10);
  }
  /* USER CODE END StartIMU_Read */
}

/* USER CODE BEGIN Header_StartTask08 */
/**
* @brief Function implementing the start_USB_CDC thread.
* @param argument: Not used
* @retval None
*/
uint8_t RecePackage[30];
/* USER CODE END Header_StartTask08 */
void StartTask08(void *argument)
{
  /* USER CODE BEGIN StartTask08 */

		_send_packetinfo sd;
		uint8_t RecePackage[sizeof(_receive_packet)];
		sd = (_sendpacket *)malloc(sizeof(_sendpacket));
		receinfo = (_receive_packetinfo)malloc(sizeof(_receive_packet));
		sd->header = 0X5A;
		sd->robot_color = 1;
		sd->task_mode= 2;
		sd->reserve = 5;

		sd->checksum=0X00;
		memset(receinfo,0,sizeof(_receivepacket));
	  /* Infinite loop */
	  for(;;)
	  {
		  /***向上位机发�?�角�??***/
		  sd->pitch = angle_pitch;
		  sd->yaw = angle_yaw;
		  if(!receinfo->tracking)
		  {
			  sd->aim_x = 0;
			  sd->aim_y = 0;
			  sd->aim_z = 0;
		  }
		  else{
			  sd->aim_x = aim[0];
			  sd->aim_y = aim[1];
			  sd->aim_z = aim[2];
		  }
		  //memmove(temp_CRC,sd,10);
//		  sd->checksum=Get_CRC16_Check_Sum(sd, temp_CRC, SendData, 24, 0xFFFF);
		  Pack_And_Send_Data_ROS2(sd,(size_t)sizeof(_sendpacket));
		  CDC_Receive_ROS2(RecePackage, (size_t)sizeof(_receive_packet), receinfo);
		  osDelay(1);
	  }
  /* USER CODE END StartTask08 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

