#ifndef __PID_H__
#define __PID_H__
#include "main.h"
typedef struct __PID_TypeDef
{
	float Kp;
	float Ki;
	float Kd;
	float Err_former;
	float Err_last;
	float Err_now;
	float Err_sum;
	float Err_diff;
	float Output_Max;
	float Err_sum_Max;

}PID_TypeDef;

typedef enum
{
    ANGLE = 0,          //速度环，使用增量式
    SPEED = 1,         //角度环，使用原始位置式
    DUAL_LOOP,   //双环控制，外环角度内环速度
    IMU,
	AMMO
}PID_Type_e;

extern float PID_Origin(PID_TypeDef *hpid, float val_now, float target_now);
extern float PID_Incr(PID_TypeDef *hpid, float val_now, float target_now);
extern void PID_Clear(PID_TypeDef *hpid);

#endif
