#include "imu.h"
#include "i2c.h"
#include "spi.h"
#include "register.h"
#include "malloc.h"
#include "string.h"
IMU_TypeDef *imu_list[IMU_NUM];
uint8_t spi_TxData, spi_RxData;

void BMI088ReadAccel(IMU_TypeDef *imu)
 {
    GPIO_TypeDef *port = imu->spi.port_accel;
    uint16_t pin = imu->spi.pin_accel;
	uint8_t temp_arr[6];
	HAL_GPIO_WritePin(port, pin, RESET);
	spi_TxData = 0x12 | 0x80;		//使地�?第一位为1（读模式�?
	HAL_SPI_TransmitReceive(imu->spi.hspi_x, &spi_TxData, &spi_RxData, 1, 300);	//写入�?要读取的地址
	spi_TxData = 0x12 | 0x80;		//使地�?第一位为1（读模式�?
	HAL_SPI_TransmitReceive(imu->spi.hspi_x, &spi_TxData, &spi_RxData, 1, 300);	//写入�?要读取的地址
	spi_TxData = 0x55;
	for(uint8_t i=0;i<6;i++)
	    {
			HAL_SPI_TransmitReceive(imu->spi.hspi_x, &spi_TxData, &spi_RxData, 1, 300);
	        temp_arr[i] = spi_RxData;
	    }
	imu->accel[0] = (temp_arr[0] + (temp_arr[1] << 8));
	imu->accel[1] = (temp_arr[2] + (temp_arr[3] << 8));
	imu->accel[2] = (temp_arr[4] + (temp_arr[5] << 8));

	HAL_GPIO_WritePin(port, pin, SET);    //传输停止
 }

void BMI088ReadGyro(IMU_TypeDef *imu)
 {
    GPIO_TypeDef *port = imu->spi.port_gyro;
    uint16_t pin = imu->spi.pin_gyro;
	uint8_t temp_arr[6];
	HAL_GPIO_WritePin(port, pin, RESET);
	spi_TxData = 0x02 | 0x80;		                       //使地�?第一位为1（读模式�?
	HAL_SPI_Transmit(imu->spi.hspi_x, &spi_TxData, 1, 300);	       //写入�?要读取的地址
//	while(HAL_SPI_GetState(imu->hspi_x) == HAL_SPI_STATE_BUSY);
	spi_TxData = 0x55;
	for(int i=0; i<6; i++)                                 //接受读取信息
	{
		HAL_SPI_TransmitReceive(imu->spi.hspi_x, &spi_TxData, &spi_RxData, 1, 300);
		temp_arr[i] = spi_RxData;
//		while(HAL_SPI_GetState(imu->hspi_x) == HAL_SPI_STATE_BUSY);
	}

	imu->gyro[0] = (temp_arr[0] + (temp_arr[1] << 8));
	imu->gyro[1] = (temp_arr[2] + (temp_arr[3] << 8));
	imu->gyro[2] = (temp_arr[4] + (temp_arr[5] << 8));
	HAL_GPIO_WritePin(port, pin, SET);    //传输停止

 }

void BMI088WriteByte(IMU_TypeDef *imu, uint8_t write_data, uint8_t addr, accel_or_gyro a_enum)
{
	switch(a_enum)
	{
	case accel:
		HAL_GPIO_WritePin(imu->spi.port_accel, imu->spi.pin_accel, RESET);
		break;
    case gyro:
		HAL_GPIO_WritePin(imu->spi.port_gyro, imu->spi.pin_gyro, RESET);
		break;
	}
	spi_TxData = addr & 0x7F;                    //bit0为0，向imu写
	HAL_SPI_Transmit(imu->spi.hspi_x, &spi_TxData, 1, 500);
	while(HAL_SPI_GetState(imu->spi.hspi_x)==HAL_SPI_STATE_BUSY_TX);
	HAL_SPI_Transmit(imu->spi.hspi_x, &write_data, 1, 500);
	HAL_Delay(30);
	switch(a_enum)
	{
	case accel:
	    HAL_GPIO_WritePin(imu->spi.port_accel, imu->spi.pin_accel, SET);
		break;
	case gyro:
		HAL_GPIO_WritePin(imu->spi.port_gyro, imu->spi.pin_gyro, SET);
		break;
	}
}

void IST8310WriteByte(Mag_Typedef *mag, uint8_t write_data, uint8_t addr)
{
	HAL_I2C_Mem_Write(mag->hi2c_x, (IST8310_I2C_ADDR << 1), addr,
			I2C_MEMADD_SIZE_8BIT, &write_data, 1, 50);
}

void IST8310Read(Mag_Typedef *mag)
{
	uint8_t read_buf[6];
	HAL_I2C_Mem_Read(mag->hi2c_x, (IST8310_I2C_ADDR << 1), IST8310_DATA_XL_ADDR, I2C_MEMADD_SIZE_8BIT, read_buf, 6, 50);
	mag->mag[0] = read_buf[0] + (read_buf[1] << 8);
	mag->mag[1] = read_buf[2] + (read_buf[3] << 8);
	mag->mag[2] = read_buf[4] + (read_buf[5] << 8);
}
void IST8310Init(Mag_Typedef *mag)
{
	HAL_GPIO_WritePin(mag->port, mag->pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(mag->port, mag->pin, GPIO_PIN_SET);
	HAL_Delay(50);
	IST8310WriteByte(mag, 0X00, IST8310_STAT2_ADDR);
	IST8310WriteByte(mag, IST8310_AVGCNTL_FOURTH, IST8310_AVGCNTL_ADDR);//平均采样四次
	IST8310WriteByte(mag, IST8310_CNTL1_CONTINUE, IST8310_CNTL1_ADDR);//输出频率200Hz
}

void BMI088Init(IMU_TypeDef *imu)
{

        //加速度计初始化
        BMI088WriteByte(imu, 0xB6, 0x7E, accel);            //向0x7E写入0xb6以软件复位加速度计
        BMI088WriteByte(imu, 0x04, 0x7D, accel);            //向0x7D写入0x04以取消加速度计暂停
        BMI088WriteByte(imu, 0x00,0x41, accel);             //设置量程为±3g
        BMI088WriteByte(imu, 0x89,0x40 , accel);

	    //陀螺仪初始化
        BMI088WriteByte(imu, 0xB6, 0x14, gyro);             //向0x14写入0xb6以软件复位陀螺仪
        BMI088WriteByte(imu, 0x00,  0x11, gyro);
        BMI088WriteByte(imu, GYRO_RANGE_500_DEG_S, GYRO_RANGE_ADDR, gyro);//±500
        BMI088WriteByte(imu, GYRO_ODR_200Hz_BANDWIDTH_64Hz, GYRO_BANDWIDTH_ADDR, gyro);

}


/*
 * @brief  	读取BMI088原始数据，减零偏、滤波
 * @param	IMU结构体指针
 * @retval 	无，结果在结构体中
 * @note    以200Hz运行
 */
void BMI088Update(IMU_TypeDef *imu)
{
	 /***读取imu***/
		 BMI088ReadGyro(imu);
		 BMI088ReadAccel(imu);
	//	 IST8310_read(&imu_data);
		 /***量程转换***/
         float imu_gyro[3],imu_accel[3];
		 for(int i=0;i<3;i++)
		 {

				 imu_gyro[i] = (imu->gyro[i])/65.536*(PI/180);

				 imu_accel[i] = imu->accel[i]*0.0008974f;
	//		 imu_mag[i] = imu->mag[i]*0.3;

		 }
		 /***减去零偏值***/
		 imu_gyro[1] -= imu->info.bias_pitch;
		 imu_gyro[2] -= imu->info.bias_yaw;  
		 imu_accel[1] -= imu->info.bias_y;

		 /***互补滤波***/
		 MahonyAHRSupdateIMU(imu->angle_q, imu_gyro[0], imu_gyro[1], imu_gyro[2], imu_accel[0], imu_accel[1], imu_accel[2]);
		 imu->angle[0] = atan2f(2.0f*(imu->angle_q[0]*imu->angle_q[3]+imu->angle_q[1]*imu->angle_q[2]), 2.0f*(imu->angle_q[0]*imu->angle_q[0]+imu->angle_q[1]*imu->angle_q[1])-1.0f);
		 imu->angle[1] = asinf(-2.0f*(imu->angle_q[1]*imu->angle_q[3]-imu->angle_q[0]*imu->angle_q[2]));
		 imu->angle[2] = atan2f(2.0f*(imu->angle_q[0]*imu->angle_q[1]+imu->angle_q[2]*imu->angle_q[3]),2.0f*(imu->angle_q[0]*imu->angle_q[0]+imu->angle_q[3]*imu->angle_q[3])-1.0f);

}

float IMUGetPitch(IMU_TypeDef *imu)
{
    return imu->angle[1];
}

float IMUGetYaw(IMU_TypeDef *imu)
{
    return imu->angle[0];
}

void IMUInit()
{
    IMU_TypeDef imu1;
	imu1.spi.hspi_x = &hspi1;
	imu1.spi.port_accel = CS1_Accel_GPIO_Port;
	imu1.spi.pin_accel = CS1_Accel_Pin;
	imu1.spi.port_gyro = GPIOB;
	imu1.spi.pin_gyro = GPIO_PIN_0;

	imu1.info.p_type = SPI;
	imu1.info.imu_type = BMI088;
	imu1.info.bias_yaw = (10.4231017f / 65.536)*(PI/180);
	imu1.info.bias_pitch = (11.5390333f / 65.536)*(PI/180);
	imu1.info.bias_y = (141.763613f * 0.0008974);
	
	imu1.angle_q[0] = 1;
	imu1.pitch = IMUGetPitch;
	imu1.yaw = IMUGetYaw;
	imu1.update = BMI088Update;

	BMI088Init(&imu1);
	imu_list[0] = malloc(sizeof(IMU_TypeDef));
	memcpy(imu_list[0], &imu1, sizeof(IMU_TypeDef));

}
