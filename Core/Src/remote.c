/*
 * @file		remote.c/h
 * @brief		遥控器串口中断
 * @history
 * 版本			作者			编写日期				更新内容
 * v1.0.0		姚启杰		2023/4/1
 * v1.1.1		许金帅		2023/4/12			支持接收机热拔插
 *
 */
#include "remote.h"
#include "usart.h"
#include "main.h"
#include "string.h"
#include "math.h"
#include "WatchDog.h"
#include <stdlib.h>
#include "stm32f4xx_hal_dma.h"
#include "dma.h"
#include "config.h"
RC_Ctl_t RC_Ctl;
uint8_t RC_buff[36]={0},count_remote_skip;
#define Remote_CHANNAL_ERROR_VALUE 700

void Remote_unable(void)//关闭遥控器
{

        __HAL_RCC_USART3_CLK_DISABLE();
}
void Remote_restart()//重启串口和DMA，针对于数据错位和无法进入回调
{
	__HAL_RCC_USART3_CLK_DISABLE();
	__HAL_RCC_DMA1_CLK_DISABLE();
    __HAL_DMA_CLEAR_FLAG(&hdma_usart3_rx,DMA_FLAG_TCIF1_5);
	__HAL_UART_CLEAR_IDLEFLAG(&huart3);
	__HAL_DMA_CLEAR_FLAG(&hdma_usart3_rx,DMA_IT_TC);
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    memset(&RC_Ctl, 0, sizeof(RC_Ctl));
    HAL_UART_Receive_DMA(&huart3, RC_buff, RC_FRAME_LENGTH);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART3)
	{
		feedDog(&remote_WatchDog);//进回调则喂狗
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
	        Remote_restart();
	        return ;
	    }
//dev
//	    memcpy(&RC_Ctl.keyboard, RC_buff +6, 8);
//	    RC_Ctl.keyboard.W = RC_buff[14] & 0x80;
//	    RC_Ctl.keyboard.S = RC_buff[14] & 0x40;
//	    RC_Ctl.keyboard.A = RC_buff[14] & 0x20;
//	    RC_Ctl.keyboard.D = RC_buff[14] & 0x10;
//	    RC_Ctl.keyboard.Q = RC_buff[14] & 0x08;
//	    RC_Ctl.keyboard.E = RC_buff[14] & 0x04;
//	    RC_Ctl.keyboard.SHIFT = RC_buff[14] & 0x02;
//	    RC_Ctl.keyboard.CTRL = RC_buff[14] & 0x01;

	    RC_Ctl.rc.wheel = (RC_buff[16] | RC_buff[17] << 8) - 1024;
	    HAL_UART_Receive_DMA(&huart3, RC_buff, RC_FRAME_LENGTH);//初始化DMA
	    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);//IDLE 中断使能
	}
}
//判断遥控器数据是否出错，
uint8_t Remote_data_is_error(void)
{
    //使用了go to语句 方便出错统一处理遥控器变量数据归零
    if ((RC_Ctl.rc.ch1) > Remote_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if ((RC_Ctl.rc.ch2) > Remote_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if ((RC_Ctl.rc.ch3) > Remote_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if ((RC_Ctl.rc.ch4) > Remote_CHANNAL_ERROR_VALUE)
    {
        goto error;
    }
    if (RC_Ctl.rc.sw1 == 0)
    {
        goto error;
    }
    if (RC_Ctl.rc.sw2 == 0)
    {
        goto error;
    }
    return 0;

error:
    RC_Ctl.rc.ch1 = 0;
    RC_Ctl.rc.ch2 = 0;
    RC_Ctl.rc.ch3 = 0;
    RC_Ctl.rc.ch4 = 0;
    RC_Ctl.rc.sw1 = RC_SW_DOWN;
    RC_Ctl.rc.sw2 = RC_SW_DOWN;
    return 1;
}

void RemoteInit(void)
{
	//TODO:改成receiveToIdle()的形式
	HAL_UART_Receive_DMA(&REMOTE_UART, RC_buff, RC_FRAME_LENGTH);//初始化遥控器DMA
 	__HAL_UART_ENABLE_IT(&REMOTE_UART, UART_IT_IDLE);//IDLE 中断使能
}

