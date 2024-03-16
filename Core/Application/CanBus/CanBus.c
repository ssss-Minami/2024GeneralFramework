#include "CanBus.h"
#include "cmsis_os2.h"
#include "config.h"
#include "CanInst.h"
#include "remote.h"
#include "../Core/Instance/motor/motor.h"
extern void MotorStop(Motor_TypeDef *motor);



osThreadId_t canbus_task_handel;
//const osThreadAttr_t canbus_task_handel_attributes = {
//  .name = "CANBusTask",
//  .stack_size = 128 * 4,
//  .priority = (osPriority_t) osPriorityHigh,
//};

void CANBusInit()
{
	const osThreadAttr_t attr = {
	  .name = "CANBusTask",
	  .stack_size = 128 * 4,
	  .priority = (osPriority_t) osPriorityHigh,
	};
    canbus_task_handel = osThreadNew(CANBusTask, NULL, &attr);
    
}


void CANBusTask(void *argument)
{
//    uint32_t time = osKernelSysTick();
    for(;;)
    {
        MotorCalc();
        CanSendMsg(); 
        osDelay(5);
//        osDelayUntil(time+CANBUS_TASK_PERIOD);
//        time = osKernelSysTick();
    }
}
