#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "can.h"
#include "pid.h"
#include "CanInst.h"
typedef enum
{
    OTHER,
    GM6020,
	M3508,
	M2006
}Motor_e;

typedef struct 
{
    //TODO: add watchdog
    PID_Type_e pid_type;
    Motor_e motor_type;
    uint8_t enable;
    float output_max;
    float zero_point;
}Motor_InfoTypeDef;

typedef struct
{
    uint16_t angle;
	int16_t speed;
	int16_t current;       //电流
	uint8_t temp;          //温度
    uint8_t unused;
}__attribute__((__packed__)) Motor_CbkMsg;

typedef struct _Motor_TypeDef
{
    Can_InfoTypedef can;
    Motor_CbkMsg data;
    float target;
    /* switching data source accroding to Motor_InfoTypeDef->type
    NOTE: don't read derectly, use MotorGetVal() instead */
    float (*value)(struct _Motor_TypeDef *st);          
    float feed_fwd;        //feedforward when type='IMU'
    float output;
    PID_TypeDef pid[2];    //outer(or angle)(0)、inner(or speed)(1) circle
    Motor_InfoTypeDef info;
}Motor_TypeDef;

typedef struct
{
    uint8_t can_id;
    uint16_t txheader_id; 
    uint16_t rxheader_id;
    PID_Type_e pid_type; 
    PID_TypeDef pid_outer; 
    PID_TypeDef pid_inner; 
    uint8_t list_id; 
    CAN_HandleTypeDef *hcan;
    Motor_e motor_type;
    float opt_max;
    float *source;      //source of ref in outer pid loop, valid only when pid_type='IMU'
}Motor_InitTypedef;

extern void MotorOptLmt(Motor_TypeDef *motor, float lmt);
extern void MotorStatusUpdate(Motor_TypeDef *motor, PID_Type_e newtype, float *source);
extern void MotorCalc(void);
extern void MotorInit(void);
extern void MotorSetTar(Motor_TypeDef *motor, float tar, uint8_t style);
extern float MotorGetVal(Motor_TypeDef *motor, uint8_t style);
extern uint8_t MotorSetZeroPoint(Motor_TypeDef *motor);
extern void MotorStop(Motor_TypeDef *motor);
extern void MotorRestart(Motor_TypeDef *motor);

extern Motor_TypeDef *motor_list[];
#endif
