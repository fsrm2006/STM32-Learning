#include<stm32f1xx_hal.h>
//#include "spi.h"
#ifdef __SPI_H__
// 硬件SPI底层驱动（SPI1，模式0：CPOL=0、CPHA=1Edge，高位先行）
// 引脚映射：PA4=SS片选（普通GPIO软件控制，非硬件NSS）、PA5=SCK时钟、PA6=MISO主机输入、PA7=MOSI主机输出

// 控制SS片选引脚电平：PinState=0 拉低选中从机，PinState=1 拉高释放从机
void MySPI_W_SS(uint8_t PinState){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, (GPIO_PinState)PinState);
}

// SPI总线初始化：仅将SS置为空闲高电平，外设本身由MX_SPI1_Init完成
void MySPI_Init(){
	// SS拉高，总线空闲时不选中任何从机
	MySPI_W_SS(1);
}

// SPI起始：拉低SS选中从机，开启一次通信
void MySPI_Start(){
	MySPI_W_SS(0);
}

// SPI结束：拉高SS释放从机，本次通信终止
void MySPI_Stop(){
	MySPI_W_SS(1);
}

// 全双工交换一个字节：发送TxData的同时接收1字节，返回接收值
uint8_t MySPI_SwapByte(uint8_t TxData){
	// 阻塞式收发，1字节收发完成即返回
	uint8_t Rx_Data = 0;
	HAL_SPI_TransmitReceive(&hspi1, &TxData, &Rx_Data, 1, 100);	
	return Rx_Data;
}
#endif