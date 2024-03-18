#ifndef __CHASSIS_H__
#define __CHASSIS_H__
#include "main.h"
#include "cmsis_os2.h"

#define ROOT_2 1.4142135

typedef struct
{
    float vx;
    float vy;
    float omega_z;   //spin clockwise from above when positive 
    uint8_t stop:1;
}__attribute__((__packed__))Chassis_CmdTypedef;

extern osMessageQueueId_t chassis_MQ_handel;
extern osMessageQueueId_t chassis_RMQ_handel;
extern osThreadId_t chassis_task_handel;;
void ChassisInit(void);
#endif
