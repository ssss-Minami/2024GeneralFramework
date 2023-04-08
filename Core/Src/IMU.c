#include "main.h"
#include "spi.h"
#include "IMU.h"
#include "usart.h"
#include "i2c.h"
IMU_TypeDef imu_data;
float imu_gyro[3],imu_accel[3],imu_mag[3];
uint8_t spi_TxData, spi_RxData;

void BMI088_read_Accel(IMU_TypeDef *imu)
 {
	uint8_t temp_arr[6];
	HAL_GPIO_WritePin(CS1_Accel_GPIO_Port, CS1_Accel_Pin, RESET);
	spi_TxData = 0x12 | 0x80;		//使地�?第一位为1（读模式�?
	HAL_SPI_TransmitReceive(&hspi1, &spi_TxData, &spi_RxData, 1, 300);	//写入�?要读取的地址
	spi_TxData = 0x12 | 0x80;		//使地�?第一位为1（读模式�?
	HAL_SPI_TransmitReceive(&hspi1, &spi_TxData, &spi_RxData, 1, 300);	//写入�?要读取的地址
	spi_TxData = 0x55;
	for(uint8_t i=0;i<6;i++)
	    {
			HAL_SPI_TransmitReceive(&hspi1, &spi_TxData, &spi_RxData, 1, 300);
	        temp_arr[i] = spi_RxData;
	    }
	imu->accel[0] = (temp_arr[0] + (temp_arr[1] << 8));
	imu->accel[1] = (temp_arr[2] + (temp_arr[3] << 8));
	imu->accel[2] = (temp_arr[4] + (temp_arr[5] << 8));

	HAL_GPIO_WritePin(CS1_Accel_GPIO_Port, CS1_Accel_Pin, SET);    //传输停止
 }

void BMI088_read_Gyro(IMU_TypeDef *imu)
 {
	uint8_t temp_arr[6];
	HAL_GPIO_WritePin(CS1_Gyro_GPIO_Port, CS1_Gyro_Pin, RESET);
	spi_TxData = 0x02 | 0x80;		                       //使地�?第一位为1（读模式�?
	HAL_SPI_Transmit(&hspi1, &spi_TxData, 1, 300);	       //写入�?要读取的地址
//	while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY);
	spi_TxData = 0x55;
	for(int i=0; i<6; i++)                                 //接受读取信息
	{
		HAL_SPI_TransmitReceive(&hspi1,&spi_TxData, &spi_RxData, 1, 300);
		temp_arr[i] = spi_RxData;
//		while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY);
	}

	imu->gyro[0] = (temp_arr[0] + (temp_arr[1] << 8));
	imu->gyro[1] = (temp_arr[2] + (temp_arr[3] << 8));
	imu->gyro[2] = (temp_arr[4] + (temp_arr[5] << 8));
	HAL_GPIO_WritePin(CS1_Gyro_GPIO_Port, CS1_Gyro_Pin, SET);    //传输停止

 }

void BMI088_write_byte(uint8_t write_data, uint8_t addr, accel_or_gyro a_enum)
{
	switch(a_enum)
	{
	case accel:
		HAL_GPIO_WritePin(CS1_Accel_GPIO_Port, CS1_Accel_Pin, RESET);
		break;
    case gyro:
		HAL_GPIO_WritePin(CS1_Gyro_GPIO_Port, CS1_Gyro_Pin, RESET);
		break;
	}
	spi_TxData = addr & 0x7F;                    //bit0为0，向imu写
	HAL_SPI_Transmit(&hspi1, &spi_TxData, 1, 500);
	while(HAL_SPI_GetState(&hspi1)==HAL_SPI_STATE_BUSY_TX);
	HAL_SPI_Transmit(&hspi1, &write_data, 1, 500);
	HAL_Delay(30);
	switch(a_enum)
	{
	case accel:
	    HAL_GPIO_WritePin(CS1_Accel_GPIO_Port, CS1_Accel_Pin, SET);
		break;
	case gyro:
		HAL_GPIO_WritePin(CS1_Gyro_GPIO_Port, CS1_Gyro_Pin, SET);
		break;
	}
}

void BMI088_init()
{
	//加速度计初始化
    BMI088_write_byte(0xB6, 0x7E, accel);            //向0x7E写入0xb6以软件复位加速度计
	BMI088_write_byte(0x04, 0x7D, accel);            //向0x7D写入0x04以取消加速度计暂停
	BMI088_write_byte(0x00,0x41,accel);//设置量程为±3g
	BMI088_write_byte(0x89,0x40 , accel);

	//陀螺仪初始化
	BMI088_write_byte(0xB6, 0x14, gyro);             //向0x14写入0xb6以软件复位陀螺仪
	BMI088_write_byte(0x00,  0x11, gyro);
	BMI088_write_byte(GYRO_RANGE_500_DEG_S, GYRO_RANGE_ADDR, gyro);//±500
	BMI088_write_byte(GYRO_ODR_200Hz_BANDWIDTH_64Hz, GYRO_BANDWIDTH_ADDR, gyro);
	imu_data.sensitivity = 1;

}

void IST8310_write_byte(uint8_t write_data, uint8_t addr)
{
	HAL_I2C_Mem_Write(&hi2c3, (IST8310_I2C_ADDR << 1), addr,
			I2C_MEMADD_SIZE_8BIT, &write_data, 1, 50);
}

void IST8310_read(IMU_TypeDef *imu)
{
	uint8_t read_buf[6];
	HAL_I2C_Mem_Read(&hi2c3, (IST8310_I2C_ADDR << 1), IST8310_DATA_XL_ADDR, I2C_MEMADD_SIZE_8BIT, read_buf, 6, 50);
	imu->mag[0] = read_buf[0] + (read_buf[1] << 8);
	imu->mag[1] = read_buf[2] + (read_buf[3] << 8);
	imu->mag[2] = read_buf[4] + (read_buf[5] << 8);
}
void IST8310_init()
{
	HAL_GPIO_WritePin(IST8310_Reset_GPIO_Port, IST8310_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(IST8310_Reset_GPIO_Port, IST8310_Reset_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	IST8310_write_byte(0X00, IST8310_STAT2_ADDR);
	IST8310_write_byte(IST8310_AVGCNTL_FOURTH, IST8310_AVGCNTL_ADDR);//平均采样四次
	IST8310_write_byte(IST8310_CNTL1_CONTINUE, IST8310_CNTL1_ADDR);//输出频率200Hz
}
