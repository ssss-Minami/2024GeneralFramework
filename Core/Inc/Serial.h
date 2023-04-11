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
}__attribute__((packed))_sendpacket,*_send_packetinfo;

int CDC_SendFeed(uint8_t* Fed, uint16_t Len);//CDC发送反馈数据的函数
//ROS2发送代码
void Pack_And_Send_Data_ROS2(_send_packetinfo sendinfo,uint16_t Len);
int CDC_Receive_ROS2(uint8_t* Buf, uint16_t Len,_receive_packetinfo packinfo);//ROS2下的接收函数
uint16_t Get_CRC16_Check_Sum(uint8_t* pchMessage,uint32_t dwLength, uint16_t wCRC);
extern _receive_packetinfo receinfo;
#endif
