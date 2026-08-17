#include<stm32f10x.h>

void PWM_Init(){
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//TIM2在APB1总线上！！！
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//PA0是CH1输出端口
	
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
//	GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);	//利用AFIO的重映射将TIM2的CH1映射到PA15
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//解除JTAG在PA15的映射，恢复PA15的GPIO功能
	
	//初始化GPIO
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;			//复用推挽，将引脚电平控制权交给TIM
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;					//CH1映射到PA15了 	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//滤波采样时钟分频参数
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; 	//向上计数	
	//驱动舵机周期为20ms
	TIM_TimeBaseInitStruct.TIM_Period = 20000 - 1;					//设置ARR寄存器 计数器目标值   0-65535
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;					//PSC寄存器 预分频取值	0-65535
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				//高级计时器中重复计时器参数，不用，写0
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);	
	 
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;				//设置为PWM1比较模式(CNT<CCR输出有效电平)
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;		//有效电平设置为高电平
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//开启OC输出（TIM2 CH1输出通道在PA0）
	TIM_OCInitStruct.TIM_Pulse = 500;							//设置CCR寄存器
	//其他结构体成员是高级计时器的参数，通用计时器不用填，TIM_OCStructInit已初始化
//	TIM_OC1Init(TIM2, &TIM_OCInitStruct);	//通道1初始化
	TIM_OC2Init(TIM2, &TIM_OCInitStruct);	//通道2初始化
	

	
	TIM_Cmd(TIM2, ENABLE);	//允许计数器CNT开始计数
	
}

void PWM_SetCompare1(uint16_t Compare){
	TIM_SetCompare1(TIM2, Compare);
}

void PWM_SetCompare2(uint16_t Compare){
	TIM_SetCompare2(TIM2, Compare);
}
