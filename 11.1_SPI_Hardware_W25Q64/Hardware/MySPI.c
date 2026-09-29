#include<stm32f10x.h>

// 硬件SPI底层驱动（SPI1，模式0，CPOL=0 CPHA=0，高位先行）
// 引脚映射：PA4=SS片选（GPIO模拟控制）、PA5=SCK时钟、PA6=MISO主机输入、PA7=MOSI主机输出

// 控制SS片选引脚电平，0=拉低选中从机，1=拉高释放从机
void MySPI_W_SS(uint8_t BitValue){
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)BitValue);
}

// 硬件SPI初始化
void MySPI_Init(){
	// 开启GPIOA端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	// 开启SPI1外设工作时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// 配置PA4为推挽输出，作为SS片选控制引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 配置PA6为上拉输入，作为MISO数据接收引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 配置PA5(SCK)、PA7(MOSI)为复用推挽输出，引脚控制权交给SPI外设
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
	// SPI外设参数配置
	SPI_InitTypeDef SPI_InitSturct;
	
	// 时钟相位：第一个时钟沿采样数据，对应CPHA=0
	SPI_InitSturct.SPI_CPHA = SPI_CPHA_1Edge;
	// 时钟极性：空闲时SCL为低电平，对应CPOL=0，组合为SPI模式0
	SPI_InitSturct.SPI_CPOL = SPI_CPOL_Low;
	// CRC校验多项式，默认值7，普通通信无需CRC可保持默认
	SPI_InitSturct.SPI_CRCPolynomial = 7;
	// 数据帧长度：8位数据宽度
	SPI_InitSturct.SPI_DataSize = SPI_DataSize_8b;
	// 位序：高位先行，符合标准SPI协议约定
	SPI_InitSturct.SPI_FirstBit = SPI_FirstBit_MSB;
	// 工作模式：SPI主机模式
	SPI_InitSturct.SPI_Mode = SPI_Mode_Master;
	// 片选控制：软件模式，由GPIO独立控制SS引脚
	SPI_InitSturct.SPI_NSS = SPI_NSS_Soft;
	// 通信方向：双线全双工模式
	SPI_InitSturct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	// 波特率分频系数：APB2总线72MHz / 64分频 = 1.125MHz通信速率
	SPI_InitSturct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_Init(SPI1, &SPI_InitSturct);
	
	// 使能SPI1外设，开始工作
	SPI_Cmd(SPI1, ENABLE);
	
	// SS默认拉高，总线空闲，不选中任何从机
	MySPI_W_SS(1);
}

// SPI起始条件：拉低SS，选中目标从机，开启一次通信
void MySPI_Start(){
	MySPI_W_SS(0);
}

// SPI终止条件：拉高SS，释放从机，结束本次通信
void MySPI_Stop(){
	MySPI_W_SS(1);
}

// 全双工交换一个字节，发送1字节的同时接收1字节
uint8_t MySPI_SwapByte(uint8_t ByteSend){
	// 等待TXE标志置1，确认发送缓冲区为空，可以写入新数据
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET);
	
	// 写入发送数据到DR寄存器，硬件自动启动发送流程
	SPI_I2S_SendData(SPI1, ByteSend);
	
	// 等待RXNE标志置1，确认当前字节接收完成，接收缓冲区有数据
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);
	
	// 读取接收数据并返回，读取操作自动清除RXNE标志
	return SPI_I2S_ReceiveData(SPI1);
}
