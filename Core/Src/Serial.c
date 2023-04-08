/*
 * @file		Serial.c/h
 * @brief		usb-cdc通信程序，用于上位机和stm32的通信，需配合上位机代码使用
 * @history
 * 版本			作者			编写日期
 * v1.0.0		许金帅		2023/4/1
 *
 */
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "stdio.h"
#include "string.h"
#include "Serial.h"

int CDC_SendFeed(uint8_t* Fed, uint16_t Len);//CDC发送反馈数据的函数
int CDC_Receive(uint8_t* Buf, uint16_t Len,_controldata_chassisInfo concha,_controldata_panInfo conpan);//CDC接收控制数据的函数
_receive_packetinfo receinfo;
void Pack_Data(_FeedBack* feedback,uint8_t* feedArray);//打包反馈的信息
void UnPack_Data_Cha(uint8_t *ConChaArray,_controldata_chassisInfo concha);//解码收到的底盘控制数据
void UnPack_Data_pan(uint8_t *ConpanArray,_controldata_panInfo conpan);//解码收到的云台控制数据
//ROS2相关代码
//void Pack_And_Send_Data_ROS2(_send_packetinfo sendinfo,uint8_t* TempArray,uint8_t* sendData,uint16_t Len);//打包以及发送函数
void UnPack_Data_ROS2(uint8_t *receive_Array,_receive_packetinfo receive_info);//解包函数
int CDC_Receive_ROS2(uint8_t* Buf, uint16_t Len,_receive_packetinfo packinfo);//接收函数
void Data_Select(uint8_t* Origin_Data,uint8_t* Finish_Data);//剔除无用发送数据
//uint16_t Get_CRC16_Check_Sum( _send_packetinfo sendinfo,uint8_t* TempArray,uint8_t* pchMessage,uint32_t dwLength, uint16_t wCRC);//CRC校验码生成

//这是CRC校验码的数据表
const uint16_t CRC_Data[256]={ /* CRC 字节余式表 */
		    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 0x8c48,
		    0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7, 0x1081, 0x0108,
		    0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 0x9cc9, 0x8d40, 0xbfdb,
		    0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876, 0x2102, 0x308b, 0x0210, 0x1399,
		    0x6726, 0x76af, 0x4434, 0x55bd, 0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e,
		    0xfae7, 0xc87c, 0xd9f5, 0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e,
		    0x54b5, 0x453c, 0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd,
		    0xc974, 0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
		    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 0x5285,
		    0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 0xdecd, 0xcf44,
		    0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 0x6306, 0x728f, 0x4014,
		    0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 0xef4e, 0xfec7, 0xcc5c, 0xddd5,
		    0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 0x7387, 0x620e, 0x5095, 0x411c, 0x35a3,
		    0x242a, 0x16b1, 0x0738, 0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862,
		    0x9af9, 0x8b70, 0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e,
		    0xf0b7, 0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
		    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 0x18c1,
		    0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 0xa50a, 0xb483,
		    0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, 0x2942, 0x38cb, 0x0a50,
		    0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 0xb58b, 0xa402, 0x9699, 0x8710,
		    0xf3af, 0xe226, 0xd0bd, 0xc134, 0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7,
		    0x6e6e, 0x5cf5, 0x4d7c, 0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1,
		    0xa33a, 0xb2b3, 0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72,
		    0x3efb, 0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
		    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 0xe70e,
		    0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 0x6b46, 0x7acf,
		    0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9, 0xf78f, 0xe606, 0xd49d,
		    0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 0x7bc7, 0x6a4e, 0x58d5, 0x495c,
		    0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};
/**
* @brief  将数据包发送到上位机
  * @param
			Fed 数据包
			Len 数据包的大小--占用内存字节数（协议规定为8Bytes）
  * @retval 无
  */
int CDC_SendFeed(uint8_t* Fed, uint16_t Len)
{
	CDC_Transmit_FS(Fed, Len);
	return 0;
}

/**
  * @brief  接收上位机传入的数据包
  * @param
		Buf 指向一个_FeedBack的指针
	    Len 传入数据的长度
	    concha 底盘控制信息的结构体
	    conpan 云台控制信息的结构体
  * @retval 0  暂时没有意义
  */
int CDC_Receive(uint8_t* Buf, uint16_t Len,_controldata_chassisInfo concha,_controldata_panInfo conpan)
{
	CDC_Receive_FS(Buf, &Len);
	if(Buf[0]==0xFA)UnPack_Data_Cha(Buf, concha);
	else if(Buf[0]==0xFB)UnPack_Data_pan(Buf, conpan);
	else return -1;
	return 0;
}

/**
  * @param
		feedback 指向一个_FeedBack的指针
	    feedArray 由数据段重组的uint8类型数组
  * @retval 无
  */
void Pack_Data(_FeedBack* feedback,uint8_t* feedArray)
{	//把数组中信息封入数据包中
	feedArray[0] = 0XFF;//这是帧头
	feedArray[1] = feedback->Shoot_Mode;
	feedArray[2] = feedback->Shoot_Speed;
	feedArray[3] = feedback->Armor_Id;
	feedArray[4] = (uint8_t)(feedback->HP_Remain);
	feedArray[5] = (uint8_t)(feedback->HP_Remain >> 8);
	feedArray[6] = 0XAA;//暂时无意义
	feedArray[7] = 0XFE;//芝士帧尾
}

/**
  * @brief  将接收到的控制数组拆分为对应的结构体
  * @param
        ConChaArray 传入底盘控制数组
        concha 底盘控制信息的结构体
  * @retval 无
  */
void UnPack_Data_Cha(uint8_t *ConChaArray,_controldata_chassisInfo concha)
{
	concha->x_Speed=ConChaArray[1];
	concha->y_Speed=ConChaArray[2];
	concha->rotational_speed=ConChaArray[3];
	concha->chassis_state=ConChaArray[4];
}

/**
  * @brief  将接收到的控制数组拆分为对应的结构体
  * @param
		ConpanArray 传入云台控制数组
		conpan 云台控制数组的结构体
  * @retval 无
  */
void UnPack_Data_pan(uint8_t *ConpanArray,_controldata_panInfo conpan)
{
	conpan->pitch_angle = ConpanArray[1] | ConpanArray[2] <<8;
	conpan->yaw_angle = ConpanArray[3] | ConpanArray[4] <<8;
	conpan->shoot_mode = ConpanArray[5];
	conpan->shoot_speed = ConpanArray[6];
}


//以下是为了与ROS2而特制的接收发送代码
/**
  * @brief  将对应的结构体打包为缓冲数组
  * @param
		sendinfo  要发送的结构体
		TempArray 用于暂存数据的缓冲数组
		sendData  存放最终发送的实际数据
		Len       确定发送的字节长度
  * @retval 无
  */
void Pack_And_Send_Data_ROS2(_send_packetinfo sendinfo,uint16_t Len)
{
	uint8_t pchMessage[Len];
	uint16_t w_crc=0;
	memcpy(pchMessage, sendinfo, Len-2);
	w_crc=Get_CRC16_Check_Sum(pchMessage, Len-2, 0xFFFF);
	pchMessage[Len - 2] = (uint8_t)(w_crc & 0x00ff);
	pchMessage[Len - 1] = (uint8_t)((w_crc >> 8) & 0x00ff);
	CDC_SendFeed(pchMessage, Len);
}

/**
  * @brief  将接收到的控制数组拆分为对应的结构体
  * @param
		receive_Array 接收到的数据数组
		receive_info 接收数据的结构体
  * @retval 无
  */
void UnPack_Data_ROS2(uint8_t *receive_Array,_receive_packetinfo receive_info)
{
	memcpy(receive_info,receive_Array, 48);
}

/**
  * @brief  ROS2下的数据接收函数
  * @param
        Buf 缓存数组
		Len 数组长度
		packinfo 具体接收数据的结构体
  * @retval 无
  */
int CDC_Receive_ROS2(uint8_t* Buf, uint16_t Len,_receive_packetinfo packinfo)
{
	CDC_Receive_FS(Buf, &Len);
	if(Buf[0]==(uint8_t)(0xA5)){UnPack_Data_ROS2(Buf,packinfo);}
	else return -1;
	return 0;
}

/**
  * @brief  数据挑选函数
  * @param
		Origin_Data 打包后的初始数组
		Finish_Data 整理过的发送数组
  * @retval 无
  */
void Data_Select(uint8_t* Origin_Data,uint8_t* Finish_Data)//因为打包后数组中间会多2个字节，所以需要进行调整才能发送
{
	for(int i=0;i<2;i++)
	Finish_Data[i]=Origin_Data[i];
	for(int i=2;i<12;i++)
	Finish_Data[i]=Origin_Data[i+2];
}

/**
  * @brief CRC16 Caculation function
  * @param[in] sendinfo : 结构体
  * @param[in] TempArray : 存储被转换后的数据，需要进一步剔除无用字节
  * @param[in] pchMessage : 最终需要被计算的数据
  * @param[in] dwLength : Stream length = Data + checksum
  * @param[in] wCRC : CRC16 init value(default : 0xFFFF)
  * @return : CRC16 checksum
  */
uint16_t Get_CRC16_Check_Sum(uint8_t* pchMessage,uint32_t dwLength, uint16_t wCRC)
{
  uint8_t ch_data;
  if (pchMessage == 0) return 0xFFFF;
  while (dwLength--) {
    ch_data = *pchMessage++;
    (wCRC) =
      ((uint16_t)(wCRC) >> 8) ^ CRC_Data[((uint16_t)(wCRC) ^ (uint16_t)(ch_data)) & 0x00ff];
  }
  return wCRC;
}
