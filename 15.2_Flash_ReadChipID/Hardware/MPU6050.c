#include<stm32f10x.h>
#include"MPU6050Reg.h"		// MPU6050寄存器地址宏定义头文件

#define MPU6050_Addr 0xD0		// MPU6050 7位从机地址为0x68，左移1位拼接写位(0)，即 0x68<<1 = 0xD0


// 封装事件等待函数，带超时防死机制
// 硬件I2C事件未触发时程序会阻塞在此，超时计数归零后强制跳出，避免硬件异常导致程序卡死
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT){
	uint32_t Timeout = 10000;		// 超时时限计数
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS){
		Timeout--;
		if (Timeout == 0){
			// 可扩展错误处理逻辑
			break;
		}
	}
}


// 指定地址写：向MPU6050指定寄存器写入1字节数据
// 硬件时序：起始 → 从机写地址 → 寄存器地址 → 写入数据 → 停止
void MPU6050_WriteReg(uint8_t RegAddr, uint8_t Data){
	
	I2C_GenerateSTART(I2C2, ENABLE);							// 硬件生成起始条件S
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);		// 等待EV5：起始条件生成完成，SB标志置1
	
	I2C_Send7bitAddress(I2C2, MPU6050_Addr, I2C_Direction_Transmitter);	// 发送从机地址+写位
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	// 等待EV6：地址发送完成，从机应答，ADDR标志置1
	
	I2C_SendData(I2C2, RegAddr);								// 写入寄存器地址，设置从机内部地址指针
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);	// 等待EV8：DR寄存器空，移位寄存器正在发送，可写入下一字节
	
	I2C_SendData(I2C2, Data);									// 写入要发送的数据
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	// 等待EV8_2：最后一字节传输完成，BTF标志置1
	
	I2C_GenerateSTOP(I2C2, ENABLE);								// 硬件生成停止条件P
}
	

// 指定地址读：读取MPU6050指定寄存器的1字节数据
// 通信逻辑：先写模式设置地址指针 → 重复起始SR切换读模式 → 读取单字节
uint8_t MPU6050_ReadReg(uint8_t RegAddr){
	
	I2C_GenerateSTART(I2C2, ENABLE);							// 生成起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);		// 等待EV5：起始完成
	
	I2C_Send7bitAddress(I2C2, MPU6050_Addr, I2C_Direction_Transmitter);	// 发送写模式地址
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	// 等待EV6：地址应答完成
	
	I2C_SendData(I2C2, RegAddr);								// 发送目标寄存器地址
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);	// 等待EV8_2：寄存器地址发送完成
	
	I2C_GenerateSTART(I2C2, ENABLE);							// 生成重复起始条件SR
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);		// 等待EV5：重复起始完成
	
	I2C_Send7bitAddress(I2C2, MPU6050_Addr, I2C_Direction_Receiver);	// 发送读模式地址，切换为接收方向
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);	// 等待EV6：读地址应答完成
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);						// 提前关闭应答，配置为NACK（单字节接收必须提前设置）
	I2C_GenerateSTOP(I2C2, ENABLE);								// 预约停止条件：硬件收完当前字节后自动生成停止，不会打断当前传输
	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);	// 等待EV7：字节接收完成，RxNE标志置1
	uint8_t Data = I2C_ReceiveData(I2C2);						// 读取DR寄存器，获取接收到的字节
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);						// 恢复应答使能，不影响后续通信
	
	return Data;
}


// MPU6050初始化：GPIO复用配置 + I2C外设初始化 + 传感器核心寄存器配置
void MPU6050_Init(){
	// 开启外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);		// I2C2挂载APB1总线，对应引脚PB10(SCL)、PB11(SDA)
	
	// GPIO配置为复用功能模式，引脚控制权交给I2C外设，由硬件自动生成总线时序
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
	// I2C外设参数配置
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;					// 应答使能：默认接收字节后自动回复ACK
	I2C_InitStruct.I2C_ClockSpeed = 100000; 					// 通信速率：标准模式100kHz
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;				// 快速模式SCL占空比，标准模式下无效
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;						// 工作模式：标准I2C模式
	// 以下为从机模式配置，主机模式下不生效
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	// 从机地址位数：7位
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;						// 从机自身地址，主机模式可忽略
	I2C_Init(I2C2, &I2C_InitStruct);
	
	// 使能I2C2外设
	I2C_Cmd(I2C2, ENABLE);
	
	// MPU6050工作寄存器配置
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);	// 电源管理1：解除睡眠，选择陀螺仪X轴为时钟源
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);	// 电源管理2：所有轴全功率工作，无待机降功耗
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);	// 采样率分频：1kHz基准 / (9+1) = 100Hz采样率
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	// 陀螺仪配置：满量程±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	// 加速度计配置：满量程±16g
}
	

// 读取6轴原始数据：加速度计X/Y/Z + 陀螺仪X/Y/Z
// 指针传参：直接修改外部变量，无需多返回值；可改用结构体指针打包传递
// 可优化：寄存器地址连续，可配置MPU地址自增，一次连续读取14字节，大幅降低通信开销
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

// 读取设备ID：WHO_AM_I寄存器固定返回0x68，用于校验I2C通信是否正常、设备是否正确识别
uint8_t MPU6050_GetID(){
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}
