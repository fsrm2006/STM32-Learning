#include<stm32f10x.h>
#include"MyI2C.h"
#include"MPU6050Reg.h"		// 存放MPU6050各类寄存器地址的宏定义头文件

#define MPU6050_Addr 0xD0		// MPU6050 7位从机地址为0x68，左移1位后拼接写位(0)，即 (0x68<<1) | 0 = 0xD0


// 指定地址写：向MPU6050的指定寄存器写入1字节数据
// 通信流程：起始条件 → 发从机写地址 → 收从机应答 → 发寄存器地址 → 收应答 → 发写入数据 → 收应答 → 停止条件
void MPU6050_WriteReg(uint8_t RegAddr, uint8_t Data){
	MyI2C_Start();

	MyI2C_SendByte(MPU6050_Addr);	// 发送从机地址+写位，指定通信对象为MPU6050、通信模式为写

	if (MyI2C_ReceiveACK() == 0){	// 收到从机应答（0=ACK），确认从机在线，继续后续传输
		 MyI2C_SendByte(RegAddr);	// 发送寄存器地址，设置从机内部的地址指针
		 MyI2C_ReceiveACK();		// 接收寄存器地址对应的应答
		 MyI2C_SendByte(Data);		// 向地址指针指向的寄存器写入目标数据
		 MyI2C_ReceiveACK();		// 接收数据写入对应的应答
	}

	MyI2C_Stop();
}
	

// 指定地址读：读取MPU6050指定寄存器的1字节数据
// 通信逻辑：先以写模式设置从机内部地址指针，再通过重复起始(SR)切换为读模式读取数据
uint8_t MPU6050_ReadReg(uint8_t RegAddr){
	
	MyI2C_Start();
	MyI2C_SendByte(MPU6050_Addr);	// 先发送写模式地址，目的是写入寄存器地址、设置从机地址指针
	MyI2C_ReceiveACK();		// 接收从机地址应答
	MyI2C_SendByte(RegAddr);
	MyI2C_ReceiveACK();		// 接收寄存器地址应答
	
	MyI2C_Start();			// 复用Start函数作为SR重复起始条件：该函数先拉高SDA再拉高SCL，不会触发Stop条件，符合重复起始时序
	MyI2C_SendByte(MPU6050_Addr | 0x01);	// 发送从机地址+读位，切换为读模式
	MyI2C_ReceiveACK();
	
	uint8_t Data = MyI2C_ReceiveByte();		// 读取当前地址指针指向的寄存器数据
	MyI2C_SendACK(1);		// 发送NACK(1)，通知从机结束数据传输，不再继续输出下一字节（仅读取单字节）
	
	MyI2C_Stop();
	
	return Data;
}


// MPU6050初始化：I2C外设初始化 + 核心工作寄存器配置
void MPU6050_Init(){
	MyI2C_Init();

	// 电源管理寄存器1 (PWR_MGMT_1，地址0x6B)，写入值 0x01
	// 位功能说明（从高位bit7到低位bit0）：
	// bit7 DEVICE_RESET：0 = 不复位设备；1 = 复位所有寄存器至默认值
	// bit6 SLEEP：0 = 解除睡眠模式，传感器正常工作；1 = 睡眠模式（上电默认值0x40即此位置1）
	// bit5 CYCLE：0 = 不进入周期唤醒模式；1 = 低功耗周期采样模式
	// bit3 TEMP_DIS：0 = 开启内部温度传感器；1 = 禁用温度传感器
	// bit2~bit0 CLKSEL[2:0]：001 = 选择PLL以陀螺仪X轴为时钟源（精度更高，标准配置）
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
	
	// 电源管理寄存器2 (PWR_MGMT_2，地址0x6C)，写入值 0x00
	// 位功能说明：
	// bit7~bit6 LP_WAKE_CTRL[1:0]：00 = 低功耗唤醒频率最低（1.25Hz）
	// bit5~bit3 STBY_XA / STBY_YA / STBY_ZA：0 = 加速度计对应轴正常工作；1 = 对应轴待机降功耗
	// bit2~bit0 STBY_XG / STBY_YG / STBY_ZG：0 = 陀螺仪对应轴正常工作；1 = 对应轴待机降功耗
	// 全0配置：所有轴全功率工作，无待机
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
	
	// 采样率分频寄存器 (SMPLRT_DIV，地址0x19)，写入值 0x09，分频系数为9
	// 采样率计算公式：采样率 = 陀螺仪输出频率 / (1 + SMPLRT_DIV)
	// 开启数字低通滤波时陀螺仪基准输出频率为1kHz，最终采样率 = 1000 / (9+1) = 100Hz
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
	
	// 陀螺仪配置寄存器 (GYRO_CONFIG，地址0x1B)，写入值 0x18（二进制 0001 1000）
	// 位功能说明：
	// bit7~bit5 XG_ST / YG_ST / ZG_ST：0 = 不开启陀螺仪自检
	// bit4~bit3 FS_SEL[1:0]：11 = 陀螺仪满量程 ±2000°/s
	// 可选档位：00=±250°/s、01=±500°/s、10=±1000°/s、11=±2000°/s
	// 低3位无功能
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	
	// 加速度计配置寄存器 (ACCEL_CONFIG，地址0x1C)，写入值 0x18（二进制 0001 1000）
	// 位功能说明：
	// bit7~bit5 XA_ST / YA_ST / ZA_ST：0 = 不开启加速度计自检
	// bit4~bit3 AFS_SEL[1:0]：11 = 加速度计满量程 ±16g
	// 可选档位：00=±2g、01=±4g、10=±8g、11=±16g
	// bit2~bit0 ACCEL_HPF[2:0]：000 = 关闭加速度计高通滤波
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}
	

// 读取6轴原始数据：加速度计X/Y/Z轴 + 陀螺仪X/Y/Z轴
// 采用指针传参：通过指针直接修改外部变量，无需多返回值；也可改用结构体指针打包传递
// 可优化点：加速度计、温度、陀螺仪寄存器地址连续排列，可利用I2C地址自增特性
// 一次指定起始地址连续读取14个字节，大幅减少通信开销，提升读取效率
void MPU6050_GetData(int16_t *ACC_X, int16_t *ACC_Y, int16_t *ACC_Z,
	int16_t *GYRO_X, int16_t *GYRO_Y, int16_t *GYRO_Z){
		
	uint8_t DataH, DataL;	// H存储高8位寄存器值、L存储低8位寄存器值，拼接后为16位原始采样值
	
	// 加速度计 X轴
	DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*ACC_X = (DataH << 8) | DataL;	// 高8位左移8位，与低8位拼接为16位有符号整数（补码形式）
		
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

// 读取设备ID寄存器 WHO_AM_I（地址0x75）
// MPU6050固定返回值为0x68，用于校验I2C通信是否正常、设备是否正确识别
uint8_t MPU6050_GetID(){
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}
