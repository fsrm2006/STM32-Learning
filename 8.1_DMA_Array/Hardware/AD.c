#include<stm32f10x.h>

void AD_Init(void)
{
	// 开启ADC1外设时钟、GPIOA端口时钟；PA0为ADC1通道0模拟输入引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// ADC内核转换时钟配置：PCLK2 6分频，72MHz /6 =12MHz，满足ADCCLK最大14MHz硬件限制
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	// GPIO初始化 PA0：模拟输入模式，数字输入输出电路关闭，避免干扰模拟采样
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;         // 模拟输入模式
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;     // 模拟输入下Speed参数保留，无实际作用
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; // PA0 1 2 3 对应ADC1_IN1 2 3
	GPIO_Init(GPIOA, &GPIO_InitStruct);

//	// 配置ADC规则组通道
//	// 参数：ADC外设，通道号，序列次序Rank=1，采样时间28.5个ADC时钟周期
//	// 采样时间越大抗干扰越强，但转换耗时增加；多通道可多次调用此函数添加多条转换序列
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);

	// ADC初始化结构体配置
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;						// 独立模式，只用ADC1，不使用ADC1+ADC2双ADC同步模式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;					// 转换结果右对齐，有效数据存低12位，直接读取0~4095无需移位
	ADC_InitStruct.ADC_ScanConvMode	= DISABLE;							// 关闭扫描模式：只转换序列里第1个通道，适合单通道采集
	ADC_InitStruct.ADC_NbrOfChannel = 1;								// 规则组序列通道数量，Scan关闭时该参数不起作用，填写1即可
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		// 关闭外部硬件触发，使用软件触发转换
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;					// 关闭连续转换：一次触发只完成1次转换，转换结束自动停止
	// 若开启连续转换模式，触发一次后会循环不停采样，无需每次调用SoftwareStartConvCmd，注意及时读取防止数据覆盖
	ADC_Init(ADC1, &ADC_InitStruct);

	// 使能ADC外设供电，ADC上电之后才可以执行校准操作
	ADC_Cmd(ADC1, ENABLE);			

	// ---------------- ADC上电自校准流程，消除内部硬件偏移误差，上电必须执行一次 ----------------
	ADC_ResetCalibration(ADC1);		// 触发复位校准，硬件将复位校准状态标志置SET
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);	// 阻塞等待复位校准完成，标志硬件自动清零变为RESET

	ADC_StartCalibration(ADC1);		// 启动ADC硬件自校准，硬件将校准状态标志置SET
	while (ADC_GetCalibrationStatus(ADC1) == SET);		// 阻塞等待自校准运算全部完成，标志硬件自动清零变为RESET
}

uint16_t AD_GetValue(uint8_t ADC_Channel){
	 ADC_RegularChannelConfig(ADC1, ADC_Channel, 1, ADC_SampleTime_28Cycles5);		//每次读取时手动指定转换的通道，实现多通道传感器的测量
	 ADC_SoftwareStartConvCmd(ADC1, ENABLE);					//手动软件开始转换
	 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);		//等待转换完成，EOC标志位置SET(未转换完成RESET)
	 return ADC_GetConversionValue(ADC1);
}