#include<stm32f10x.h>

uint16_t size;	//保存需要搬运的数据个数，全局变量

void MyDMA_Init(uint32_t Dataptr, uint32_t Desptr, uint16_t Size)
{
	size = Size;	//把要搬运的数据数量存入全局变量

	//开启DMA1外设时钟，DMA挂载在AHB总线上，注意不是APB！
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitTypeDef DMA_InitStruct;

	// ==========外设端配置：M2M模式下，这里作为【源】==========
	DMA_InitStruct.DMA_PeripheralBaseAddr = Dataptr;						//源数据起始地址
	// Byte=8位(uint8_t)  HalfWord=16位(uint16_t)  Word=32位(uint32_t)
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//源端单次搬运数据宽度：1字节
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;			//源地址自增，数组拷贝必须打开

	// ==========存储器端配置：M2M模式下，这里作为【目标】==========
	DMA_InitStruct.DMA_MemoryBaseAddr = Desptr;								//目标数据起始地址
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			//目标端单次搬运数据宽度：1字节
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;					//目标地址自增，数组拷贝必须打开

	DMA_InitStruct.DMA_BufferSize = size;					//传输计数器NDTR初始值，代表总共要搬运多少次，范围0~65535
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;			//传输方向：Peripheral作为源 → Memory作为目标(M2M模式固定这个配置)
	DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;				//开启存储器到存储器模式：开启后内部软件自动产生触发信号，不需要外设硬件请求
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;				//普通模式，不开启自动重装；搬运完指定次数后NDTR变为0，停止工作
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;		//通道优先级，多通道同时请求时仲裁使用；单通道影响不大

	DMA_Init(DMA1_Channel1, &DMA_InitStruct);	//把配置写入DMA1通道1寄存器

	DMA_Cmd(DMA1_Channel1, ENABLE);				//使能DMA通道，此时等待触发信号
}

