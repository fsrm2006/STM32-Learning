#include <stm32f1xx_hal.h>
#include "MPU6050Reg.h"     // MPU6050寄存器地址宏定义头文件
//#include "i2c.h"
#ifdef __I2C_H__


// MPU6050 7位从机地址为0x68，左移1位拼接写方向位(0) = 0xD0
// 注意：HAL库I2C存储器读写函数，DevAddress统一传写地址即可，读操作库内部自动切换方向
#define MPU6050_Addr 0xD0

/**
 * @brief  向MPU6050指定寄存器写入1字节数据
 * @param  RegAddr：目标寄存器地址
 * @param  Data：待写入的1字节数据
 * @note   HAL库已封装完整时序：起始→从机写地址→寄存器地址→写入数据→停止
 *         底层自动处理EV5/EV6/EV8事件与标志位清除，无需手动控制时序
 * @note   第4个参数为寄存器地址宽度：I2C_MEMADD_SIZE_8BIT表示8位地址(占1字节)
 *         不可填数字8！该参数代表地址的字节长度，不是位数
 */
void MPU6050_WriteReg(uint8_t RegAddr, uint8_t Data){
	HAL_I2C_Mem_Write(&hi2c2, MPU6050_Addr, RegAddr, I2C_MEMADD_SIZE_8BIT, &Data, 1, 100);
}

/**
 * @brief  读取MPU6050指定寄存器的1字节数据
 * @param  RegAddr：目标寄存器地址
 * @retval 寄存器读取结果
 * @note   HAL库内部自动实现「先写地址设置指针 + 重复起始SR切换读方向」的复合时序
 *         无需手动处理重复起始、方向切换、应答控制
 * @note   DevAddress仍传写地址0xD0，库内部会自动拼接读方向位
 */
uint8_t MPU6050_ReadReg(uint8_t RegAddr){
	uint8_t Data;
	HAL_I2C_Mem_Read(&hi2c2, MPU6050_Addr, RegAddr, I2C_MEMADD_SIZE_8BIT, &Data, 1, 100);
	return Data;
}

/**
 * @brief  MPU6050工作寄存器初始化
 * @note   I2C外设初始化、GPIO复用配置、时钟开启均由CubeMX自动生成(MX_I2C2_Init)
 *         本函数仅配置传感器内部寄存器，实现解除睡眠、设置量程与采样率
 * @note   重点：MPU6050上电默认处于睡眠模式，必须先配置PWR_MGMT_1解除睡眠
 *         未解除睡眠时所有传感器停止工作，读取寄存器会返回错误值
 */
void MPU6050_Init(){
	// 电源管理寄存器1：解除睡眠，选择陀螺仪X轴PLL作为时钟源，精度更高
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
	// 电源管理寄存器2：所有轴全功率工作，无待机降功耗
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
	// 采样率分频：陀螺仪基准输出1kHz / (9+1) = 100Hz采样率
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
	// 陀螺仪配置：满量程 ±2000°/s
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	// 加速度计配置：满量程 ±16g
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

/**
 * @brief  读取6轴原始数据：加速度计X/Y/Z轴 + 陀螺仪X/Y/Z轴
 * @param  ACC_X/ACC_Y/ACC_Z：加速度计原始值输出指针，16位有符号补码
 * @param  GYRO_X/GYRO_Y/GYRO_Z：陀螺仪原始值输出指针，16位有符号补码
 * @note   数据分高、低8位两个寄存器存储，高8位在前，地址连续排列
 * @note   可优化：利用从机地址指针自动自增特性，指定起始地址后连续读取14字节
 *         替代当前逐个寄存器读取的方式，大幅降低通信开销、提升读取效率
 */
void MPU6050_GetData(int16_t *ACC_X, int16_t *ACC_Y, int16_t *ACC_Z,
	int16_t *GYRO_X, int16_t *GYRO_Y, int16_t *GYRO_Z){
		
	uint8_t DataH, DataL;	// 高8位、低8位寄存器缓存，拼接为16位原始补码值
	
	// 加速度计 X轴
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*ACC_X = (DataH << 8) | DataL;
	
	// 加速度计 Y轴
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*ACC_Y = (DataH << 8) | DataL;
	
	// 加速度计 Z轴
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*ACC_Z = (DataH << 8) | DataL;
	
	// 陀螺仪 X轴
	DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GYRO_X = (DataH << 8) | DataL;
		
	// 陀螺仪 Y轴
	DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GYRO_Y = (DataH << 8) | DataL;
	
	// 陀螺仪 Z轴
	DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GYRO_Z = (DataH << 8) | DataL;
}

/**
 * @brief  读取设备ID寄存器WHO_AM_I
 * @retval 设备ID值，MPU6050固定返回0x68
 * @note   用于校验I2C通信链路是否正常、设备是否正确识别
 *         值正确代表硬件接线、I2C时序、从机地址均无问题
 */
uint8_t MPU6050_GetID(){
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}

#endif
