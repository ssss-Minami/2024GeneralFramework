/*
 * @file		Chassis.c/h
 * @brief		底盘行为控制
 * @history
 * 版本			作者			编写日期
 * v1.0.0		郑煜壅		2023/4/1
 *
 */
#ifndef __CHASSIS_H
#define __CHASSIS_H

#include "pid.h"
#ifndef PI
#define PI 3.14159265358979f
#endif
//底盘麦轮间距
#define Length 0.38
#define Width 0.41
//麦轮半径
#define Wheel_radius 0.075
//功率限制量

//转速限制量
#define Speed_rpm_Limit 450.0

extern void Chassis_Ctrl(void);
extern void Chassis_Move(void);
extern void Chassis_PowerCtrl(void);
extern void Chassis_Follow(void);
extern void Chassis_Spin(void);
extern void Chassis_angleTransform(void);
extern void Chassis_Task(void);

extern int16_t Chassis_ctrl[4];
extern float speed_x,speed_y,omega,speed_x_commend,speed_y_commend,angle_rad;
#endif
