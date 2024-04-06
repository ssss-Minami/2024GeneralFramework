/*
 * @file		referee.c/h
 * @brief	裁判系统、图传串口中断
 * @history
 * 版本			作者			编写日期
 * v1.0.0		姚启杰		2023/4/1	
 */
#ifndef _REFEREE_H_
#define _REFEREE_H_

#define REFEREE_RXBUFF_SIZE 300
#define REFEREE_PICBUFF_SIZE 100
#define REFEREE_TXBUFF_SIZE 128
#define Offset_SOF_DataLength 1      //裁判系统数据帧偏移
#define Offset_CRC8 4
#define Offset_cmd_ID 5
#define Offset_data 7

#include "main.h"

typedef enum{
	referee_OK = 0,
	referee_solving = 1,
	referee_error = 2
}Referee_StatusTypeDef;
typedef enum
{
	ID_game_status  = 0x0001,
	ID_game_result  = 0x0002,
	ID_game_robot_hp  = 0x0003,//比赛机器人存活数据
	ID_game_dart_state = 0x0004, //飞镖发射状态
	ID_game_buff  =0x0005,//buff

	ID_event_data  = 0x0101,//场地事件数据
	ID_supply_projectile_action = 0x0102,//场地补给站动作标识数据
	ID_referee_warn = 0x0104,//裁判系统警告数据
	ID_dart_shoot_info =0x0105  , //飞镖发射

	ID_game_robot_state = 0x0201,//机器人状态数据
	ID_power_heat_data = 0x0202,//实时功率热量数据
	ID_game_robot_pos = 0x0203,//机器人位置数据
	ID_buff_musk = 0x0204,//机器人增益数据
	ID_aerial_robot_energy = 0x0205,//空中机器人能量状态数据
	ID_robot_hurt = 0x0206,//伤害状态数据
	ID_shoot_data = 0x0207,//实时射击数据
	ID_bullet_remaining  = 0x0208,//剩余发射数
	ID_rfid_status	= 0x0209,//机器人RFID状态，1Hz

	ID_dart_client_directive = 0x020A,//飞镖机器人客户端指令书, 10Hz

	ID_robot_interactive_header_data = 0x0301,//机器人交互数据，——发送方触发——发送 10Hz
	ID_controller_interactive_header_data = 0x0302,//自定义控制器交互数据接口，通过——客户端触发——发送 30Hz
	ID_map_interactive_header_data  = 0x0303,//客户端小地图交互数据，——触发发送——
	ID_keyboard_information = 0x0304//键盘、鼠标信息，通过——图传串口——发送

}CmdID;

typedef enum
{
	/* Std */
	LEN_FRAME_HEAD = 5,	// 帧头长度
	LEN_CMD_ID 	= 2,	// 命令码长度
	LEN_FRAME_TAIL = 2,	// 帧尾CRC16
	/* Ext */

	LEN_game_state =  11,	//0x0001
	LEN_game_result =  1,	//0x0002
	LEN_game_robot_hp =  32,	//0x0003  比赛机器人血量数据(v1.4中被修改)

	LEN_event_data  =  4,	//0x0101  场地事件数据
	LEN_supply_projectile_action =  4,	//0x0102场地补给站动作标识数据
	LEN_referee_warn =3, //裁判系统警告 0x0104
	LEN_dart_info =3  , //飞镖发射口倒计时

	LEN_game_robot_state = 13,	//0x0201机器人状态数据
	LEN_power_heat_data = 16,	//0x0202实时功率热量数据
	LEN_game_robot_pos = 16,	//0x0203机器人位置数据
	LEN_buff_musk =  6,	//0x0204机器人增益数据
	LEN_aerial_robot_energy  =  2,	//0x0205空中机器人能量状态数据
	LEN_robot_hurt =  1,	//0x0206伤害状态数据
	LEN_shoot_data =  7,	//0x0207	实时射击数据（此条目在串口协议v1.4中被修改）
	LEN_bullet_remaining = 6,//剩余发射数（此条目在串口协议v1.4中被修改）

	LEN_rfid_status	= 4,
	LEN_dart_client_directive = 6,//0x020A
	LEN_map_interactive_headerdata = 15,//0x20B
	LEN_keyboard_information = 12,//0x0304

}JudgeDataLength;

/* ID: 0x0001  Byte:  11    比赛状态数据 */
typedef struct
{
 uint8_t game_type : 4;
 uint8_t game_progress : 4;
 uint16_t stage_remain_time;
 uint64_t SyncTimeStamp;
}__attribute__((packed))game_status_t;

/* ID: 0x0002  Byte:  1    比赛结果数据 */
typedef struct
{
	uint8_t winner;
}__attribute__((packed)) ext_game_result_t;

/* ID: 0x0003  Byte:  32    比赛机器人血量数据 */
typedef struct
{
 uint16_t red_1_robot_HP;
 uint16_t red_2_robot_HP;
 uint16_t red_3_robot_HP;
 uint16_t red_4_robot_HP;
 uint16_t red_5_robot_HP;
 uint16_t red_7_robot_HP;
 uint16_t red_outpost_HP;
 uint16_t red_base_HP;
 uint16_t blue_1_robot_HP;
 uint16_t blue_2_robot_HP;
 uint16_t blue_3_robot_HP;
 uint16_t blue_4_robot_HP;
 uint16_t blue_5_robot_HP;
 uint16_t blue_7_robot_HP;
 uint16_t blue_outpost_HP;
 uint16_t blue_base_HP;
}__attribute__((packed)) game_robot_HP_t;

/* ID: 0x0101  Byte:  4    场地事件数据 */
typedef struct
{
	uint32_t event_data;
}__attribute__((packed)) ext_event_data_t;


/* ID: 0x0102  Byte:  4    场地补给站动作标识数据 */
typedef struct
{
	uint8_t reserved;
	uint8_t supply_robot_id;
	uint8_t supply_projectile_step;
	uint8_t supply_projectile_num;
}__attribute__((packed)) ext_supply_projectile_action_t;

/* ID: 0x0104  Byte: 2   裁判系统警告信息 */
typedef struct
{
  	uint8_t level;
	uint8_t offending_robot_id;
	uint8_t count;
}__attribute__((packed)) ext_referee_warning_t;

/* ID: 0x0105  Byte:1  飞镖发射口倒计时 */
typedef struct
{
	uint8_t dart_remaining_time;
 	uint16_t dart_info;
} __attribute__((packed)) dart_info_t;

/* ID: 0X0201  Byte: 27    机器人状态数据 */
typedef struct
{
	uint8_t robot_id;
	uint8_t robot_level;
	uint16_t current_HP;
	uint16_t maximum_HP;
	uint16_t shooter_barrel_cooling_value;
	uint16_t shooter_barrel_heat_limit;
	uint16_t chassis_power_limit;
	uint8_t power_management_gimbal_output : 1;
	uint8_t power_management_chassis_output : 1;
	uint8_t power_management_shooter_output : 1;
}__attribute__((packed)) ext_game_robot_state_t;

/* ID: 0X0202  Byte: 16    实时功率热量数据 */
typedef struct
{
	uint16_t chassis_voltage;
	uint16_t chassis_current;
	float chassis_power;
	uint16_t buffer_energy;
	uint16_t shooter_17mm_1_barrel_heat;
	uint16_t shooter_17mm_2_barrel_heat;
	uint16_t shooter_42mm_barrel_heat;
}__attribute__((packed)) ext_power_heat_data_t;

/* ID: 0x0203  Byte: 16    机器人位置数据 */
typedef struct
{
	float x;
	float y;
	float angle;
}__attribute__((packed)) ext_game_robot_pos_t;

/* ID: 0x0204  Byte:  1    机器人增益数据 */
typedef struct
{
	uint8_t recovery_buff;
	uint8_t cooling_buff;
	uint8_t defence_buff;
	uint8_t vulnerability_buff;
	uint16_t attack_buff;
}__attribute__((packed)) ext_buff_musk_t;

/* ID: 0x0205  Byte:  1    空中机器人能量状态数据 */
typedef struct
{
  	uint8_t airforce_status;
 	uint8_t time_remain;
}__attribute__((packed)) aerial_robot_energy_t;

/* ID: 0x0206  Byte:  1    伤害状态数据 */
typedef struct
{
	uint8_t armor_id : 4;
	uint8_t hurt_type : 4;
}__attribute__((packed)) ext_robot_hurt_t;

/* ID: 0x0207  Byte:  7    实时射击数据 */
typedef struct
{
	uint8_t bullet_type;
	uint8_t shooter_number;
	uint8_t launching_frequency;
	float initial_speed;
} __attribute__((packed)) ext_shoot_data_t;


/* ID: 0x0208  Byte:  6    子弹剩余数量 */
typedef struct
{
  uint16_t bullet_remaining_num_17mm;
  uint16_t bullet_remaining_num_42mm;
  uint16_t coin_remaining_num;//金币剩余
} __attribute__((packed)) ext_bullet_remaining_t;

/* ID: 0x0209  Byte:  4 	机器人RFID状态 */
typedef struct
{
	uint32_t rfid_status;
}__attribute__((packed)) ext_rfid_status_t;
/* ID: 0x020A  Byte:  6 */
typedef struct{
	uint8_t dart_launch_opening_status;
	uint8_t reserved;
	uint16_t target_change_time;
	uint16_t latest_launch_cmd_time;
} __attribute__((packed)) ext_dart_client_cmd_t; //LEN_DART_CLIENT_DIRECTIVE  表3-19


enum judge_robot_ID{
	hero_red       = 1,
	engineer_red   = 2,
	infantry3_red  = 3,
	infantry4_red  = 4,
	infantry5_red  = 5,
	plane_red      = 6,

	hero_blue      = 101,
	engineer_blue  = 102,
	infantry3_blue = 103,
	infantry4_blue = 104,
	infantry5_blue = 105,
	plane_blue     = 106,
};
typedef struct{
	uint16_t teammate_hero;
	uint16_t teammate_engineer;
	uint16_t teammate_infantry3;
	uint16_t teammate_infantry4;
	uint16_t teammate_infantry5;
	uint16_t teammate_plane;
	uint16_t teammate_sentry;

	uint16_t client_hero;
	uint16_t client_engineer;
	uint16_t client_infantry3;
	uint16_t client_infantry4;
	uint16_t client_infantry5;
	uint16_t client_plane;
} ext_interact_id_t;

typedef struct
{
	uint8_t data[113]; //数据段,n需要小于113
} __attribute__((packed))robot_interactive_data_t;


typedef struct{

	game_status_t 		Game_Status;				// 0x0001           比赛状态数据
	ext_game_result_t 		Game_Result;				// 0x0002         比赛结果数据
	game_robot_HP_t 	Game_Robot_HP;			// 0x0003         机器人血量数据

	ext_event_data_t			Event_Data;					// 0x0101         场地事件数据
	ext_supply_projectile_action_t	Supply_Projectile_Action;		// 0x0102 补给站动作标识
	ext_referee_warning_t		Referee_Warning;		// 0x0104         裁判警告信息
	dart_info_t	dart_info;// 0x0105         飞镖发射口倒计时

	ext_game_robot_state_t	Game_Robot_state;	// 0x0201         比赛机器人状态
	ext_power_heat_data_t	Power_Heat_Data;		// 0x0202         实时功率热量数据
	ext_game_robot_pos_t	Game_Robot_Pos;			// 0x0203         机器人位置
	ext_buff_musk_t		    Buff_Musk;						// 0x0204     机器人增益
	aerial_robot_energy_t	Aerial_Robot_Energy;// 0x0205             空中机器人能量状态
	ext_robot_hurt_t		Robot_Hurt;					// 0x0206         伤害状态
	ext_shoot_data_t		Shoot_Data;					// 0x0207         实时射击信息(射频  射速  子弹信息)
	ext_bullet_remaining_t	bullet_remaining;		// 0x0208	        子弹剩余发射数
	ext_rfid_status_t		rfid_status;				// 0x0209	        RFID信息
	ext_dart_client_cmd_t   dart_client;        // 0x020A         飞镖客户端

	ext_interact_id_t		ids;								//与本机交互的机器人id
	uint16_t                self_client;        //本机客户端

}Referee_InfoTypedef;

typedef struct
{
int16_t mouse_x;
int16_t mouse_y;
int16_t mouse_z;
int8_t left_button_down;
int8_t right_button_down;
uint16_t keyboard_value;
uint16_t reserved;
}__attribute__((packed))ext_robot_command_t;

void RefereeInit();
void RefereeSolve(uint8_t *data);

extern Referee_InfoTypedef refree_info;
extern uint8_t referee_tx_buf[REFEREE_TXBUFF_SIZE];
extern uint8_t referee_rx_buf[REFEREE_RXBUFF_SIZE];           //dma接收区
extern uint8_t referee_pic_rx_buf[REFEREE_PICBUFF_SIZE];
extern uint8_t UI_Seq;
extern uint8_t referee_rx_len;    //裁判系统串口idle中断接收数据长度
extern uint8_t referee_pic_rx_len;

#endif
