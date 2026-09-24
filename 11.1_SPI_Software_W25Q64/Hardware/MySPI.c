#include<stm32f10x.h>

//=============================
// 软件模拟SPI底层驱动（模式0，CPOL=0 CPHA=0，高位先行）
// 引脚映射：PA4=SS片选、PA5=SCK时钟、PA6=MISO主机输入、PA7=MOSI主机输出
//=============================

/**
 * @brief  控制SS片选引脚电平
 * @param  BitValue: 0=拉低选中从机，1=拉高释放从机
 */
void MySPI_W_SS(uint8_t BitValue){
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)BitValue);
}

/**
 * @brief  控制SCK时钟引脚电平
 * @param  BitValue: 输出到SCK引脚的电平值
 */
void MySPI_W_SCK(uint8_t BitValue){
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, (BitAction)BitValue);
}

/**
 * @brief  控制MOSI数据输出引脚电平
 * @param  BitValue: 主机输出到MOSI线的电平值
 */
void MySPI_W_MOSI(uint8_t BitValue){
	GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)BitValue);
}

/**
 * @brief  读取MISO数据输入引脚电平
 * @retval 从机输出到MISO线的电平值（0/1）
 */
uint8_t MySPI_R_MISO(){
	return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
}

/**
 * @brief  软件模拟SPI初始化
 * @note   配置对应GPIO的输入输出模式，设置总线空闲电平
 *         默认工作在模式0：空闲时SCK为低电平，上升沿采样数据
 */
void MySPI_Init(){
	// 开启GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 配置SS、SCK、MOSI为推挽输出模式
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 配置MISO为上拉输入模式，保证空闲电平稳定
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	MySPI_W_SS(1);	// 总线空闲：SS拉高，默认不选中任何从机
	MySPI_W_SCK(0);	// 模式0空闲电平：SCK保持低电平（CPOL=0）
}

/**
 * @brief  SPI起始条件：拉低SS，选中目标从机，开启一次通信
 */
void MySPI_Start(){
	MySPI_W_SS(0);
}

/**
 * @brief  SPI终止条件：拉高SS，释放从机，结束本次通信
 */
void MySPI_Stop(){
	MySPI_W_SS(1);
}

/**
 * @brief  全双工交换一个字节（模式0时序，高位先行）
 * @param  ByteSend: 主机要发送的字节数据
 * @retval 从机返回的字节数据
 * @note   核心逻辑：SPI收发同步，发送1位的同时接收1位
 *         模式0时序：SCK低电平时更新数据，上升沿采样数据
 */
uint8_t MySPI_SwapByte(uint8_t ByteSend){
	for (uint8_t i = 0; i < 8; i++){				
		MySPI_W_MOSI(ByteSend & 0x80);		// 取出最高位输出到MOSI，SPI高位先行（MSB First）
		ByteSend <<= 1;					// 发送字节左移1位，腾出最低位用于接收从机数据
		MySPI_W_SCK(1);					// SCK上升沿：从机采样主机数据，主机采样从机数据（CPHA=0）
		ByteSend |= MySPI_R_MISO();			// 读取MISO电平，存入ByteSend的最低位
		MySPI_W_SCK(0);					// SCK下降沿：更新下一位数据，为下一个周期做准备
	}
	return ByteSend;
}
