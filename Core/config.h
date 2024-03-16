#ifndef __CONFIG_H__
#define __CONFIG_H__

//电机
#define MOTOR_NUM 2     //电机总数
#define MOTOR_YAW 0     //电机列表索引测试用
#define MOTOR_PITCH 1
#define MOTOR_CHS_1 2   //chassis motor
#define MOTOR_CHS_2 3
#define MOTOR_CHS_3 4
#define MOTOR_CHS_4 5
#define MOTOR_AMMO 6    //ammofeeder motor
#define MOTOR_SHT_L 7   //left shooter motor
#define MOTOR_SHT_R 8   

//
#define CAN_HEADER_NUM 1    //CAN标识符数量
#define CAN_INST_NUM 2      //CAN设备总数
#define CAN_NUM 1           //CAN控制器数量
//#define USE_CAN2 1        //若使用CAN2，取消注释


#define TOPIC_NUM 0
//IMU
#define IMU_NUM 1
#define IMU_YAW 0

//freeRTOS任务周期
#define CONTROL_TASK_PERIOD 10   //freeRTOS执行周期, ms
#define GIMBAL_TASK_PERIOD 10 
#define CHASSIS_TASK_PERIOD 10
#define CANBUS_TASK_PERIOD 1 

//遥控器灵敏度设置
#define REMOTE_YAW_SEN 0.00000625
#define REMOTE_PITCH_SEN 0.000003125
#define REMOTE_X_SEN    0.00003125
#define REMOTE_Y_SEN    0.00003125

//各模块所用控制器
#define REMOTE_UART huart3
#define REFEREE_UART huart1

//车体数据
#define WHEEL_RADIUS 0.1  //底盘车轮半径, 米

//other
#define ABS 0
#define INCR 1
#define OUTER 0
#define INNER 1
#define FEED 2


#endif
