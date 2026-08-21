#include<stm32f10x.h>

uint16_t size;

void MyDMA_Init(uint32_t Dataptr, uint32_t Desptr, uint16_t Size){		//要搬运的数据的地址，目标地址
	size = Size;	//给全局变量size赋值
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			//DMA在AHB总线上！
	
	DMA_InitTypeDef DMA_InitStruct;
	//外设站点配置
	DMA_InitStruct.DMA_PeripheralBaseAddr = Dataptr;						//起始地址
	//Byte - uint8_t  HalfWord - uint16_t  Word - uint32_t
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//数据宽度（1字节）
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;			//地址是否自增，数组间转运需要自增
	//存储器站点配置
	DMA_InitStruct.DMA_MemoryBaseAddr = Desptr;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	
	DMA_InitStruct.DMA_BufferSize = size;					//缓冲区大小(传输计数器大小，指定传输次数) 0~65535
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;			//传输方向 SRC:外设->内存; DST:内存->外设
	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;				//选择硬件自动触发还是软件触发（软件触发）
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;				//传输模式是否使用自动重装 (否)
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;		//优先级(多个通道时的优先级)
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	
	//DMA使能
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
}

void MyDMA_Transfer(){
	//DMA工作的三大条件：传输计数器>0, 有触发信号, DMA使能（RCC时钟也要开启）
	//转运完传输计数器=0，手动置为>0即可重新开始
	//传输计数器赋值一定要先失能DMA再赋值！！！
	DMA_Cmd(DMA1_Channel1, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel1, size);	//设置传输寄存器值为size
	DMA_Cmd(DMA1_Channel1, ENABLE);					//DMA使能
	
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);	//等待转运完成，硬件置DMA1_FLAG_TC1为SET
	DMA_ClearFlag(DMA1_FLAG_TC1);		//将FLAG手动置RESET，为下一次传输做准备
}
