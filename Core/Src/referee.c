/*
 * @file		referee.c/h
 * @brief	裁判系统、图传串口中断
 * @history
 * 版本			作者			编写日期
 * v1.0.0		姚启杰		2023/4/1
 *
 */
#include "main.h"
#include "usart.h"
#include "dma.h"
#include "string.h"
#include "referee.h"
#include "remote.h"
#include "WatchDog.h"
uint8_t referee_rx_len;    //裁判系统串口idle中断接收数据长度
uint8_t referee_rx_buf[referee_buf_size];           //dma接收区
uint8_t referee_data[referee_buf_size];
Referee_StatusTypeDef Referee_Status = 2;//初始默认为可用
Referee_InfoTypedef Ref_Info;
ext_robot_command_t Robot_Cmd;
int counter_test = 0;
uint8_t referee_test_msg[] = "IRQ_entered/n";
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	//stm32f4xx_it.c文件中相应函数已被注释
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) != RESET)  //空闲中断
	{
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	HAL_UART_DMAStop(&huart1);
	referee_rx_len = referee_buf_size - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
	referee_solve(referee_rx_buf);
//	if(Referee_Status == referee_OK)

		memcpy(referee_data, referee_rx_buf, referee_rx_len);


    memset(referee_rx_buf, 0, referee_buf_size);
    HAL_UART_Receive_DMA(&huart1, referee_rx_buf, referee_buf_size);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

	}
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	feedDog(&referee_WatchDog);//若进入中断，则喂狗
	//stm32f4xx_it.c文件中相应函数已被注释
	if(__HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE) == SET)  //空闲中断
	{
	__HAL_UART_CLEAR_IDLEFLAG(&huart6);
	HAL_UART_DMAStop(&huart6);
	referee_rx_len = referee_buf_size - __HAL_DMA_GET_COUNTER(&hdma_usart6_rx);

//	if(Referee_Status == referee_OK)

	referee_solve(referee_rx_buf);
    memset(referee_rx_buf, 0, referee_buf_size);

    HAL_UART_Receive_DMA(&huart6, referee_rx_buf, referee_buf_size);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);

	}
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
void referee_solve(uint8_t *data)
{

	if(data[0] != 0xA5)       //起始字节非0xA5直接跳出
	{
		Referee_Status = referee_error;
		return;
	}
//	Referee_Status = referee_solving;
	uint16_t offset_frame_tail = (data[Offset_SOF_DataLength + 1]<<8) + data[Offset_SOF_DataLength]
						+LEN_CMD_ID +LEN_FRAME_HEAD;
	uint16_t cmd_id = (data[Offset_cmd_ID + 1] << 8) + data[Offset_cmd_ID];
	uint16_t data_length = (data[Offset_SOF_DataLength + 1] >> 8) + data[Offset_SOF_DataLength];
	switch(cmd_id)
	{
	//0x000
	case ID_game_status:
		memcpy(&(Ref_Info.Game_Status), (data + Offset_data), LEN_game_state);
		break;
	case ID_game_result:
		memcpy(&(Ref_Info.Game_Result), (data + Offset_data), LEN_game_result);
		break;
	case ID_game_robot_hp:
		memcpy(&(Ref_Info.Game_Robot_HP), (data + Offset_data), data_length); //v1.4中有冲突，按sof中为准
		break;
	case ID_game_dart_state:
		memcpy(&(Ref_Info.Game_Dart_status), (data + Offset_data), LEN_game_dart_state);
		break;

	//0x100
	case ID_event_data:
			memcpy(&(Ref_Info.Event_Data), (data + Offset_data), LEN_event_data);
			break;
	case ID_supply_projectile_action:
			memcpy(&(Ref_Info.Supply_Projectile_Action), (data + Offset_data), LEN_supply_projectile_action);
			break;
	case ID_referee_warn:
			memcpy(&(Ref_Info.Referee_Warning), (data + Offset_data), LEN_referee_warn);
			break;
	case ID_dart_shoot_time:
			memcpy(&(Ref_Info.dart_remaining_time), (data + Offset_data), LEN_dart_remaining_time);
			break;

	//0x200
	case ID_game_robot_state:
			memcpy(&(Ref_Info.Game_Robot_state), (data + Offset_data), LEN_game_robot_state);
			break;
	case ID_power_heat_data:
			memcpy(&(Ref_Info.Power_Heat_Data), (data + Offset_data), LEN_power_heat_data);
			break;
	case ID_game_robot_pos:
			memcpy(&(Ref_Info.Game_Robot_Pos), (data + Offset_data), LEN_game_robot_pos);
			break;
	case ID_buff_musk:
			memcpy(&(Ref_Info.Buff_Musk), (data + Offset_data), LEN_buff_musk);
			break;
	case ID_aerial_robot_energy:
			memcpy(&(Ref_Info.Aerial_Robot_Energy), (data + Offset_data), LEN_aerial_robot_energy);
			break;
	case ID_robot_hurt:
			memcpy(&(Ref_Info.Game_Status), (data + Offset_data), LEN_robot_hurt);
			break;
	case ID_shoot_data:
			memcpy(&(Ref_Info.Shoot_Data), (data + Offset_data), data_length);//v1.4中冲突，以sof中为准
			break;
	case ID_bullet_remaining:
			memcpy(&(Ref_Info.bullet_remaining), (data + Offset_data), data_length);//v1.4中冲突，以sof中为准
			break;
	case ID_rfid_status:
			memcpy(&(Ref_Info.rfid_status), (data + Offset_data), LEN_rfid_status);
			break;
	case ID_dart_client_directive:
			memcpy(&(Ref_Info.dart_client), (data + Offset_data), LEN_dart_client_directive);
			break;

	//0x300
	case ID_keyboard_information:
			memcpy(&(RC_Ctl.keyboard), (data + Offset_data), LEN_keyboard_information);
			break;
	}
//	memset(referee_data, 0, referee_buf_size);

}
void referee_photo_solve(uint8_t *data)
{
	memcpy(&RC_Ctl.keyboard, data + 6, 10);
}
