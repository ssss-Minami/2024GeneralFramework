/*
 * @file		WatchDog.c/h
 * @brief	一个简易版本的看门狗程序
 * @history
 * 版本			作者			编写日期
 * v1.0.0		郑煜壅		2023/4/1
 *
 */
#ifndef WATCHDOG_H
#define WATCHDOG_H

#include "main.h"

typedef struct{
	uint32_t last_time;
	uint8_t status;
} WatchDog_t;
extern WatchDog_t remote_WatchDog;
extern WatchDog_t referee_WatchDog;
extern WatchDog_t motor_WatchDog[8];
void feedDog(WatchDog_t* hdog);
uint8_t Dog_Status_update(WatchDog_t* hdog);


#endif
