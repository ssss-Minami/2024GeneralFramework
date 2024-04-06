/*
 * @file		kalman.c/h
 * @brief		卡尔曼滤波
 * @history
 * 版本			作者			编写日期
 * v1.0.0	 姚启杰 郑煜壅		2023/4/1
 *
 */
#include "kalman.h"
#include "math.h"
const float dt = 0.001;
/*
 * @brief  	简化版一阶卡尔曼滤波初始化
 * @param	卡尔曼滤波结构体
 * @retval 	无
 *
 */
void KalmanFilter_Init(Kalman_TypeDef *klm_typedef)
{
	klm_typedef->K = 0;
	klm_typedef->Q = 0.01;
	klm_typedef->R = 0.3;
	klm_typedef->p_now = 0;
	klm_typedef->p_past = 0;
	klm_typedef->output_Max = 40;
}
/*
 * @brief  	简化版一阶卡尔曼滤波
 * @param
 * 		f 		卡尔曼滤波结构体
 * 		input	数据输入
 * @retval 	滤波输出（在f中）
 *
 */
float KalmanFilter(Kalman_TypeDef *f, float input)
{
	//简化一阶卡尔曼滤波，默认A=1，H=1，B=0
	if(fabs(input - (float)(f->output)) >= 8)
	{
		f->output = input;
	}
	f->p_now = f->p_past + f->Q;
	f->K = f->p_now / (f->p_now + f->R);
	f->output = f->output + f->K * (input - f->output);

	f->p_past = (1 - f->K) * f->p_now;
	return f->output;
}
Kalman_TypeDef Klm_Motor[6];
IMU_fliter_TypeDef imu_fliter[3];
/*
 * @brief  	未简化的基于imu的一阶卡尔曼滤波
 * @param
 * 		f 			imu滤波结构体
 * 		newangle	获取的新角度
 * 		newgyro		获取的角速度
 * @retval 	滤波输出（在f中）
 *
 */
void IMU_fliter(IMU_fliter_TypeDef *f,float newangle,float newgyro)
{
	f->angle += f->gyro*dt;

	f->P[0][0] = f->P[0][0] + f->Q_angle + (f->P[0][1]+f->P[1][0])*dt + (f->P[1][1]*dt*dt);
	f->P[0][1] = f->P[0][1] + (f->P[1][1]*dt);
	f->P[1][0] = f->P[1][0] + (f->P[1][1]*dt);
	f->P[1][1] = f->P[1][1] + f->Q_gyro;

	f->K[0] = (f->P[0][0]+f->P[0][1])/(f->P[0][0]+f->P[0][1]+f->P[1][0]+f->P[1][1]+ f->R_angle);
	f->K[1] = (f->P[1][0]+f->P[1][1])/(f->P[0][0]+f->P[0][1]+f->P[1][0]+f->P[1][1]+ f->R_gyro);
	if(f->P[0][0] >= 1)
	{
		f->P[0][0]=f->P[0][0];
	}

	if(newangle - f->angle > 3.1415926535f)
		newangle -= 2*3.1415926535f;
	if(newangle - f->angle < -3.1415926535f)
		newangle += 2*3.1415926535f;

	f->angle += f->K[0]*(newangle - f->angle);

	if(f->angle > 3.1415926535f)
		f->angle -= 2*3.1415926535f;
	if(f->angle < -3.1415926535f)
		f->angle += 2*3.1415926535f;

	f->gyro += f->K[1]*(newgyro - f->gyro);
	f->output = f->angle;

	f->P[0][0] = f->P[0][0] - (f->K[0]*f->P[0][0]);
	f->P[0][1] = f->P[0][1] - (f->K[0]*f->P[0][1]);
	f->P[1][0] = f->P[1][0] - (f->K[1]*f->P[1][0]);
	f->P[1][1] = f->P[1][1] - (f->K[1]*f->P[1][1]);

}
/*
 * @brief  	基于imu的一阶卡尔曼滤波初始化
 * @param	卡尔曼滤波结构体
 * @retval 	无
 *
 */
void IMU_fliter_Init(IMU_fliter_TypeDef *f)
{
	f->P[0][0] = 0.001;
	f->P[0][1] = 0.001;
	f->P[1][0] = 0.001;
	f->P[1][1] = 0.001;
	f->K[0] = 0;
	f->K[1] = 0;
	f->Q_angle = 0.01;
	f->Q_gyro = 0.012;
	f->R_angle = 0.001;
	f->R_gyro = 0.003;
}
