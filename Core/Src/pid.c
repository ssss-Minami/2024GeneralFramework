/*
 * @file		pid.c/h
 * @brief		pid相关程序
 * @history
 * 版本			作者			编写日期
 * v1.0.0	 姚启杰 郑煜壅		2023/4/1
 *
 */
#include "pid.h"
#include "math.h"
PID_TypeDef PID_Motor_Speed[8];
PID_TypeDef PID_Motor_Angle[8];
Motor_TypeDef Motor[8];

float PID_SpeedCtrl_Config[8][5] = {{ 45 , 16 , 0 , 5000, 500},
                                    { 2 , 0.003 , 0 , 5000, 0},
                                    { 2 , 0.003 , 0 , 5000, 0},
                                    { 2 , 0.003 , 0 , 5000, 0},
	                            	{ 2 , 0.003 , 0 , 5000, 0},
		                            { 500 , 6.5 , 0 , 2000, 500},
									{ 500 , 6.5 , 0 , 2000, 500},
									{ 2 , 0.07 , 0 , 500, 500}};//PID速度环参数
float PID_AngleCtrl_Config[8][5] = {{ 0.002 , 0 , 0 , 5, 0},
		                            { 70 , 36 , 0 , 5000, 500},
									{ 1 , 1 , 1 , 300, 500},
									{ 1 , 1 , 1 , 300, 500},
									{ 1 , 1 , 1 , 300, 500},
									{ 0.125 , 0 , 0.05 , 100, 10000},
									{ 75 , 0 , 0 , 100, 300},
									{ 1 , 1 , 1 , 300, 500}};// PID角度环参数
/*
 * @brief  	PID初始化
 * @param	无
 * @retval 	无
 * 备注：单独使用该文件时建议改为传参的形式
 */
void PID_Init()
{
	for(int i=0;i<8;i++)
	{
		PID_Motor_Speed[i].Kp = PID_SpeedCtrl_Config[i][0];
		PID_Motor_Speed[i].Ki = PID_SpeedCtrl_Config[i][1];
		PID_Motor_Speed[i].Kd = PID_SpeedCtrl_Config[i][2];
		PID_Motor_Speed[i].Output_Max = PID_SpeedCtrl_Config[i][3];
		PID_Motor_Speed[i].Err_sum_Max = PID_SpeedCtrl_Config[i][4];
		PID_Motor_Speed[i].PID_Type = Speed;
	}
	for(int i=0;i<8;i++)
	{
		PID_Motor_Angle[i].Kp = PID_AngleCtrl_Config[i][0];
		PID_Motor_Angle[i].Ki = PID_AngleCtrl_Config[i][1];
		PID_Motor_Angle[i].Kd = PID_AngleCtrl_Config[i][2];
		PID_Motor_Angle[i].Output_Max = PID_AngleCtrl_Config[i][3];
		PID_Motor_Angle[i].Err_sum_Max = PID_AngleCtrl_Config[i][4];
		PID_Motor_Angle[i].PID_Type = Angle;
		PID_Motor_Angle[i].ID = i;
	}
}
/*
 * @brief  	清除PID积分值和输出
 * @param	pid结构体
 * @retval 	无
 *
 */
void PID_Clear(PID_TypeDef *hpid)
{
	hpid->Output_Max = 0;
	hpid->Kp = 0;
	hpid->Ki = 0;
	hpid->Kd = 0;
	hpid->Err_sum_Max = 0;
}
/*
 * @brief  	位置式pid控制器
 * @param
 * 		hpid			pid结构体
 * 		val_now			获取到的当前值
 * 		target_now		目标值
 * @retval 	控制量（在hpid中）
 *
 */
void PID_Origin(PID_TypeDef *hpid, float val_now, float target_now)
{
	if(target_now - val_now > 4096) val_now += 8192;
	if(val_now - target_now > 4096) val_now -= 8192;

	if(hpid->ID == 6)
	{
		if(target_now - val_now > 3.1415926535f) val_now += 2*3.1415926535f;
		if(val_now - target_now > 3.1415926535f) val_now -= 2*3.1415926535f;
	}

	switch(hpid->PID_Type)
	{
	case Speed:
		hpid->Err_now = 2*target_now - val_now;
		break;
	case Angle:
		hpid->Err_now = target_now - val_now;
	}

	hpid->Err_sum += hpid->Err_now;
    if(hpid->Err_sum < -hpid->Err_sum_Max) hpid->Err_sum = -hpid->Err_sum_Max;
	if(hpid->Err_sum > hpid->Err_sum_Max) hpid->Err_sum = hpid->Err_sum_Max;

	hpid->Output = hpid->Kp*hpid->Err_now + hpid->Ki*hpid->Err_sum
			+ (hpid->Kd)*(hpid->Err_now - hpid->Err_last);
    if(hpid->Output > hpid->Output_Max)
		hpid->Output = hpid->Output_Max;
	if(hpid->Output < -hpid->Output_Max )
		hpid->Output = -hpid->Output_Max;

	hpid->Err_former = hpid->Err_last;
	hpid->Err_last = hpid->Err_now;
}
/*
 * @brief  	增量式pid控制器
 * @param
 * 		hpid			pid结构体
 * 		val_now			当前值
 * 		target_now		目标值
 * @retval 	控制量的增量
 *
 */
void PID_Incr(PID_TypeDef *hpid, float val_now, float target_now)
{
	hpid->Err_now = target_now - val_now;
    hpid->Output = hpid->Kp*(hpid->Err_now - hpid->Err_last) + hpid->Ki*(hpid->Err_now)
    		+ hpid->Kd*(hpid->Err_now - hpid->Err_last - hpid->Err_diff);
	if(hpid->Output > hpid->Output_Max) hpid->Output = hpid->Output_Max;
	if(hpid->Output < -hpid->Output_Max) hpid->Output = -hpid->Output_Max;
	hpid->Err_diff = hpid->Err_now - hpid->Err_last;
	hpid->Err_last = hpid->Err_now;
}


