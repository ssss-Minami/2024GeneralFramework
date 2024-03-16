#include "Control.h"
#include "cmsis_os2.h"
#include "remote.h"
#include "../Core/Application/Gimbal/Gimbal.h"
#include "../Core/Application/Chassis/Chassis.h"
#include "config.h"
#include "motor.h"

osThreadId_t control_task_handel;

void ControlInit(void)
{
	osThreadAttr_t attr = {.name = "ControlTask"};
    control_task_handel = osThreadNew(ControlTask, NULL, &attr);
}

void ControlTask(void *argument)
{
    uint32_t time = osKernelSysTick();
    Gimbal_CmdTypedef static gimbal_cmd;
    Chassis_CmdTypedef static chassis_cmd;
    float static omege_chs;
    for(;;)
    {
        if(RC_Ctl.rc.sw1==0 || RC_Ctl.rc.sw1==2)
        {   
            MotorStop(NULL);
            chassis_cmd.stop = 1;
        }
        else if(RC_Ctl.rc.sw1==3)
        {
            MotorRestart(NULL);
            chassis_cmd.stop = 0;
        }
        
        chassis_cmd.vx = RC_Ctl.rc.ch4 * REMOTE_PITCH_SEN;
        chassis_cmd.vy = RC_Ctl.rc.ch4 * REMOTE_PITCH_SEN;
        chassis_cmd.omega_z = RC_Ctl.rc.sw2==1 ? 1 : 0;
        osMessageQueuePut(chassis_MQ_handel, &chassis_cmd, NULL, 0);

        osMessageQueueGet(chassis_RMQ_handel, &omege_chs, NULL, 0);
        gimbal_cmd.v_yaw = RC_Ctl.rc.ch3 * REMOTE_YAW_SEN*CONTROL_TASK_PERIOD;
        gimbal_cmd.v_pitch = RC_Ctl.rc.ch4 * REMOTE_PITCH_SEN*CONTROL_TASK_PERIOD;
        gimbal_cmd.omega_z = omege_chs;
        osMessageQueuePut(gimbal_MQ_handel, &gimbal_cmd, NULL, 0);
//        osDelayUntil(time+CONTROL_TASK_PERIOD);
//        time = osKernelSysTick();
        osDelay(CONTROL_TASK_PERIOD);
    }
}
