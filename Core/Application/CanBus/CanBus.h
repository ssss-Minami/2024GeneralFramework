#ifndef __CANBUS_H__
#define __CANBUS_H__

#include "main.h"
#include "cmsis_os2.h"

extern void CANBusTask(void *argument);
void CANBusInit();
//extern osThreadId_t canbus_task_handel;
#endif
