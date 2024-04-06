#ifndef __UI_H__
#define __UI_H__

#include "main.h"

typedef enum
{
	graph_operate_empty = 0,
	graph_operate_add = 1,
	graph_operate_change = 2,
	graph_operate_delete = 3,

	graph_type_line = 0,
	graph_type_rectangle = 1,
	graph_type_circle = 2,
	graph_type_oval = 3,
	graph_type_arc = 4,
	graph_type_float = 5,
	graph_type_int = 6,
	graph_type_char = 7,

	graph_color_R_or_B = 0,
	graph_color_yellow = 1,
	graph_color_green = 2,
	graph_color_orange = 3,
	graph_color_purple = 4,
	graph_color_pink = 5,
	graph_color_cyan = 6,
	graph_color_black = 7,
	graph_color_white = 8,

	graph_LineWidth_default = 3,
	graph_FrontSize_default = 25,

	LEN_graph_TxHeader = 13,
	LEN_graph_ID = 6,
	LEN_data_delete = 2,
	LEN_data_draw1 = 15,
    LEN_data_draw2 = 30,
    LEN_data_draw5 = 75,
	LEN_data_char = 30,

}graph_e;

typedef struct
{
	uint8_t SOF;
	uint16_t data_length;
	uint8_t seq;
	uint8_t CRC8;
	uint16_t Cmd_id;
	uint16_t Data_id;
	uint16_t Tx_id;
	uint16_t Rx_id;
}__attribute__((packed))graphic_txHeader_t;

typedef struct
{
	uint8_t graphic_name[3];
	uint32_t operate_type:3;
	uint32_t graphic_type:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t details_a:9;
	uint32_t details_b:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11;
	uint32_t details_c:10;
	uint32_t details_d:11;
	uint32_t details_e:11;
}__attribute__((packed))graphic_data_t;

void UIInit();
void UITest();
void UIRefresh(float pitch, float cap, uint8_t aim_state, uint8_t cover_state);
void UIClear();

#endif