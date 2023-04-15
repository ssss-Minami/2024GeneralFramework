/*
 * @file		WatchDog.c/h
 * @brief	一个简易版本的看门狗程序
 * @history
 * 版本			作者			编写日期
 * v1.0.0		郑煜壅		2023/4/1
 *
 */

#include "main.h"
#include "WatchDog.h"
#include "FreeRTOS.h"

WatchDog_t remote_WatchDog;
WatchDog_t referee_WatchDog;
WatchDog_t motor_WatchDog[8];
/*
 * @brief	喂狗程序代码，放在需要进行检测的程序段中
 * @param	对应的看门狗指针（由用户自定义）
 * @retval	无
 */
void feedDog(WatchDog_t* hdog)
{
	hdog->status = 1;
	hdog->last_time = uwTick;
}
/*
 * @brief	看门狗状态更新，放在任意能够稳定运行的程序段中
 * @param	对应的看门狗指针（由用户自定义）
 * @retval	无
 */
uint8_t Dog_Status_update(WatchDog_t* hdog)
{
	if(uwTick - hdog->last_time > 1000)//此处修改最小判断离线时间，单位ms
		hdog->status = 0;
	return hdog->status;
}
