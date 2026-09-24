#include<stm32f10x.h>

uint16_t AD[4];	// 存放4路ADC采样结果的数组

void AD_Init(void)
{
	// 开启ADC1外设时钟、GPIOA端口时钟；PA0‑PA3为ADC1通道0~3模拟输入引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// ADC内核转换时钟配置：PCLK2 6分频，72MHz /6 =12MHz，满足ADCCLK最大14MHz硬件限制
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	// GPIO初始化 PA0 PA1 PA2 PA3：模拟输入模式，关闭数字电路，防止干扰模拟采样
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;         // 模拟输入模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // 模拟输入下Speed无实际作用，语法保留
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// 配置ADC规则组转换序列：4个通道，指定各自的序列序号与采样时间
	// Rank：转换序列次序；采样时间28.5周期，抗干扰更好
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_28Cycles5);

	// ADC初始化结构体配置
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;						// 独立ADC模式，不使用双ADC同步
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;					// 数据右对齐，低12位为有效采样值0~4095
	ADC_InitStruct.ADC_ScanConvMode	= ENABLE;							// 开启扫描模式，按序列依次转换多路通道
	ADC_InitStruct.ADC_NbrOfChannel = 4;								// 规则组序列总通道数为4
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	// 关闭外部触发，使用软件触发ADC
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;						//开启连续转换
	ADC_Init(ADC1, &ADC_InitStruct);

	// ========= DMA配置：ADC1硬件触发DMA1通道1，外设→存储器 =========
	// 开启DMA1时钟，DMA挂载AHB总线
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitTypeDef DMA_InitStruct;

	// 外设端：源 = ADC1数据寄存器DR，地址固定不变
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;			// 源地址：ADC数据寄存器
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	// 源宽度：16位半字，ADC_DR是16位
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			// 外设寄存器地址固定，禁止地址自增

	// 存储器端：目标 = SRAM数组AD，用来存放采样结果
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)AD;						// 目标地址：采样结果数组首地址
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;		// 目标宽度：16位半字，与源保持一致
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;					// 数组需要依次存储，开启存储器地址自增

	DMA_InitStruct.DMA_BufferSize = 4;					// NDTR初始值：一轮总共搬运4个数据，对应4个ADC通道
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;		// 传输方向：外设作为源，搬运到存储器
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;			// 关闭M2M模式，使用ADC硬件事件触发DMA，不是软件内存拷贝
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;		// 开启NDTR自动重装，持续搬运数据
	DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;	// DMA通道仲裁优先级

	DMA_Init(DMA1_Channel1, &DMA_InitStruct);	// 将配置写入DMA1通道1（硬件绑定ADC1）
	DMA_Cmd(DMA1_Channel1, ENABLE);			// 使能DMA通道，等待ADC硬件触发信号

	ADC_DMACmd(ADC1, ENABLE);		// 使能ADC的DMA输出请求：ADC转换完成就会发DMA请求

	// 使能ADC外设，上电后才能执行校准
	ADC_Cmd(ADC1, ENABLE);

	// ---------------- ADC上电硬件自校准，上电必须执行一次 ----------------
	ADC_ResetCalibration(ADC1);						// 触发复位校准
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);// 等待复位校准完成

	ADC_StartCalibration(ADC1);						// 启动ADC自校准
	while (ADC_GetCalibrationStatus(ADC1) == SET);		// 等待自校准完成
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);				// 开启ADC转换
}


////ADC单次扫描时，启动一轮4通道ADC扫描采集，阻塞等待DMA全部搬运完成
//void AD_GetValue(void)
//{
//	/*
//	DMA工作三条件：
//	1.DMA通道使能； 2.NDTR传输计数器>0； 3.收到触发信号(此处由ADC转换完成硬件触发)
//	普通模式一轮结束NDTR变为0，即使DMA使能也不会响应触发；修改NDTR必须先关闭DMA
//	*/
//	DMA_Cmd(DMA1_Channel1, DISABLE);					// 修改NDTR前必须关闭DMA通道
//	DMA_SetCurrDataCounter(DMA1_Channel1, 4);			// 重置传输计数器，一轮搬运4个数据
//	DMA_Cmd(DMA1_Channel1, ENABLE);						// 重新使能DMA，等待ADC触发

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);				// 软件触发ADC，开始一轮4通道扫描转换

//	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);	// 阻塞等待DMA把4个数据全部搬运完毕，硬件TC1置SET
//	DMA_ClearFlag(DMA1_FLAG_TC1);						// TC标志硬件不会自动清零，软件手动清除，为下一轮做准备
//}