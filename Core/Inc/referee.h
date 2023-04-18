/*
 * @file		referee.c/h
 * @brief	裁判系统、图传串口中断
 * @history
 * 版本			作者			编写日期
 * v1.0.0		姚启杰		2023/4/1
 *
 */
#ifndef _REFEREE_H_
#define _REFEREE_H_

#define referee_buf_size 300
#define referee_pic_buf_size 100
#define referee_tx_buf_size 128
#define Offset_SOF_DataLength 1      //裁判系统数据帧偏移
#define Offset_CRC8 4
#define Offset_cmd_ID 5
#define Offset_data 7

extern uint8_t referee_rx_len;
extern uint8_t referee_pic_rx_len;
extern uint8_t referee_rx_buf[referee_buf_size];
extern uint8_t referee_pic_rx_buf[referee_pic_buf_size];
extern uint8_t referee_tx_buf[referee_tx_buf_size];

void referee_solve(uint8_t *data);
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
	ID_dart_shoot_time =0x0105  , //飞镖发射口倒计时

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
	LEN_game_dart_state =3  , //0X0004飞镖发射状态
	LEN_game_buff =11 , //0X0005

	LEN_event_data  =  4,	//0x0101  场地事件数据
	LEN_supply_projectile_action =  4,	//0x0102场地补给站动作标识数据
	LEN_referee_warn =2, //裁判系统警告 0x0104
	LEN_dart_remaining_time =1  , //飞镖发射口倒计时

	LEN_game_robot_state = 27,	//0x0201机器人状态数据
	LEN_power_heat_data = 16,	//0x0202实时功率热量数据
	LEN_game_robot_pos = 16,	//0x0203机器人位置数据
	LEN_buff_musk =  1,	//0x0204机器人增益数据
	LEN_aerial_robot_energy  =  1,	//0x0205空中机器人能量状态数据
	LEN_robot_hurt =  1,	//0x0206伤害状态数据
	LEN_shoot_data =  7,	//0x0207	实时射击数据（此条目在串口协议v1.4中被修改）
	LEN_bullet_remaining = 6,//剩余发射数（此条目在串口协议v1.4中被修改）

	LEN_rfid_status	= 4,
	LEN_dart_client_directive = 12,//0x020A
	LEN_map_interactive_headerdata = 15,
	LEN_keyboard_information = 12,//0x0304

}JudgeDataLength;

/* ID: 0x0001  Byte:  11    比赛状态数据 */
typedef struct
{
	uint8_t game_type : 4;
	uint8_t game_progress : 4;
	uint16_t stage_remain_time;
  uint64_t SyncTimeStamp;
}__attribute__((packed)) ext_game_status_t;

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
//	uint16_t red_6_robot_HP;  //协议v1.4中此条目被删除
	uint16_t red_7_robot_HP;
	uint16_t red_outpost_HP;
  uint16_t red_base_HP;

	uint16_t blue_1_robot_HP;
	uint16_t blue_2_robot_HP;
	uint16_t blue_3_robot_HP;
	uint16_t blue_4_robot_HP;
	uint16_t blue_5_robot_HP;
//	uint16_t blue_6_robot_HP;  //协议v1.4中此条目被删除
	uint16_t blue_7_robot_HP;

	uint16_t blue_outpost_HP;
  uint16_t blue_base_HP;
}__attribute__((packed)) ext_game_robot_HP_t;

/* ID: 0x0004  Byte:  3    飞镖发射状态 */
typedef struct
{
	uint8_t dart_belong;
	uint16_t stage_remaining_time;
}__attribute__((packed)) ext_dart_status_t;

/* ID: 0x0005  Byte:  11    buff */
typedef struct
{
	uint8_t F1_zone_status:1;
	uint8_t F1_zone_buff_debuff_status:3;

	uint8_t F2_zone_status:1;
	uint8_t F2_zone_buff_debuff_status:3;

	uint8_t F3_zone_status:1;
	uint8_t F3_zone_buff_debuff_status:3;

	uint8_t F4_zone_status:1;
	uint8_t F4_zone_buff_debuff_status:3;

	uint8_t F5_zone_status:1;
	uint8_t F5_zone_buff_debuff_status:3;

	uint8_t F6_zone_status:1;
	uint8_t F6_zone_buff_debuff_status:3;

  uint16_t red1_bullet_left;

  uint16_t red2_bullet_left;

  uint16_t blue1_bullet_left;

  uint16_t blue2_bullet_left;

}__attribute__((packed)) ext_ICRA_buff_debuff_zone_status_t;

/* ID: 0x0101  Byte:  4    场地事件数据 */
typedef struct
{
	uint32_t event_type;
}__attribute__((packed)) ext_event_data_t;


/* ID: 0x0102  Byte:  4    场地补给站动作标识数据 */
typedef struct
{
	uint8_t supply_projectile_id;
	uint8_t supply_robot_id;
	uint8_t supply_projectile_step;
  uint8_t supply_projectile_num;
}__attribute__((packed)) ext_supply_projectile_action_t;

/* ID: 0x0104  Byte: 2   裁判系统警告信息 */
typedef struct
{
  uint8_t level;
	uint8_t foul_robot_id;
}__attribute__((packed)) ext_referee_warning_t;

/* ID: 0x0105  Byte:1  飞镖发射口倒计时 */
typedef struct
{
	uint8_t dart_remaining_time;
} __attribute__((packed)) ext_dart_remaining_time_t;

/* ID: 0X0201  Byte: 27    机器人状态数据 */
typedef struct
{
	uint8_t robot_id;   //机器人ID，可用来校验发送
	uint8_t robot_level;  //1一级，2二级，3三级
	uint16_t remain_HP;  //机器人剩余血量
	uint16_t max_HP; //机器人满血量

  uint16_t shooter1_17mm_cooling_rate;  //机器人 17mm 子弹热量冷却速度 单位 /s
  uint16_t shooter1_17mm_cooling_limit;   // 机器人 17mm 子弹热量上限
  uint16_t shooter1_17mm_speed_limit;


  uint16_t shooter2_17mm_cooling_rate;
  uint16_t shooter2_17mm_cooling_limit;
  uint16_t shooter2_17mm_speed_limit;


  uint16_t shooter_42mm_cooling_rate;
  uint16_t shooter_42mm_cooling_limit;
  uint16_t shooter_42mm_speed_limit;


	uint16_t max_chassis_power;
	uint8_t mains_power_gimbal_output : 1;
	uint8_t mains_power_chassis_output : 1;
	uint8_t mains_power_shooter_output : 1;
}__attribute__((packed)) ext_game_robot_state_t;

/* ID: 0X0202  Byte: 16    实时功率热量数据 */
typedef struct
{
	uint16_t chassis_volt;
	uint16_t chassis_current;
	float chassis_power;   //瞬时功率
	uint16_t chassis_power_buffer;//60焦耳缓冲能量
	uint16_t shooter_heat0;//17mm
	uint16_t shooter_heat1;
	uint16_t mobile_shooter_heat2;
}__attribute__((packed)) ext_power_heat_data_t;

/* ID: 0x0203  Byte: 16    机器人位置数据 */
typedef struct
{
	float x;
	float y;
	float z;
	float yaw;
}__attribute__((packed)) ext_game_robot_pos_t;

/* ID: 0x0204  Byte:  1    机器人增益数据 */
typedef struct
{
	uint8_t power_rune_buff;
}__attribute__((packed)) ext_buff_musk_t;

/* ID: 0x0205  Byte:  1    空中机器人能量状态数据 */
typedef struct
{
  uint8_t attack_time;
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
  uint8_t shooter_id;  //发射机构是17还是42
	uint8_t bullet_freq;
	float bullet_speed;
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

typedef struct{
	uint8_t dart_launch_opening_status;//当前飞镖发射口的状态
	uint8_t dart_attack_target;        //飞镖的打击目标，默认为前哨站（1：前哨站，2：基地）
	uint16_t target_change_time;       //切换打击目标时的比赛剩余时间
	uint8_t first_dart_speed;          //检测到的第一枚飞镖速度，单位 0.1m/s/LSB
	uint8_t second_dart_speed;         //检测到的第二枚飞镖速度，单位 0.1m/s/LSB
	uint8_t third_dart_speed;          //检测到的第三枚飞镖速度，单位 0.1m/s/LSB
	uint8_t fourth_dart_speed;         //检测到的第四枚飞镖速度，单位 0.1m/s/LSB
	uint16_t last_dart_launch_time;    //最近一次的发射飞镖的比赛剩余时间，单位秒
	uint16_t operate_launch_cmd_time;  //最近一次操作手确定发射指令时的比赛剩余时间，单位秒
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

	ext_game_status_t 		Game_Status;				// 0x0001           比赛状态数据
	ext_game_result_t 		Game_Result;				// 0x0002         比赛结果数据
	ext_game_robot_HP_t 	Game_Robot_HP;			// 0x0003         机器人血量数据
	ext_dart_status_t		Game_Dart_status;				// 0x0004         飞镖发射状态
	ext_ICRA_buff_debuff_zone_status_t	Game_ICRA_buff;      //                人工智能挑战赛加成与惩罚区状态

	ext_event_data_t			Event_Data;					// 0x0101         场地事件数据
	ext_supply_projectile_action_t	Supply_Projectile_Action;		// 0x0102 补给站动作标识
	ext_referee_warning_t		Referee_Warning;		// 0x0104         裁判警告信息
	ext_dart_remaining_time_t	dart_remaining_time;// 0x0105         飞镖发射口倒计时

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
}__attribute__((packed))graphic_TxHeader_Typedef;

typedef struct
{
	uint8_t graphic_name[3];
	uint32_t operate_tpye:3;
	uint32_t graphic_tpye:3;
	uint32_t layer:4;
	uint32_t color:4;
	uint32_t start_angle:9;
	uint32_t end_angle:9;
	uint32_t width:10;
	uint32_t start_x:11;
	uint32_t start_y:11;
	uint32_t radius:10;
	uint32_t end_x:11;
	uint32_t end_y:11;
}__attribute__((packed))graphic_data_struct_t;

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
	LEN_data_char = 30,

}graph_config;

extern Referee_InfoTypedef Ref_Info;
extern graphic_data_struct_t const_char, aim_char, chassis_char, droppoint_rectangle;
extern graphic_TxHeader_Typedef graphic_TxHeader;

extern void UI_Print_char(graphic_data_struct_t *graphic_data, uint8_t *char_to_send, uint8_t ui_color, uint32_t x, uint32_t y);
extern void UI_Print_rectangle(graphic_data_struct_t *graphic_data, uint8_t ui_color, uint8_t ui_layer, uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y);
extern void UI_Clear_layer(uint8_t ui_layer);
extern void UI_Refresh_graph(graphic_data_struct_t *graphic_data, uint32_t new_y);
extern void UI_Refresh_char(graphic_data_struct_t *graphic_data, uint8_t *new_char);

#endif
