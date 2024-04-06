/*
 * @file		referee.c/h
 * @brief	裁判系统、图传串口中断
 * @history
 * 	版本			作者			编写日期			内容
 * 	v1.0		姚启杰		2023/4/1		裁判系统、图传链路通信
 *	v1.1		郑煜壅		2023/11/3		更改部分代码风格
 *	V2.0		YQJ			2024/4/1		无需再注释库函数
 */
#include "main.h"
#include "usart.h"
#include "dma.h"
#include "string.h"
#include "referee.h"
#include "../Core/Instance/remote/remote.h"
#include "../Core/Instance/CRC/CRC.h"
uint8_t referee_rx_len;    //裁判系统串口idle中断接收数据长度
uint8_t referee_pic_rx_len;
uint8_t referee_rx_buf[REFEREE_RXBUFF_SIZE];           //dma接收区
uint8_t referee_pic_rx_buf[REFEREE_PICBUFF_SIZE];
uint8_t referee_tx_buf[REFEREE_TXBUFF_SIZE];                        //maximum 128 bytes
Referee_InfoTypedef refree_info;             //裁判系统数据
uint8_t UI_Seq = 0;

void RefereeSolve(uint8_t *data);
void RefereeInit()
{
//	HAL_DMA_RegisterCallback(&hdma_usart6_rx, HAL_DMA_XFER_CPLT_CB_ID, RefereeSolve);
//	HAL_DMA_RegisterCallback(&hdma_usart1_rx, HAL_DMA_XFER_CPLT_CB_ID, RefereeSolve);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart6, referee_rx_buf, REFEREE_RXBUFF_SIZE);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, referee_pic_rx_buf, REFEREE_PICBUFF_SIZE);

}

void RefereeSolve(uint8_t *data)
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
		memcpy(&(refree_info.Game_Status), (data + Offset_data), LEN_game_state);
		break;
	case ID_game_result:
		memcpy(&(refree_info.Game_Result), (data + Offset_data), LEN_game_result);
		break;
	case ID_game_robot_hp:
		memcpy(&(refree_info.Game_Robot_HP), (data + Offset_data), LEN_game_robot_hp); //v1.4中有冲突，按sof中为准
		break;

	//0x100
	case ID_event_data:
			memcpy(&(refree_info.Event_Data), (data + Offset_data), LEN_event_data);
			break;
	case ID_supply_projectile_action:
			memcpy(&(refree_info.Supply_Projectile_Action), (data + Offset_data), LEN_supply_projectile_action);
			break;
	case ID_referee_warn:
			memcpy(&(refree_info.Referee_Warning), (data + Offset_data), LEN_referee_warn);
			break;
	case ID_dart_shoot_info:
			memcpy(&(refree_info.dart_info), (data + Offset_data), LEN_dart_info);
			break;

	//0x200
	case ID_game_robot_state:
			memcpy(&(refree_info.Game_Robot_state), (data + Offset_data), LEN_game_robot_state);
			break;
	case ID_power_heat_data:
			memcpy(&(refree_info.Power_Heat_Data), (data + Offset_data), LEN_power_heat_data);
			break;
	case ID_game_robot_pos:
			memcpy(&(refree_info.Game_Robot_Pos), (data + Offset_data), LEN_game_robot_pos);
			break;
	case ID_buff_musk:
			memcpy(&(refree_info.Buff_Musk), (data + Offset_data), LEN_buff_musk);
			break;
	case ID_aerial_robot_energy:
			memcpy(&(refree_info.Aerial_Robot_Energy), (data + Offset_data), LEN_aerial_robot_energy);
			break;
	case ID_robot_hurt:
			memcpy(&(refree_info.Game_Status), (data + Offset_data), LEN_robot_hurt);
			break;
	case ID_shoot_data:
			memcpy(&(refree_info.Shoot_Data), (data + Offset_data), LEN_shoot_data);//v1.4中冲突，以sof中为准
			break;
	case ID_bullet_remaining:
			memcpy(&(refree_info.bullet_remaining), (data + Offset_data), LEN_bullet_remaining);//v1.4中冲突，以sof中为准
			break;
	case ID_rfid_status:
			memcpy(&(refree_info.rfid_status), (data + Offset_data), LEN_rfid_status);
			break;
	case ID_dart_client_directive:
			memcpy(&(refree_info.dart_client), (data + Offset_data), LEN_dart_client_directive);
			break;

	//0x300
	case ID_keyboard_information:
			memcpy(&(RC_Ctl.keyboard), (data + Offset_data), LEN_keyboard_information);
			break;
	}

	if(data[offset_frame_tail+2] == 0xA5)
		RefereeSolve(data +offset_frame_tail +2);
	return;

}









