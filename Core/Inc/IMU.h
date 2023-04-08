#ifndef _IMU_H_
#define _IMU_H_

//BMI088寄存器地址
#define BMI088_Chip_ID_Add 0X00
#define BMI088_Gyro_X_LSB_Add 0x02
#define BMI088_Gyro_X_MSB_Add 0x03
#define BMI088_Gyro_Y_LSB_Add 0x04
#define BMI088_Gyro_Y_MSB_Add 0x05
#define BMI088_Gyro_Z_LSB_Add 0x06
#define BMI088_Gyro_Z_MSB_Add 0x07
#define BMI088_Accel_X_LSB_Add 0X12
#define BMI088_Accel_X_MSB_Add 0X13
#define BMI088_Accel_Y_LSB_Add 0X14
#define BMI088_Accel_Y_MSB_Add 0X15
#define BMI088_Accel_Z_LSB_Add 0X16
#define BMI088_Accel_Z_MSB_Add 0X17
#define BMI088_Temp_MSB_Add 0x22    //低三位  [2:0]
#define BMI088_Temp_LSB_Add 0x23    //高8位   [10:3]
#define GYRO_RANGE_ADDR 0x0F
#define GYRO_RANGE_2000_DEG_S 0x00
#define GYRO_RANGE_1000_DEG_S 0x01
#define GYRO_RANGE_500_DEG_S 0x02
#define GYRO_RANGE_250_DEG_S 0x03
#define GYRO_RANGE_125_DEG_S 0x04
#define GYRO_BANDWIDTH_ADDR 0x10
#define GYRO_ODR_2000Hz_BANDWIDTH_532Hz 0x00
#define GYRO_ODR_2000Hz_BANDWIDTH_230Hz 0x01
#define GYRO_ODR_1000Hz_BANDWIDTH_116Hz 0x02
#define GYRO_ODR_400Hz_BANDWIDTH_47Hz 0x03
#define GYRO_ODR_200Hz_BANDWIDTH_23Hz 0x04
#define GYRO_ODR_100Hz_BANDWIDTH_12Hz 0x05
#define GYRO_ODR_200Hz_BANDWIDTH_64Hz 0x06
#define GYRO_ODR_100Hz_BANDWIDTH_32Hz 0x07

//IST8310寄存器地址
#define IST8310_CHIP_ID_ADDR 0x00
#define IST8310_CHIP_ID_VAL 0x10
#define IST8310_STAT1_ADDR 0x02
#define IST8310_DATA_XL_ADDR 0x03
#define IST8310_DATA_XH_ADDR 0x04
#define IST8310_DATA_YL_ADDR 0x05
#define IST8310_DATA_YH_ADDR 0x06
#define IST8310_DATA_ZL_ADDR 0x07
#define IST8310_DATA_ZH_ADDR 0x08
#define IST8310_STAT2_ADDR 0x09
#define IST8310_CNTL1_ADDR 0x0A
#define IST8310_CNTL1_SLEEP 0x00
#define IST8310_CNTL1_SINGLE 0x01
#define IST8310_CNTL1_CONTINUE 0x0B
#define IST8310_CNTL2_ADDR 0x0B
#define IST8310_STAT2_NONE_ALL 0x00
#define IST8310_SELF_CHECK_ADDR 0x0C
#define IST8310_TEMPL_ADDR 0x1C
#define IST8310_TEMPH_ADDR 0x1D
#define IST8310_AVGCNTL_ADDR 0x41
#define IST8310_AVGCNTL_TWICE 0x09
#define IST8310_AVGCNTL_FOURTH 0x12

typedef struct
{
	int16_t gyro[3];//陀螺仪
	int16_t accel[3];//加速度计
	int16_t mag[3];//磁力计
	float angle_q[4];
	float angle[3];
	uint8_t temp;
	int sensitivity;

}IMU_TypeDef;
typedef enum
{
	accel = 0,
	gyro = 1
}accel_or_gyro;
extern IMU_TypeDef imu_data;
extern float imu_gyro[3],imu_accel[3],imu_mag[3];
extern void BMI088_read_Gyro(IMU_TypeDef *imu);
extern void BMI088_read_Accel(IMU_TypeDef *imu);
extern void BMI088_init();
extern void IST8310_init();
extern void IST8310_read(IMU_TypeDef *imu);
extern void IST8310_write_byte(uint8_t write_data, uint8_t addr);
#endif
