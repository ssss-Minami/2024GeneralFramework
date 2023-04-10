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
#include "CRC.h"
uint8_t referee_rx_len;    //裁判系统串口idle中断接收数据长度
uint8_t referee_pic_rx_len;
uint8_t referee_rx_buf[referee_buf_size];           //dma接收区
uint8_t referee_pic_rx_buf[referee_pic_buf_size];
uint8_t referee_tx_buf[referee_tx_buf_size];                        //maximum 128 bytes
Referee_StatusTypeDef Referee_Status = 2;//初始默认为可用
Referee_InfoTypedef Ref_Info;             //裁判系统数据
ext_robot_command_t Robot_Cmd;            //图传接收数据


int counter_test = 0;
uint8_t crc_ifcorrect;
uint16_t crc16_tx_test;
uint8_t tracking_test_now = 0, tracking_test_past = 0;

uint8_t referee_test_msg[] = "IRQ_entered/n";
graphic_data_struct_t graphic_data;
graphic_TxHeader_Typedef graphic_TxHeader;

void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	//stm32f4xx_it.c文件中相应函数已被注释
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE) == SET)  //空闲中断
	{
	__HAL_UART_CLEAR_IDLEFLAG(&huart1);
	HAL_UART_DMAStop(&huart1);
	referee_rx_len = referee_buf_size - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

	referee_solve(referee_rx_buf);
	Graphic_draw(1, 1);
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
	referee_pic_rx_len = referee_buf_size - __HAL_DMA_GET_COUNTER(&hdma_usart6_rx);

	referee_solve(referee_pic_rx_buf);
    memset(referee_pic_rx_buf, 0, referee_buf_size);

    HAL_UART_Receive_DMA(&huart6, referee_pic_rx_buf, referee_buf_size);
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);

	}
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
void referee_solve(uint8_t *data)
{
//
	if(data[0] != 0xA5)       //起始字节非0xA5直接跳出
	{
		Referee_Status = referee_error;
		return;
	}

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
		memcpy(&(Ref_Info.Game_Robot_HP), (data + Offset_data), LEN_game_robot_hp); //v1.4中有冲突，按sof中为准
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
			memcpy(&(Ref_Info.Shoot_Data), (data + Offset_data), LEN_shoot_data);//v1.4中冲突，以sof中为准
			break;
	case ID_bullet_remaining:
			memcpy(&(Ref_Info.bullet_remaining), (data + Offset_data), LEN_bullet_remaining);//v1.4中冲突，以sof中为准
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
//
//	if(data[offset_frame_tail+2] == 0xA5)
//		referee_solve(data);

}
uint8_t temp_total_len = 0;
uint8_t temp_data_len = 0;
void Graphic_draw(uint8_t isTracking_now, uint8_t isTracking_past)
{
	graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = 0;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw1;
	Append_CRC8_Check_Sum(&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0101;       //draw single graphic
	graphic_TxHeader.Tx_id = 0x5;
	graphic_TxHeader.Rx_id = graphic_TxHeader.Tx_id + 0x100;

//	graphic_TxHeader.Tx_id = Ref_Info.Game_Robot_state.robot_id;
//	graphic_TxHeader.Rx_id = Ref_Info.Game_Robot_state.robot_id + 0x100;  //receive_end client id


	//	if(isTracking_now ==0 && isTracking_past == 1)            //清除
//	{
//		graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_delete;
//		graphic_data.operate_tpye = graph_operate_delete;
//		temp_total_len = LEN_graph_TxHeader + LEN_data_delete + LEN_FRAME_TAIL;
//		temp_data_len = LEN_data_delete;
//	}
//	else if(isTracking_now ==1 && isTracking_past == 0)       //新增
//	{
//		graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw1;
//		graphic_data.operate_tpye = graph_operate_add;
//		temp_total_len = LEN_graph_TxHeader + LEN_data_draw1 + LEN_FRAME_TAIL;
//		temp_data_len = LEN_data_draw1;
//	}
//	else       //不变
//	{
//		graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw1;
//		graphic_data.operate_tpye = graph_operate_change;
//		temp_total_len = LEN_graph_TxHeader + LEN_data_draw1 + LEN_FRAME_TAIL;
//		temp_data_len = LEN_data_draw1;
//	}

	graphic_data.operate_tpye = graph_operate_add;
	temp_total_len = LEN_graph_TxHeader + LEN_data_draw1 + LEN_FRAME_TAIL;
	temp_data_len = LEN_data_draw1;
	memcpy(referee_tx_buf, &graphic_TxHeader, LEN_graph_TxHeader);

	graphic_data.graphic_name[0] = 0x01;
	graphic_data.graphic_name[1] = 0x01;       //若操作为清除，仅发送这两个字节以清除图层1
	graphic_data.graphic_name[2] = 0x00;
	graphic_data.graphic_tpye = graph_type_rectangle;
	graphic_data.layer = 1;
	graphic_data.color = graph_color_orange;
	graphic_data.start_angle = 0;
	graphic_data.end_angle = 0;
	graphic_data.width = graph_FrontSize_default;
	graphic_data.start_x = 300;
	graphic_data.start_y = 300;
	graphic_data.radius = 0;
	graphic_data.end_x = 600;
	graphic_data.end_y = 600;
	memcpy(referee_tx_buf +LEN_graph_TxHeader, &graphic_data, temp_data_len);

	Ref_Append_CRC16_Check_Sum(referee_tx_buf, temp_total_len);
	HAL_UART_Transmit(&huart1, referee_tx_buf, temp_total_len, 300);
	memset(referee_tx_buf, 0, referee_tx_buf_size);
}
