#ifndef __GIMBAL_H__
#define __GIMBAL_H__
#include "main.h"
#include "cmsis_os2.h"
typedef enum
{
    stop = 0,
    shoot = 1
}shooter_e;


typedef struct
{
    shooter_e shooter;
    float pitch;
    float yaw;
    float v_yaw;
    float v_pitch;
    float omega_z;   //spin clockwise from above when positive 
}__attribute__((__packed__))Gimbal_CmdTypedef;


extern osMessageQueueId_t gimbal_MQ_handel;
extern void GimbalTask(void *argument);
void GimbalInit(void);

#endif
