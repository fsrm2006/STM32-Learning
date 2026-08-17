#include<stm32f10x.h>

extern uint16_t Num;
void Timer_Init(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//TIM2在APB1总线上！！！
	
	TIM_InternalClockConfig(TIM2);	//用内部时钟作为时钟源，默认内部时钟，不写也行

	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//滤波采样时钟分频参数
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; 		//向上计数		
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;			//ARR计数器目标值   0-65535
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;		//PSC时钟分频取值	0-65535
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;	//高级计时器中重复计时器参数，不用，写0
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);	
	
	//TIM_TimeBaseInit函数的最后将中断标志位置为1，程序一开始就会进中断，所以要把中断标志位置为0
	TIM_ClearFlag(TIM2, TIM_IT_Update);	
	//选择中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//TTIM2，更新中断，使能中断
	
	//NVIC配置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;	//TIM2在NVIC的通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	//开启中断
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;			//相应优先级
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2, ENABLE);	//允许计数器CNT开始计数
	
}

//函数名在启动文件startupxxx.md.s里面找
void TIM2_IRQHandler(){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)	//检查中断标志位
	{
		Num++;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
	
}
