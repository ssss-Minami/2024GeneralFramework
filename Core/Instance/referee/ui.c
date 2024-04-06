#include "ui.h"
#include "referee.h"
#include "../Core/Instance/CRC/CRC.h"
#include "string.h"
#include "usart.h"

#define TEXT_COVER 0
#define TEXT_AIM 1
#define LINE_CAP 2
#define LINE_SIGHT_VER 3
#define LINE_SIGHT_HOR 4
#define REC_CAP 5

#define CHARLIST_COVEROPEN 0
#define CHARLIST_COVERCLOSE 1
#define CHARLIST_AIMSEARCH 2
#define CHARLIST_AIMLOCKED 3

#define LAYER_DEFAULT 0


uint8_t char_list[][LEN_data_char] = {"Cover Opened", "Cover Closed", "SEARCHING", "LOCKED"};
graphic_data_t graph_list[6];

/*
 * @brief  	直接向操作手界面绘制文字
 * @param	图形结构体指针
 * @param   要绘制的字符串的指针
 * @retval 	无
 * @note    调用此函数将清空发送缓冲区
 */
void UI_Print_Char(graphic_data_t *graphic_data, uint8_t *str, uint8_t *buff)
{
    if(graphic_data->graphic_type != graph_type_char)
        return;
    memset(buff, 0, REFEREE_TXBUFF_SIZE);
	uint8_t pointer = 0;
    static graphic_txHeader_t graphic_TxHeader;
	
    graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw1 +LEN_data_char;
	Append_CRC8_Check_Sum((uint8_t*)&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0110;       //draw single char
	graphic_TxHeader.Tx_id = refree_info.Game_Robot_state.robot_id;
	graphic_TxHeader.Rx_id = refree_info.Game_Robot_state.robot_id + 0x100;  //receive_end client id	
	memcpy(buff + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;

	graphic_data->operate_type = graph_operate_add;

	memcpy(buff +pointer, graphic_data, sizeof(*graphic_data));
	pointer += sizeof(*graphic_data);

	memcpy(buff +pointer, str, LEN_data_char);
	pointer += LEN_data_char;

	Ref_Append_CRC16_Check_Sum(buff, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart6, buff, pointer +LEN_FRAME_TAIL, 300);
}

/*
 * @brief  	新增图形至发送缓冲区
 * @param	图形结构体指针
 * @param   缓冲区中此图形数据段的首地址指针
 * @retval 	无
 *
 */
void UI_Add_Graph(graphic_data_t *graphic_data, uint8_t *buff)
{
    if(graphic_data->graphic_type == graph_type_char)
        return;
	memcpy(buff, graphic_data, sizeof(*graphic_data));
}

void UIClear()
{
	memset(referee_tx_buf, 0, REFEREE_TXBUFF_SIZE);
	uint8_t temp_arr[2];
	uint8_t pointer = 0;
    graphic_txHeader_t graphic_TxHeader;
	graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_delete;
	Append_CRC8_Check_Sum((uint8_t*)&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0100;       //clear graph
	if(graphic_TxHeader.Tx_id == 0 || graphic_TxHeader.Rx_id == 0)
		{
			graphic_TxHeader.Tx_id = refree_info.Game_Robot_state.robot_id;
			graphic_TxHeader.Rx_id = refree_info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
		}
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;

	temp_arr[0] = 2;
	temp_arr[1] = 0;
	memcpy(referee_tx_buf +pointer, temp_arr, LEN_data_delete);
	pointer += LEN_data_delete;

	Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart1, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);
}

//TODO:将红蓝方来源换成更可靠的
void UIInit()
{
    UIClear();
    for(int i=0;i<6;i++)
    {
        graph_list[i].graphic_name[2] = i+1;
        graph_list[i].operate_type = graph_operate_add;
        graph_list[i].layer = LAYER_DEFAULT;
    }
    graph_list[TEXT_COVER].graphic_type = graph_type_char;
    graph_list[TEXT_COVER].color = graph_color_white;
    graph_list[TEXT_COVER].details_a = 20;  //frontsize
    graph_list[TEXT_COVER].details_b = 12;  //length of string
    graph_list[TEXT_COVER].start_x = 102;
    graph_list[TEXT_COVER].start_y = 811;
    graph_list[TEXT_COVER].width = 2;

    graph_list[TEXT_AIM].graphic_type = graph_type_char;
    graph_list[TEXT_AIM].color = graph_color_white;
    graph_list[TEXT_AIM].details_a = 60;  //frontsize
    graph_list[TEXT_AIM].details_b = 6;  //length of string
    graph_list[TEXT_AIM].start_x = 908;
    graph_list[TEXT_AIM].start_y = 105;
    graph_list[TEXT_AIM].width = 9;

    graph_list[LINE_CAP].graphic_type = graph_type_line;
    graph_list[LINE_CAP].color = graph_color_yellow;
    graph_list[LINE_CAP].details_a = 0;  
    graph_list[LINE_CAP].details_b = 0;  
    graph_list[LINE_CAP].start_x = 559;
    graph_list[LINE_CAP].start_y = 83;
    graph_list[LINE_CAP].details_d = 559;  
    graph_list[LINE_CAP].details_e = 280; 
    graph_list[LINE_CAP].width = 28;

    graph_list[LINE_SIGHT_VER].graphic_type = graph_type_line;
    graph_list[LINE_SIGHT_VER].color = graph_color_green;
    graph_list[LINE_SIGHT_VER].details_a = 0;  
    graph_list[LINE_SIGHT_VER].details_b = 0;  
    graph_list[LINE_SIGHT_VER].start_x = 960;
    graph_list[LINE_SIGHT_VER].start_y = 428;
    graph_list[LINE_SIGHT_VER].details_d = 960;  
    graph_list[LINE_SIGHT_VER].details_e = 491; 
    graph_list[LINE_SIGHT_VER].width = 5;

    graph_list[LINE_SIGHT_HOR].graphic_type = graph_type_line;
    graph_list[LINE_SIGHT_HOR].color = graph_color_green;
    graph_list[LINE_SIGHT_HOR].details_a = 0;  
    graph_list[LINE_SIGHT_HOR].details_b = 0;  
    graph_list[LINE_SIGHT_HOR].start_x = 913;
    graph_list[LINE_SIGHT_HOR].start_y = 456;
    graph_list[LINE_SIGHT_HOR].details_d = 1010;  
    graph_list[LINE_SIGHT_HOR].details_e = 456; 
    graph_list[LINE_SIGHT_HOR].width = 5;

    graph_list[REC_CAP].graphic_type = graph_type_rectangle;
    graph_list[REC_CAP].color = graph_color_R_or_B;
    graph_list[REC_CAP].details_a = 0;  
    graph_list[REC_CAP].details_b = 0;  
    graph_list[REC_CAP].start_x = 559;
    graph_list[REC_CAP].start_y = 83;
    graph_list[REC_CAP].details_d = 559+28;  
    graph_list[REC_CAP].details_e = 83+257; 
    graph_list[REC_CAP].width = 5;

    UI_Print_Char(&graph_list[TEXT_COVER], char_list[CHARLIST_COVERCLOSE], referee_tx_buf);
    osDelay(120);
    UI_Print_Char(&graph_list[TEXT_AIM], char_list[CHARLIST_AIMSEARCH], referee_tx_buf);
    osDelay(120);
    
    memset(referee_tx_buf, 0, REFEREE_TXBUFF_SIZE);
	uint8_t pointer = 0;
    graphic_txHeader_t graphic_TxHeader;
	graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw5;
	Append_CRC8_Check_Sum((uint8_t*)&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0103;       //draw 5
	graphic_TxHeader.Tx_id = refree_info.Game_Robot_state.robot_id;
	graphic_TxHeader.Rx_id = refree_info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;
    
    UI_Add_Graph(&graph_list[LINE_CAP], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[LINE_SIGHT_HOR], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[LINE_SIGHT_VER], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[REC_CAP], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    graph_list[REC_CAP].operate_type = graph_operate_empty;
    UI_Add_Graph(&graph_list[REC_CAP], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);

    Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart6, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);
    graph_list[LINE_CAP].operate_type = graph_operate_change;
    graph_list[LINE_SIGHT_HOR].operate_type = graph_operate_change;
    graph_list[LINE_SIGHT_VER].operate_type = graph_operate_change;
    graph_list[TEXT_AIM].operate_type = graph_operate_change;
    graph_list[TEXT_COVER].operate_type = graph_operate_change;    
}

uint32_t SolveDroppoint(float pitch)
{
    //test
    return (uint32_t)pitch;
}

/*
 * @brief  	刷新UI
 * @param	pitch轴角度，弧度制
 * @param   超电剩余容量，0~1
 * @param   自瞄状态，1=锁
 * @param   弹舱盖状态，1=open
 * @retval 	无
 * @note    上行频率被限制至10hz
 * @todo    PITCH轴角度值落点映射
 */
void UIRefresh(float pitch, float cap, uint8_t aim_state, uint8_t cover_state)
{
    uint8_t static aim_last_state, cover_last_state;
    if(aim_last_state != aim_state)
    {
    	UI_Print_Char(&graph_list[TEXT_AIM], (aim_state==1 ? char_list[CHARLIST_AIMLOCKED] : char_list[CHARLIST_AIMSEARCH]), referee_tx_buf);
    	osDelay(120);
    }
    aim_last_state = aim_state;
    if(cover_last_state != cover_state)
    {
    	UI_Print_Char(&graph_list[TEXT_COVER], (cover_state==1 ? char_list[CHARLIST_COVEROPEN] : char_list[CHARLIST_COVERCLOSE]), referee_tx_buf);
    	osDelay(120);
    }
    cover_last_state = cover_state;

    uint32_t locate = SolveDroppoint(pitch);
    graph_list[LINE_SIGHT_HOR].start_y = locate;
    graph_list[LINE_SIGHT_HOR].details_e = locate;
    graph_list[LINE_SIGHT_VER].start_y = locate-30;
    graph_list[LINE_SIGHT_VER].details_e = locate+35;
    graph_list[LINE_CAP].details_e = 83 + cap*256;


    memset(referee_tx_buf, 0, REFEREE_TXBUFF_SIZE);
	uint8_t pointer = 0;
    graphic_txHeader_t graphic_TxHeader;
	graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw5;
	Append_CRC8_Check_Sum((uint8_t*)&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0103;       //draw 5
	graphic_TxHeader.Tx_id = refree_info.Game_Robot_state.robot_id;
	graphic_TxHeader.Rx_id = refree_info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;
    
    UI_Add_Graph(&graph_list[LINE_CAP], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[LINE_SIGHT_HOR], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[LINE_SIGHT_VER], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[REC_CAP], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[REC_CAP], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);

    Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart6, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);

}


void UITest(void)
{
    UIClear();
    for(int i=0;i<6;i++)
    {
        graph_list[i].graphic_name[2] = i+1;
        graph_list[i].operate_type = graph_operate_add;
        graph_list[i].layer = LAYER_DEFAULT;
    }
    graph_list[TEXT_COVER].graphic_type = graph_type_char;
    graph_list[TEXT_COVER].color = graph_color_white;
    graph_list[TEXT_COVER].details_a = 20;  //frontsize
    graph_list[TEXT_COVER].details_b = 12;  //length of string
    graph_list[TEXT_COVER].start_x = 102;
    graph_list[TEXT_COVER].start_y = 811;
    graph_list[TEXT_COVER].width = 2;

    graph_list[1].graphic_type = graph_type_line;
    graph_list[1].color = graph_color_black;
    graph_list[1].details_a = 0;  
    graph_list[1].details_b = 0;  
    graph_list[1].start_x = 913;
    graph_list[1].start_y = 250;
    graph_list[1].details_d = 1010;  
    graph_list[1].details_e = 250; 
    graph_list[1].width = 5;

    graph_list[LINE_CAP].graphic_type = graph_type_line;
    graph_list[LINE_CAP].color = graph_color_cyan;
    graph_list[LINE_CAP].details_a = 0;  
    graph_list[LINE_CAP].details_b = 0;  
    graph_list[LINE_CAP].start_x = 913;
    graph_list[LINE_CAP].start_y = 300;
    graph_list[LINE_CAP].details_d = 1010;  
    graph_list[LINE_CAP].details_e = 300; 
    graph_list[LINE_CAP].width = 28;

    graph_list[LINE_SIGHT_VER].graphic_type = graph_type_line;
    graph_list[LINE_SIGHT_VER].color = graph_color_green;
    graph_list[LINE_SIGHT_VER].details_a = 0;  
    graph_list[LINE_SIGHT_VER].details_b = 0;  
    graph_list[LINE_SIGHT_VER].start_x = 913;
    graph_list[LINE_SIGHT_VER].start_y = 350;
    graph_list[LINE_SIGHT_VER].details_d = 1010;  
    graph_list[LINE_SIGHT_VER].details_e = 350; 
    graph_list[LINE_SIGHT_VER].width = 5;

    graph_list[LINE_SIGHT_HOR].graphic_type = graph_type_line;
    graph_list[LINE_SIGHT_HOR].color = graph_color_yellow;
    graph_list[LINE_SIGHT_HOR].details_a = 0;  
    graph_list[LINE_SIGHT_HOR].details_b = 0;  
    graph_list[LINE_SIGHT_HOR].start_x = 913;
    graph_list[LINE_SIGHT_HOR].start_y = 400;
    graph_list[LINE_SIGHT_HOR].details_d = 1010;  
    graph_list[LINE_SIGHT_HOR].details_e = 400; 
    graph_list[LINE_SIGHT_HOR].width = 5;

    graph_list[5].graphic_type = graph_type_line;
    graph_list[5].color = graph_color_R_or_B;
    graph_list[5].details_a = 0;  
    graph_list[5].details_b = 0;  
    graph_list[5].start_x = 913;
    graph_list[5].start_y = 425;
    graph_list[5].details_d = 1010;  
    graph_list[5].details_e = 425; 
    graph_list[5].width = 5;
    
    memset(referee_tx_buf, 0, REFEREE_TXBUFF_SIZE);
	uint8_t pointer = 0;
    graphic_txHeader_t graphic_TxHeader;
	graphic_TxHeader.SOF = 0xA5;
	graphic_TxHeader.seq = UI_Seq++;
	graphic_TxHeader.data_length = LEN_graph_ID + LEN_data_draw5;
	Append_CRC8_Check_Sum((uint8_t*)&graphic_TxHeader, 5);
	graphic_TxHeader.Cmd_id = 0x0301;
	graphic_TxHeader.Data_id = 0x0103;       //draw 5
	graphic_TxHeader.Tx_id = refree_info.Game_Robot_state.robot_id;
	graphic_TxHeader.Rx_id = refree_info.Game_Robot_state.robot_id + 0x100;  //receive_end client id
	memcpy(referee_tx_buf + pointer, &graphic_TxHeader, LEN_graph_TxHeader);
	pointer += LEN_graph_TxHeader;
    
    UI_Add_Graph(&graph_list[1], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[2], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[3], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[4], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);
    UI_Add_Graph(&graph_list[5], referee_tx_buf +pointer);
    pointer += sizeof(graphic_data_t);

    Ref_Append_CRC16_Check_Sum(referee_tx_buf, pointer + LEN_FRAME_TAIL);
	HAL_UART_Transmit(&huart6, referee_tx_buf, pointer +LEN_FRAME_TAIL, 300);   
}