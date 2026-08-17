#include<stm32f10x.h>

void IC_init(){
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//TIM3在APB1总线上！！！
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//PA6作为IC的输入端口
	
	
	//初始化GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;				//输入模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;					//PA6作为输入捕获的输入源	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	TIM_InternalClockConfig(TIM3);
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//滤波采样时钟分频参数
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; 	//向上计数		
	TIM_TimeBaseInitStruct.TIM_Period = 65536 - 1;					//ARR 尽可能大一些防止CNT溢出
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;					//PSC寄存器 预分频取值	0-65535
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				//高级计时器中重复计时器参数，不用，写0
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);	
	
	//输入捕获配置
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICStructInit(&TIM_ICInitStruct);							//初始化TIM_ICInitStruct里面的值
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;					//要初始化的CH通道
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;		//触发的极性（上升沿触发）
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;				//触发信号分频器（不分频）
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;	//直连模式，触发信号直接来自当前通道
	TIM_ICInitStruct.TIM_ICFilter = 0xF;							//滤波参数 0x0 - 0xF
	TIM_ICInit(TIM3, &TIM_ICInitStruct);							//初始化IC通道
		
	//设置从触发源和从模式
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);					//设置从触发源
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);					//设置从模式为清零CNT
	
	TIM_Cmd(TIM3, ENABLE);	//允许计数器CNT开始计数
}

uint32_t IC_GetFreq(){
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);					//+1是有可能少记一个
}
