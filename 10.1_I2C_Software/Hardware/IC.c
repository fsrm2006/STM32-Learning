#include<stm32f10x.h>

void IC_init(void)
{
	// 开启TIM3时钟（TIM3挂载在APB1总线）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	// 开启GPIOA时钟，PA6为TIM3_CH1复用输入引脚
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// GPIO初始化 PA6 上拉输入
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;        // 上拉输入模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;            // PA6
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    // 输入模式下速度参数可保留
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	// 定时器使用内部时钟作为计数时钟
	TIM_InternalClockConfig(TIM3);

	// 时基单元配置
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;       // 数字滤波时钟分频1倍
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;   // 向上计数模式
	TIM_TimeBaseInitStruct.TIM_Period = 65536 - 1;                 // ARR最大值，防止计数溢出
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;                 // PSC预分频：72MHz/(72)=1MHz，1us计数一次
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;              // 通用定时器无重复计数器，固定填0
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);


	// 输入捕获通道1配置
	TIM_ICInitTypeDef TIM_ICInitStruct;
	TIM_ICStructInit(&TIM_ICInitStruct);                           // 结构体赋默认值
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;                  // 使用通道1
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;       // 上升沿触发捕获
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;             // 捕获信号不分频，每有效边沿触发一次
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;   // 直连模式：捕获源=TI1(PA6)
	TIM_ICInitStruct.TIM_ICFilter = 0xF;                           // 数字滤波，抑制高频毛刺
	TIM_ICInit(TIM3, &TIM_ICInitStruct);
	
	//PWMI配置方法1，分别配置
	//同时配置另一个通道
//	TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;                  // 使用通道2
//	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;      // 下降沿触发捕获
//	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;             // 捕获信号不分频，每有效边沿触发一次
//	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_InDirectTI;  //间接连接模式：CH2捕获源=TI1(PA6)
//	TIM_ICInitStruct.TIM_ICFilter = 0xF;                           // 数字滤波，抑制高频毛刺
//	TIM_ICInit(TIM3, &TIM_ICInitStruct);

	//PWMI配置方法2，用TIM_PWMIConfig函数快捷配置，TIM_Channel自动取另一个，TIM_ICFilter不变，其它参数自动取反
	TIM_PWMIConfig(TIM3, &TIM_ICInitStruct);
	
	// 从模式配置【核心：复位模式测频率】
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);                   // 选择TI1滤波后信号作为触发源
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);                // 复位模式：触发边沿到来，CNT立即清零

	TIM_Cmd(TIM3, ENABLE);                                         // 使能TIM3计数器开始计数
}
uint32_t IC_GetFreq(){					//测周法测频率
	//CNT计数频率1MHz，捕获值N+1代表周期个数
	return 1000000 / (TIM_GetCapture1(TIM3) + 1);					//Freq = fc / N
}

uint16_t IC_GetDuty(){					//算占空比
	//这里好像因为CCR都会少记一次所以加1
	return 100 * (TIM_GetCapture2(TIM3)+1) / (TIM_GetCapture1(TIM3)+1);		//Duty = CCR2 / CCR1 * 100%
}
