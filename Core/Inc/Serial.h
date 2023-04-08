/*
 * @file		Serial.c/h
 * @brief		usb-cdc通信程序，用于上位机和stm32的通信，需配合上位机代码使用
 * @history
 * 版本			作者			编写日期
 * v1.0.0		许金帅		2023/4/1
 *
 */
#ifndef __Serial_h__
#define __Serial_h__
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"

//这是为了在结构体里定义bool类型而写的
typedef enum {FALSE = 0, TRUE = !FALSE} bool;

//下方是结构体的具体定义，解包和打包都与其有关
typedef struct FeedBack{
	uint8_t Shoot_Mode; //发射方式
	uint8_t Shoot_Speed; //子弹发射速度
	uint8_t Armor_Id; //被击打装甲板ID
	uint16_t HP_Remain; //剩余血量
}_FeedBack,*_FeedBackInfo;
typedef struct ControlData_Chassis _controldata_chassis;
typedef struct ControlData_Chassis{
	uint8_t y_Speed; //纵轴方向速度
	uint8_t x_Speed; //横轴方向速度
	uint8_t rotational_speed; //小车旋转速度
	uint8_t chassis_state; //底盘状态
}*_controldata_chassisInfo;
typedef struct ControlData_Pan _controldata_pan;
typedef struct ControlData_Pan{
	uint16_t pitch_angle; //P轴角度
	uint16_t yaw_angle; //Y轴角度
	uint8_t shoot_mode; //发射方式
	uint8_t shoot_speed; //发射速度
}*_controldata_panInfo;
//下面是为和ROS2上位机沟通而定制的结构体
typedef struct ReceivePacket _receive_packet;
typedef struct ReceivePacket{
	uint8_t header;
	uint8_t tracking;
	float x;
	float y;
	float z;
	float yaw;
	float vx;
	float vy;
	float vz;
	float v_yaw;
	float r1;
	float r2;
	float z_2;
	uint16_t checksum;
}__attribute__((packed))_receivepacket,*_receive_packetinfo;

typedef struct SendPacket
{
	uint8_t header;
	uint8_t robot_color : 1;
	uint8_t task_mode : 2;
	uint8_t reserve : 5;
	float pitch;
	float yaw;
	float aim_x;
	float aim_y;
	float aim_z;
	uint16_t checksum;
}_sendpacket,*_send_packetinfo;


//typedef struct SendPacket
//{
//	uint8_t header;
//	uint8_t robot_color : 1;
//	uint8_t task_mode : 2;
//	uint8_t reserve : 5;
//	float pitch;
//	float yaw;
//	float aim_x;
//	float aim_y;
//	float aim_z;
//	uint16_t checksum;
//
//}_sendpacket,*_send_packetinfo;

//


//下方是枚举所有结构体的可能选项，便于发送数据以及解析数据
/*
 * @Brief: 发射方式
 */
typedef enum
{
	NO_Fire     = (uint8_t)(0X00), //不发射
	SINGLE_FIRE = (uint8_t)(0X01), //点射
	BURST_FIRE  = (uint8_t)(0X02)  //连发
}ShootMode;

/*
 * @Brief: 发射速度
 */
typedef enum
{
    HIGH_SPEED      =   (uint16_t)(0x00),   //高速
    LOW_SPEED       =   (uint16_t)(0x01)    //低速
}ShootSpeed;

/*
 * @Brief: 底盘状态
 */
typedef enum
{
	FOLLOW    =   (uint8_t)(0X00),  //底盘跟随
	ROTATE    =   (uint8_t)(0X01)   //底盘摆动
}ChassisState;




int CDC_SendFeed(uint8_t* Fed, uint16_t Len);//CDC发送反馈数据的函数
int CDC_Receive(uint8_t* Buf, uint16_t Len,_controldata_chassisInfo concha,_controldata_panInfo conpan);//CDC接收控制数据的函数
void Pack_Data(_FeedBack* feedback,uint8_t* feedArray);//打包反馈的信息
//ROS2发送代码
void Pack_And_Send_Data_ROS2(_send_packetinfo sendinfo,uint8_t* TempArray,uint8_t* sendData,uint16_t Len);//ROS2下的打包以及发送函数
int CDC_Receive_ROS2(uint8_t* Buf, uint16_t Len,_receive_packetinfo packinfo);//ROS2下的接收函数
uint16_t Get_CRC16_Check_Sum( _send_packetinfo sendinfo,uint8_t* TempArray,uint8_t* pchMessage,uint32_t dwLength, uint16_t wCRC);//校验码生成
extern _receive_packetinfo receinfo;
#endif
