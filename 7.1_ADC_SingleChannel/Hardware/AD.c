#include<stm32f10x.h>

void AD_Init(){
	//开启ADC和GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//对ADC时钟进行分频
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//配置GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//规则组采样通道配置
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);		//可填充多个菜单列表
	
	//初始化配置ADC
	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;						//单ADC独立模式
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;					//数据右对齐
	ADC_InitStruct.ADC_ScanConvMode	= DISABLE;							//非扫描模式
	ADC_InitStruct.ADC_NbrOfChannel = 1;								//通道数目
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//不使用外部触发，使用内部软件触发
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;					//非连续转换模式
	//也可以切换为连续转换，这样就不用手动软件开始转换了，直接读标志位然后取值就行
	ADC_Init(ADC1, &ADC_InitStruct);
	
	//开启ADC电源
	ADC_Cmd(ADC1, ENABLE);			
	
	//校准ADC
	ADC_ResetCalibration(ADC1);		//开始复位校准，将寄存器中复位校准状态置SET(1)
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);		//启动校准，寄存器对应位置置SET(1)
	while (ADC_GetCalibrationStatus(ADC1) == SET);
}

uint16_t AD_GetValue(){
	 ADC_SoftwareStartConvCmd(ADC1, ENABLE);					//手动软件开始转换
	 while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);		//等待转换完成，EOC标志位置SET(未转换完成RESET)
	 return ADC_GetConversionValue(ADC1);
}