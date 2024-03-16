#include "pid.h"
/*
 * @brief  	位置式pid控制器
 * @param
 * 		hpid			pid结构体
 * 		val_now			获取到的当前值
 * 		target_now		目标值
 * @retval 	控制量
 *
 */
float PID_Origin(PID_TypeDef *hpid, float val_now, float target_now)
{

	hpid->Err_now = target_now - val_now;
    float Output;

	hpid->Err_sum += hpid->Err_now;
    if(hpid->Err_sum < -hpid->Err_sum_Max) hpid->Err_sum = -hpid->Err_sum_Max;
	if(hpid->Err_sum > hpid->Err_sum_Max) hpid->Err_sum = hpid->Err_sum_Max;

	Output = hpid->Kp*hpid->Err_now + hpid->Ki*hpid->Err_sum
			+ (hpid->Kd)*(hpid->Err_now - hpid->Err_last);
    if(Output > hpid->Output_Max)
		Output = hpid->Output_Max;
	if(Output < -hpid->Output_Max )
		Output = -hpid->Output_Max;

	hpid->Err_former = hpid->Err_last;
	hpid->Err_last = hpid->Err_now;
    return Output;
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
float PID_Incr(PID_TypeDef *hpid, float val_now, float target_now)
{
	hpid->Err_now = target_now - val_now;
    float Output;
    Output = hpid->Kp*(hpid->Err_now - hpid->Err_last) + hpid->Ki*(hpid->Err_now) + hpid->Kd*(hpid->Err_now - hpid->Err_last - hpid->Err_diff);
	if(Output > hpid->Output_Max) Output = hpid->Output_Max;
	if(Output < -hpid->Output_Max) Output = -hpid->Output_Max;
	hpid->Err_diff = hpid->Err_now - hpid->Err_last;
	hpid->Err_last = hpid->Err_now;
    return Output;
}

/*
 * @brief  	清除PID积分值和输出
 * @param	pid结构体
 * @retval 	无
 *
 */
void PID_Clear(PID_TypeDef *hpid)
{
//	hpid->Output_Max = 0;
//	hpid->Err_sum_Max = 0;
    hpid->Err_diff = 0;
    hpid->Err_former = 0;
    hpid->Err_last = 0;
    hpid->Err_now = 0;
    hpid->Err_sum = 0;
}
