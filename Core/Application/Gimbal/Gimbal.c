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

//TODO: updata feedforward for motor_yaw
void GimbalCtrl(Gimbal_CmdTypedef *st)
{   
	//弧度转6020编码值
    MotorSetTar(motor_list[MOTOR_YAW], st->v_yaw, INCR);
    MotorSetTar(motor_list[MOTOR_YAW], st->omega_z*60*CHASSIS_RADIUS*187/(3591*WHEEL_RADIUS), FEED);
    float pitch_tar = 4096*st->v_pitch/PI + motor_list[MOTOR_PITCH]->target;
    if(pitch_tar >PITCH_UP_BOUND)
    	pitch_tar = PITCH_UP_BOUND;
    else if(pitch_tar <PITCH_LW_BOUND)
    	pitch_tar = PITCH_LW_BOUND;
    MotorSetTar(motor_list[MOTOR_PITCH], pitch_tar, ABS);
}


void GimbalInit(void)
{   
    //gimbal command message queue
	osMessageQueueAttr_t const attr = {.name = "gimbal_cmd"};
    gimbal_MQ_handel = osMessageQueueNew(3, sizeof(Gimbal_CmdTypedef), &attr);
    osThreadAttr_t attr_task = {.name = "GimbalTask"};
    gimbal_task_handel = osThreadNew(GimbalTask, NULL, &attr_task);
    
    MotorSetZeroPoint(motor_list[MOTOR_YAW]);
    MotorSetTar(motor_list[MOTOR_YAW], motor_list[MOTOR_YAW]->info.zero_point, ABS);
    MotorSetZeroPoint(motor_list[MOTOR_PITCH]);
    MotorSetTar(motor_list[MOTOR_YAW], 0.5*(PITCH_LW_BOUND+PITCH_UP_BOUND), ABS);
}

void GimbalTask(void *argument)
{

    Gimbal_CmdTypedef gimbal_cmd;
//    uint32_t time = osKernelSysTick();
    for(;;)
    {
        osMessageQueueGet(gimbal_MQ_handel, &gimbal_cmd, NULL, 0);   //non blocking
        GimbalCtrl(&gimbal_cmd);
        
//        osDelayUntil(time+GIMBAL_TASK_PERIOD);
//        time = osKernelSysTick();
        osDelay(GIMBAL_TASK_PERIOD);
    }
}
















