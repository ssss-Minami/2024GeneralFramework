/*
 * @file		referee.c/h
 * @brief		遥控器串口中断
 * @history
 * 版本			作者			编写日期
 * v1.0.0		姚启杰		2023/4/1
 *
 */
#ifndef __REMOTE_H
#define __REMOTE_H
#include "main.h"
typedef struct
{
	struct
	{
	/* rocker channel information */
    int16_t ch1;
    int16_t ch2;
    int16_t ch3;
    int16_t ch4;
    /* left and right lever information */
    uint8_t sw1;
    uint8_t sw2;
    int16_t wheel;
   	}rc;
    /* mouse movement and button information */
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;

        uint8_t l;
        uint8_t r;

       uint16_t W : 1;
       uint16_t S : 1;
       uint16_t A : 1;
       uint16_t D : 1;
       uint16_t SHIFT : 1;
       uint16_t CTRL : 1;
       uint16_t Q : 1;
       uint16_t E : 1;
       uint16_t R : 1;
       uint16_t F : 1;
       uint16_t G : 1;
       uint16_t Z : 1;
       uint16_t X : 1;
       uint16_t C : 1;
       uint16_t V : 1;
       uint16_t B : 1;
    } __attribute__((packed))keyboard;

}RC_Ctl_t;
extern RC_Ctl_t RC_Ctl;
#endif
