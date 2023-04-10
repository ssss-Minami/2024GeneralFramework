/*
 * @file		Chassis.c/h
 * @brief		底盘行为控制
 * @history
 * 版本			作者			编写日期
 * v1.0.0		郑煜壅		2023/4/1
 * 备注：单独使用该程序时，可将函数改为传入参数的形式，以提高代码可移植性
 */
#include "Chassis.h"
#include "pid.h"
#include "math.h"
#include "referee.h"


int16_t Chassis_ctrl[4] = {0};
float speed_x = 0,speed_y = 0,omega = 0,speed_x_commend = 0,speed_y_commend = 0,angle_rad = 0;

/*
 * @brief	根据设定好的目标速度进行PID运算
 * @param	无
 * @retval	向底盘电机发送的can数据
 */
void Chassis_Ctrl(void)
{
				//功率限制
//		Chassis_PowerCtrl();
		for(int i=1;i<5;i++)
			{
				PID_Incr(&PID_Motor_Speed[i],Motor[i].speed,Motor[i].target_speed);
				Chassis_ctrl[i-1] += PID_Motor_Speed[i].Output;
				if(Chassis_ctrl[i-1]>16384)
					Chassis_ctrl[i-1] = 16384;
				if(Chassis_ctrl[i-1]<-16384)
					Chassis_ctrl[i-1] = -16384;
			}

}

/*
 * @brief	底盘全向移动，根据设定好的x、y轴速度和角速度解算出各个轮子线速度
 * @param	底盘x、y轴目标速度（speed_x speed_y) 角速度（omega）
 * @retval	每个电机目标速度（target_speed)
 */
void Chassis_Move(void)
{
	//麦轮底盘解算
	Motor[1].target_speed = -(speed_x - speed_y + omega*0.5*(Length + Width))*60/(2*Wheel_radius*PI)*3591/187;
	Motor[2].target_speed = (speed_x + speed_y - omega*0.5*(Length + Width))*60/(2*Wheel_radius*PI)*3591/187;
	Motor[3].target_speed = (speed_x - speed_y - omega*0.5*(Length + Width))*60/(2*Wheel_radius*PI)*3591/187;
	Motor[4].target_speed = -(speed_x + speed_y + omega*0.5*(Length + Width))*60/(2*Wheel_radius*PI)*3591/187;
	//限制最大转速
//	for(int i=1;i<5;i++)
//	{
//		while(Motor[i].target_speed>Speed_rpm_Limit*3591/187)
//			for(int i=1;i<5;i++)
//		{
//				Motor[i].target_speed*=0.9;
//		}
//	}
	//底盘控制
	Chassis_Ctrl();
}

/*
 * @brief	一个粗劣的功率控制程序
 * @param	无
 * @retval	无
 */
float Power_limit = 120.0;
void Chassis_PowerCtrl(void)
{
	if(Ref_Info.Power_Heat_Data.chassis_power > Power_limit)
	{
		if(Ref_Info.Power_Heat_Data.chassis_power_buffer == 0)
			Power_limit = 80;
		else if(Ref_Info.Power_Heat_Data.chassis_power_buffer < 10)
			Power_limit = 110;
		else if(Ref_Info.Power_Heat_Data.chassis_power_buffer < 20)
			Power_limit = 115;
		else if(Ref_Info.Power_Heat_Data.chassis_power_buffer < 30)
			Power_limit = 120;
		else if(Ref_Info.Power_Heat_Data.chassis_power_buffer < 40)
			Power_limit = 125;
		else if(Ref_Info.Power_Heat_Data.chassis_power_buffer < 50)
			Power_limit = 130;
		else if(Ref_Info.Power_Heat_Data.chassis_power_buffer < 60)
			Power_limit = 135;
	}
	for(int i=0;i<4;i++)
		{
			Chassis_ctrl[i] *= (Power_limit)/Ref_Info.Power_Heat_Data.chassis_power;
		}
}

/*
 * @brief	底盘跟随状态
 * @param	底盘与云台相对夹角
 * @retval	底盘目标角速度
 */
void Chassis_Follow(void)
{
	PID_Origin(&PID_Motor_Angle[Chassis_Angle_ID], Motor[Motor_Yaw_ID].angle, 3430);
	//底盘角度环,以底盘与云台的相对角度为输入
	omega = -PID_Motor_Angle[Chassis_Angle_ID].Output;
}
/*
 * @brief	小陀螺状态
 * @param	无
 * @retval	底盘目标角速度
 */
void Chassis_Spin(void)
{
	static float spin_count;

	spin_count += 0.01;

	if(spin_count >= PI)
		spin_count -= 2*PI;
	else if(spin_count <= -PI)
		spin_count += 2*PI;

//	omega = 1.5 + fabs(sin(spin_count));
	omega = 5;
}
/*
 * @brief	云台速度解算到底盘
 * @param	控制器输入的x、y轴角速度
 * @retval	底盘x、y轴目标速度
 */
void Chassis_angleTransform(void)
{
	uint16_t temp;
	temp = (Motor[Motor_Yaw_ID].angle - 3392);
	if(temp>=4096)
		temp = -(8192-temp);
	angle_rad = temp*PI/4096;
	angle_rad = temp*PI/4096;

	if(angle_rad <= -PI)
		angle_rad += 2*PI;

	speed_x = speed_x_commend*cos(angle_rad) + speed_y_commend*sin(angle_rad);
	speed_y = speed_y_commend*cos(angle_rad) - speed_x_commend*sin(angle_rad);
}
