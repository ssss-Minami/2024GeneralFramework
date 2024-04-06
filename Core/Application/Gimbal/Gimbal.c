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
static float pitch_tar=0;
//TODO: updata feedforward for motor_yaw
void GimbalCtrl(Gimbal_CmdTypedef *st)
{   
	//弧度转6020编码值
    MotorSetTar(motor_list[MOTOR_YAW], st->v_yaw, INCR);
    MotorSetTar(motor_list[MOTOR_YAW], st->omega_z*60*CHASSIS_RADIUS*187/(3591*WHEEL_RADIUS), FEED);
    
    pitch_tar += st->v_pitch ;

    if(pitch_tar >PITCH_UP_BOUND)
    	pitch_tar = PITCH_UP_BOUND;
    else if(pitch_tar <PITCH_LW_BOUND)
    	pitch_tar = PITCH_LW_BOUND;
    MotorSetTar(motor_list[MOTOR_PITCH], pitch_tar, ABS);
    
//    float speed = CALIBRATE*10/(2*PI*FRICTION_RADIUS);
//    speed = speed*3591/187;		//摩擦轮rps
    if(st->frictiongear)
    {
        MotorSetTar(motor_list[MOTOR_SHT_L], -30, ABS);
        MotorSetTar(motor_list[MOTOR_SHT_R], 30, ABS);
    }
    else
    {
        MotorSetTar(motor_list[MOTOR_SHT_L], 0, ABS);
        MotorSetTar(motor_list[MOTOR_SHT_R], 0, ABS);
    }

    
    if(st->shooter == SEMI_AUTO)
        MotorSetTar(motor_list[MOTOR_AMMO], 36*(2*PI/8), INCR);
    else if(st->shooter == FULL_AUTO)
        MotorSetTar(motor_list[MOTOR_AMMO], (36*SHOOT_FREQ*(float)(GIMBAL_TASK_PERIOD)/1000)*(2*PI/8), INCR);
    
}


void GimbalInit(void)
{   
    //gimbal command message queue
	osMessageQueueAttr_t const attr = {.name = "gimbal_cmd"};
    gimbal_MQ_handel = osMessageQueueNew(3, sizeof(Gimbal_CmdTypedef), &attr);
    osThreadAttr_t attr_task = {.name = "GimbalTask"};
    gimbal_task_handel = osThreadNew(GimbalTask, NULL, &attr_task);
    
    MotorSetZeroPoint(motor_list[MOTOR_YAW]);
    MotorSetTar(motor_list[MOTOR_YAW], (float)motor_list[MOTOR_YAW]->info.zero_point, ABS);
    MotorSetZeroPoint(motor_list[MOTOR_PITCH]);
    MotorSetTar(motor_list[MOTOR_PITCH], 0.5*(PITCH_LW_BOUND+PITCH_UP_BOUND)*2*PI/8192, ABS);
}

void GimbalTask(void *argument)
{

    Gimbal_CmdTypedef gimbal_cmd;
    shooter_e last = STOP;
//    uint32_t time = osKernelSysTick();
    for(;;)
    {
        osMessageQueueGet(gimbal_MQ_handel, &gimbal_cmd, NULL, 0);   //non blocking
        if(gimbal_cmd.shooter==SEMI_AUTO && last==SEMI_AUTO)
            gimbal_cmd.shooter = STOP;
        else
        	last = gimbal_cmd.shooter;
        GimbalCtrl(&gimbal_cmd);
        
//        osDelayUntil(time+GIMBAL_TASK_PERIOD);
//        time = osKernelSysTick();
        osDelay(GIMBAL_TASK_PERIOD);
    }
}
















