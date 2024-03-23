#include "Control.h"
#include "cmsis_os2.h"
#include "remote.h"
#include "../Core/Application/Gimbal/Gimbal.h"
#include "../Core/Application/Chassis/Chassis.h"
#include "config.h"
#include "../Core/Instance/motor/motor.h"
#include "imu.h"
#include "math.h"

osThreadId_t control_task_handel;
uint32_t heap_left;
void ControlInit(void)
{
	osThreadAttr_t attr = {.name = "ControlTask"};
    control_task_handel = osThreadNew(ControlTask, NULL, &attr);
}

/*
 * @brief  	底盘坐标系转云台坐标系
 * @param	底盘命令结构体指针
 * @param   底盘坐标系中，云台yaw与底盘x平行时yaw轴电机角度（编码值）
 * @param   yaw电机当前角度（编码值）
 * @retval 	无
 */
void ChassisCmdTrans(Chassis_CmdTypedef *cmd, float chs_zeropoint, float gim_ang)
{
    float bias = ((gim_ang/8192)*2*PI) - ((chs_zeropoint/8192)*2*PI);
    float vx_tmp = cmd->vx*cosf(bias) + cmd->vy*sinf(bias);
    float vy_tmp = -cmd->vx*sinf(bias) + cmd->vy*cosf(bias);
    cmd->vx = vx_tmp;
    cmd->vy = vy_tmp;
}

void ControlTask(void *argument)
{
//    uint32_t time = osKernelSysTick();
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

        chassis_cmd.vx = -RC_Ctl.rc.ch2 * REMOTE_X_SEN;
        chassis_cmd.vy = -RC_Ctl.rc.ch1 * REMOTE_Y_SEN;
        chassis_cmd.omega_z = RC_Ctl.rc.sw2==1 ? 2*PI : 0;
        ChassisCmdTrans(&chassis_cmd, CHASSIS_ZEROPOINT, MotorGetVal(motor_list[MOTOR_YAW], ORIGIN));
        osMessageQueuePut(chassis_MQ_handel, &chassis_cmd, NULL, 0);

        heap_left = osThreadGetStackSpace(control_task_handel);
        imu_list[0]->update(imu_list[0]);
        osMessageQueueGet(chassis_RMQ_handel, &omege_chs, NULL, 0);
        gimbal_cmd.v_yaw = -RC_Ctl.rc.ch3 * REMOTE_YAW_SEN*CONTROL_TASK_PERIOD;
        gimbal_cmd.v_pitch = -RC_Ctl.rc.ch4 * REMOTE_PITCH_SEN*CONTROL_TASK_PERIOD;
        gimbal_cmd.omega_z = -omege_chs;
        osMessageQueuePut(gimbal_MQ_handel, &gimbal_cmd, NULL, 0);
//        osDelayUntil(time+CONTROL_TASK_PERIOD);
//        time = osKernelSysTick();
        osDelay(CONTROL_TASK_PERIOD);
    }
}
