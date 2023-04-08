/*
 * @file		remote.c/h
 * @brief		遥控器串口中断
 * @history
 * 版本			作者			编写日期
 * v1.0.0		姚启杰		2023/4/1
 *
 */
#include "remote.h"
#include "usart.h"
#include "main.h"
#include "string.h"
#include "math.h"
#include "WatchDog.h"
#include <stdlib.h>
#include "WatchDog.h"
RC_Ctl_t RC_Ctl;
uint8_t RC_buff[18],count_remote_skip;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	feedDog(&remote_WatchDog);//进回调则喂狗
	if(huart->Instance == USART3)
	{
		RC_Ctl.rc.ch1 = (RC_buff[0] | RC_buff[1] << 8) & 0x07FF;
		RC_Ctl.rc.ch1 -= 1024;
		RC_Ctl.rc.ch2 = (RC_buff[1] >> 3 | RC_buff[2] << 5) & 0x07FF;
		RC_Ctl.rc.ch2 -= 1024;
		RC_Ctl.rc.ch3 = (RC_buff[2] >> 6 | RC_buff[3] << 2 | RC_buff[4] << 10) & 0x07FF;
		RC_Ctl.rc.ch3 -= 1024;
		RC_Ctl.rc.ch4 = (RC_buff[4] >> 1 | RC_buff[5] << 7) & 0x07FF;
		RC_Ctl.rc.ch4 -= 1024;
	    /* prevent remote control zero deviation */
	    if (RC_Ctl.rc.ch1 <= 5 && RC_Ctl.rc.ch1 >= -5)
	    {
	    	RC_Ctl.rc.ch1 = 0;
	    }
	    if (RC_Ctl.rc.ch2 <= 5 && RC_Ctl.rc.ch2 >= -5)
	    {
	    	RC_Ctl.rc.ch2 = 0;
	    }
	    if (RC_Ctl.rc.ch3 <= 5 && RC_Ctl.rc.ch3 >= -5)
	    {
	    	RC_Ctl.rc.ch3 = 0;
	    }
	    if (RC_Ctl.rc.ch4 <= 5 && RC_Ctl.rc.ch4 >= -5)
	    {
	    	RC_Ctl.rc.ch4 = 0;
	    }

	    RC_Ctl.rc.sw1 = ((RC_buff[5] >> 4) & 0x000C) >> 2;
	    RC_Ctl.rc.sw2 = (RC_buff[5] >> 4) & 0x0003;
	    if ((abs(RC_Ctl.rc.ch1) > 660) || \
	            (abs(RC_Ctl.rc.ch2) > 660) || \
	            (abs(RC_Ctl.rc.ch3) > 660) || \
	            (abs(RC_Ctl.rc.ch4) > 660))
	    {
	        memset(&RC_Ctl, 0, sizeof(RC_Ctl));
	        return ;
	    }
//
//	    RC_Ctl.rc.mouse.x = RC_buff[6] | (RC_buff[7] << 8); // x axis
//	    RC_Ctl.rc.mouse.y = RC_buff[8] | (RC_buff[9] << 8);
//	    RC_Ctl.rc.mouse.z = RC_buff[10] | (RC_buff[11] << 8);
//
//	    RC_Ctl.rc.mouse.l = RC_buff[12];
//	    RC_Ctl.rc.mouse.r = RC_buff[13];
//
//	    RC_Ctl.rc.kb.key_code = RC_buff[14] | RC_buff[15] << 8; // key borad code
	    RC_Ctl.rc.wheel = (RC_buff[16] | RC_buff[17] << 8) - 1024;
	    HAL_UART_Receive_DMA(&huart3, RC_buff, RC_FRAME_LENGTH);//初始化DMA
	    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);//IDLE 中断使能
	}
}
