#include<stm32f10x.h>
#include"Delay.h"

#define SCL_PORT GPIOB
#define SCL_PIN GPIO_Pin_10		// SCL时钟引脚宏定义，方便后续移植修改端口
#define SDA_PIN GPIO_Pin_11		// SDA数据引脚宏定义

// 写SCL电平：输出指定电平到SCL时钟线，并插入延时保证时序
void MyI2C_W_SCL(uint8_t BitValue){
	GPIO_WriteBit(SCL_PORT, SCL_PIN, (BitAction)BitValue);
	Delay_us(10);	// 电平保持延时，满足I2C最小脉宽要求，给从机预留采样/响应时间
}

// 写SDA电平：输出指定电平到SDA数据线，并插入延时保证时序
void MyI2C_W_SDA(uint8_t BitValue){
	GPIO_WriteBit(SCL_PORT, SDA_PIN, (BitAction)BitValue);
	Delay_us(10);	// 电平保持延时，满足I2C数据建立/保持时间要求
}

// 读SDA电平：读取SDA总线当前真实电平
uint8_t MyI2C_R_SDA(){
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(SCL_PORT, SDA_PIN);	// 读取IDR寄存器，获取引脚外部真实电平
	Delay_us(10);	// 采样后延时，保证SCL高电平持续时间，对齐整体时序节拍
	return BitValue;
}

// 软件模拟I2C初始化：配置GPIO端口，模拟I2C总线时序
void MyI2C_Init(){
	// 开启GPIOB端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	// 配置为开漏输出模式：I2C为双向共享总线，多设备挂载时推挽输出会造成电平冲突、甚至短路
	// 开漏输出写1时为高阻态，配合外部上拉电阻实现双向通信，符合I2C总线物理规范
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = SCL_PIN | SDA_PIN;	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_SetBits(GPIOB, SCL_PIN | SDA_PIN);		// 上电默认输出1，释放I2C总线，处于空闲状态
	
}

// 起始条件Start：SCL高电平期间，SDA由高变低
// 设计兼容SR重复起始：先释放SDA再释放SCL，避免通信中间调用时误触发停止条件
void MyI2C_Start(){
	// 先依次释放SDA、SCL，回到双高电平状态
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);		
	
	// 产生起始标志：SCL保持高电平，SDA由高拉低
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);	// 拉低SCL，进入位传输准备状态
}

// 停止条件Stop：SCL高电平期间，SDA由低变高
void MyI2C_Stop(){
	// 先拉低SDA，确保后续能产生SDA上升沿
	MyI2C_W_SDA(0);
	
	// 产生停止标志：先拉高SCL，再拉高SDA
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);	// SCL高电平时SDA由低到高，总线回到空闲状态
}

// 发送1字节数据，高位先行（MSB first）
void MyI2C_SendByte(uint8_t Byte){
	for (int i = 0; i < 8; i++){
		// SCL低电平期间，将当前数据位放到SDA总线上（SCL低时允许SDA变化）
		MyI2C_W_SDA(Byte & (0x80 >> i));	// 从最高位bit7到最低位bit0依次取出
		MyI2C_W_SCL(1);				// SCL拉高，进入采样窗口，从机在SCL高电平时读取数据
		MyI2C_W_SCL(0);				// SCL拉低，进入下一位的电平切换窗口
	}
}

// 接收1字节数据，高位先行（MSB first）
uint8_t MyI2C_ReceiveByte(){
	uint8_t Byte = 0x00;		// 接收数据缓存，初始化为0
	MyI2C_W_SDA(1);				// 主机释放SDA总线，交给从机驱动输出数据
	
	for (int i = 0; i < 8; i++){
		MyI2C_W_SCL(1);			// SCL拉高，进入采样窗口，SDA此时电平稳定
		if (MyI2C_R_SDA()){		// 读取当前位，为1则写入对应位置
			Byte |= 0x80 >> i;
		}
		MyI2C_W_SCL(0);			// SCL拉低，允许从机切换下一位数据
	}
	return Byte;
}

// 主机发送应答位（ACK/NACK）：主机接收完字节后，向从机反馈
void MyI2C_SendACK(uint8_t ACKBit){
	// SCL低电平期间，主机设置SDA应答电平
	MyI2C_W_SDA(ACKBit);		// 0=ACK应答，1=NACK非应答（结束传输）
	MyI2C_W_SCL(1);				// SCL拉高，从机在高电平时采样应答位
	MyI2C_W_SCL(0);				// SCL拉低，应答位传输结束
}

// 主机接收应答位：主机发送完字节后，读取从机的应答反馈
uint8_t MyI2C_ReceiveACK(){
	uint8_t ACKBit;
	MyI2C_W_SDA(1);			// 主机释放SDA，交给从机输出应答电平
	MyI2C_W_SCL(1);			// SCL拉高，进入采样窗口
	ACKBit = MyI2C_R_SDA();	// 读取从机应答：0=ACK正常应答，1=NACK无应答
	MyI2C_W_SCL(0);
	return ACKBit;
}
