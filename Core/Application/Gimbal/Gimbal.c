#include "Gimbal.h"
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "../Core/Instance/motor/motor.h"
#include "config.h"
#include "../Core/Algorithm/PID/pid.h"
#include "ErrorHandel.h"


  
osMessageQueueId_t gimbal_MQ_handel;
osThreadId_t gimbal_task_handel;
// const osMessageQueueAttr_t gimbal_cmd_attr = {
//     .name = "gimbal_cmd_queue",
//     .mq_size = 3*sizeof(gimbal_cmd),
//     .mq_mem = sizeof(gimbal_cmd)
// };


void GimbalCtrl(Gimbal_CmdTypedef *st)
{   
	//弧度转6020编码值
    MotorSetTar(motor_list[MOTOR_YAW], 4096*st->v_yaw/PI, INCR);
    MotorSetTar(motor_list[MOTOR_PITCH], 4096*st->v_pitch/PI, INCR);
}


void GimbalInit(void)
{   
    //gimbal command message queue
	osMessageQueueAttr_t const attr = {.name = "gimbal_cmd"};
    gimbal_MQ_handel = osMessageQueueNew(3, sizeof(Gimbal_CmdTypedef), &attr);
    osThreadAttr_t attr_task = {.name = "GimbalTask"};
    gimbal_task_handel = osThreadNew(GimbalTask, NULL, &attr_task);
    
    MotorGetZeroPoint(motor_list[MOTOR_YAW]);
    MotorSetTar(motor_list[MOTOR_YAW], motor_list[MOTOR_YAW]->info.zero_point, ABS);

}

void GimbalTask(void *argument)
{

    Gimbal_CmdTypedef gimbal_cmd; 
    uint32_t time = osKernelSysTick();
    for(;;)
    {
        osMessageQueueGet(gimbal_MQ_handel, &gimbal_cmd, NULL, 0);   //non blocking
        GimbalCtrl(&gimbal_cmd);
        
//        osDelayUntil(time+GIMBAL_TASK_PERIOD);
//        time = osKernelSysTick();
        osDelay(5);
    }
}
















