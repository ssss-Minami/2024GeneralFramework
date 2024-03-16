#ifndef __IMU_H__
#define __IMU_H__

#include "main.h"
#include "config.h"
//TODO:后续新增对IIC协议的支持
typedef struct
{
    SPI_HandleTypeDef *hspi_x;
    GPIO_TypeDef *port_accel;
    uint16_t pin_accel;
    GPIO_TypeDef *port_gyro;
    uint16_t pin_gyro;
}SPI_InfoTypeDef;

typedef enum
{
    SPI,
    I2C,
    UART
}Protocol_enum;

//NOTE: 考虑削减此项
typedef enum
{
    BMI088,
    MPU6050
}IMU_enum;

typedef struct
{
    IMU_enum imu_type;
    Protocol_enum p_type;
    float bias_pitch;
    float bias_yaw;
    float bias_y;
}IMU_InfoTypeDef;

typedef struct imu_s
{
    SPI_InfoTypeDef spi;
    IMU_InfoTypeDef info;
    int16_t gyro[3];//陀螺仪原始数据
	int16_t accel[3];//加速度计
	float angle_q[4];
	float angle[3];       //互补滤波结果
    float (*pitch)(struct imu_s *imu);
    float (*yaw)(struct imu_s *imu);
    void (*update)(struct imu_s *imu);
}IMU_TypeDef;

typedef struct
{
    I2C_HandleTypeDef *hi2c_x;
    GPIO_TypeDef *port;
    uint16_t pin;
    int16_t mag[3];
}Mag_Typedef;

typedef enum
{
	accel = 0,
	gyro = 1
}accel_or_gyro;

extern IMU_TypeDef *imu_list[IMU_NUM];
extern void BMI088Update(IMU_TypeDef *imu);
extern void IMUInit();


#endif
