#include "main.h"
#include "UI_task.h"
#include "config.h"
#include "cmsis_os2.h"
#include "../Core/Instance/referee/ui.h"
#include "../Core/Instance/referee/referee.h"
#include "../Core/Instance/motor/motor.h"

osThreadId_t UI_task_handel;
void UI_Task(void *arguement);

void UI_taskInit()
{
    osThreadAttr_t attr = {.name = "UI_Task"};
    UI_task_handel = osThreadNew(UI_Task, NULL, &attr);
} 
float tmp_pitch = 0;
uint8_t aim_tmp = 0;
uint8_t is_connect = 0;
void UI_Task(void *arguement)
{

    float pitch = MotorGetVal(motor_list[MOTOR_PITCH], RAD);
    for(;;)
    {
        if(!is_connect)
        {
            if(refree_info.Power_Heat_Data.buffer_energy)
            {
                is_connect = 1;
                UITest();
            }
        }
        else
        {
            // pitch = MotorGetVal(motor_list[MOTOR_PITCH], RAD);
            // UIRefresh(tmp_pitch, 0.3, aim_tmp, 0);
        }
        tmp_pitch = MotorGetVal(motor_list[MOTOR_PITCH], RAD);
        osDelay(100);
    }
}
