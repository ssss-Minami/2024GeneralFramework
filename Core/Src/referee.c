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
Referee_InfoTypedef Ref_Info;             //裁判系统数据
graphic_data_struct_t const_char, aim_char, chassis_char, droppoint_rectangle;
graphic_TxHeader_Typedef graphic_TxHeader;
uint8_t UI_Seq = 0;


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

	uint16_t offset_frame_tail = (data[Offset_SOF_DataLength + 1]<<8) + data[Offset_SOF_DataLength]
							+LEN_CMD_ID +LEN_FRAME_HEAD;
	uint16_t cmd_id = (data[Offset_cmd_ID + 1] << 8) + data[Offset_cmd_ID];

	if(data[0] != 0xA5 || !Ref_Verify_CRC16_Check_Sum(data, offset_frame_tail +2)
			           || !Verify_CRC8_Check_Sum(data, LEN_FRAME_HEAD))
	{
		return;
	}

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

	if(data[offset_frame_tail+2] == 0xA5)
		referee_solve(data +offset_frame_tail +2);

}

void UI_Print_char(graphic_data_struct_t *graphic_data, uint8_t *char_to_send, uint8_t ui_color, uint32_t x, uint32_t y)
{
	memset(referee_tx_buf, 0, referee_tx_buf_size);
	uint8_t pointer = 0;

	graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw1 +LEN_data_char;
	Append_CRC8_Check_Sum(&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0110;       //draw single char
	if(graphic_TxHeader.Tx_id == 0 || graphic_TxHeader.Rx_id == 0)
		{
			graphic_TxHeader.Tx_id = Ref_Info.Game_Robot_state.robot_id;
			graphic_TxHeader.Rx_id = Ref_Info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
		}
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;

	graphic_data->graphic_name[0] = char_to_send[0];
	graphic_data->graphic_name[1] = char_to_send[1];
	graphic_data->graphic_name[2] = char_to_send[2];
	graphic_data->operate_tpye = graph_operate_add;
	graphic_data->graphic_tpye = graph_type_char;
	graphic_data->layer = 1;
	graphic_data->color = ui_color;
	graphic_data->start_angle = graph_FrontSize_default;
	graphic_data->end_angle = 120;
	graphic_data->width = graph_LineWidth_default;
	graphic_data->start_x = x;
	graphic_data->start_y = y;
	graphic_data->radius = 0;
	graphic_data->end_x = 0;
	graphic_data->end_y = 0;
	memcpy(referee_tx_buf +pointer, graphic_data, sizeof(*graphic_data));
	pointer += sizeof(*graphic_data);

	memcpy(referee_tx_buf +pointer, char_to_send, LEN_data_char);
	pointer += LEN_data_char;

	Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart1, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);
}


void UI_Print_rectangle(graphic_data_struct_t *graphic_data, uint8_t ui_color, uint8_t ui_layer, uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y)
{
	memset(referee_tx_buf, 0, referee_tx_buf_size);
	uint8_t pointer = 0;

	graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw1;
	Append_CRC8_Check_Sum(&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0101;       //draw single char
	if(graphic_TxHeader.Tx_id == 0 || graphic_TxHeader.Rx_id == 0)
		{
			graphic_TxHeader.Tx_id = Ref_Info.Game_Robot_state.robot_id;
			graphic_TxHeader.Rx_id = Ref_Info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
		}
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;

	graphic_data->graphic_name[0] = 1;
	graphic_data->graphic_name[1] = 1;
	graphic_data->graphic_name[2] = 0;
	graphic_data->operate_tpye = graph_operate_add;
	graphic_data->graphic_tpye = graph_type_rectangle;
	graphic_data->layer = ui_layer;
	graphic_data->color = ui_color;
	graphic_data->start_angle = 0;
	graphic_data->end_angle = 0;
	graphic_data->width = graph_LineWidth_default;
	graphic_data->start_x = start_x;
	graphic_data->start_y = start_y;
	graphic_data->radius = 0;
	graphic_data->end_x = end_x;
	graphic_data->end_y = end_y;
	memcpy(referee_tx_buf +pointer, graphic_data, sizeof(*graphic_data));
	pointer += sizeof(*graphic_data);

	Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart1, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);
}

void UI_Clear_layer(uint8_t ui_layer)
{
	memset(referee_tx_buf, 0, referee_tx_buf_size);
	uint8_t temp_arr[2];
	uint8_t pointer = 0;

	graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_delete;
	Append_CRC8_Check_Sum(&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0100;       //clear graph
	if(graphic_TxHeader.Tx_id == 0 || graphic_TxHeader.Rx_id == 0)
		{
			graphic_TxHeader.Tx_id = Ref_Info.Game_Robot_state.robot_id;
			graphic_TxHeader.Rx_id = Ref_Info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
		}
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;

	temp_arr[0] = 1;
	temp_arr[1] = ui_layer;
	memcpy(referee_tx_buf +pointer, temp_arr, LEN_data_delete);
	pointer += LEN_data_delete;

	Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart1, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);
}

void UI_Refresh_graph(graphic_data_struct_t *graphic_data, uint32_t new_y)
{
	memset(referee_tx_buf, 0, referee_tx_buf_size);
	uint8_t pointer = 0;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw1;
	if(graphic_TxHeader.Tx_id == 0 || graphic_TxHeader.Rx_id == 0)
	{
		graphic_TxHeader.Tx_id = Ref_Info.Game_Robot_state.robot_id;
		graphic_TxHeader.Rx_id = Ref_Info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
	}
	Append_CRC8_Check_Sum(&graphic_TxHeader, LEN_FRAME_HEAD);
	graphic_TxHeader.Data_id = 0x101;     //print single graph
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;

	graphic_data->operate_tpye = graph_operate_change;
	graphic_data->start_y = new_y -20;
	graphic_data->end_y = new_y +30;
	memcpy(referee_tx_buf +pointer, graphic_data, sizeof(*graphic_data));
	pointer += sizeof(*graphic_data);

	Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart1, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);

}

void UI_Refresh_char(graphic_data_struct_t *graphic_data, uint8_t *new_char)
{
	memset(referee_tx_buf, 0, referee_tx_buf_size);
	uint8_t pointer = 0;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw1 + LEN_data_char;
	if(graphic_TxHeader.Tx_id == 0 || graphic_TxHeader.Rx_id == 0)
	{
		graphic_TxHeader.Tx_id = Ref_Info.Game_Robot_state.robot_id;
		graphic_TxHeader.Rx_id = Ref_Info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
	}
	Append_CRC8_Check_Sum(&graphic_TxHeader, LEN_FRAME_HEAD);
	graphic_TxHeader.Data_id = 0x110;    //print char
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;

	graphic_data->operate_tpye = graph_operate_change;
	memcpy(referee_tx_buf +pointer, graphic_data, sizeof(*graphic_data));
	pointer += sizeof(*graphic_data);

	memcpy(referee_tx_buf +pointer, new_char, LEN_data_char);
	pointer += LEN_data_char;

	Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart1, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);

}

