#include<stm32f10x.h>

void Encoder_Init()
{
    // 开启TIM3外设时钟（TIM3挂载在APB1总线）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    // 开启GPIOA端口时钟，PA6、PA7作为编码器A/B相输入
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // GPIO初始化配置：PA6(TIM3_CH1)、PA7(TIM3_CH2) 正交编码器输入
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;          // 上拉输入，适配编码器开漏输出
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // PA6 = TI1(A相)，PA7 = TI2(B相)
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;      // 输入模式下速率参数可保留，规范写法
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 定时器时基单元配置
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;    // 定时器时钟分频，用于数字滤波器时钟
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;// 编码器模式下硬件自动切换增减方向，此设置会被覆盖
    TIM_TimeBaseInitStruct.TIM_Period = 65536 - 1;              // ARR=0xFFFF，16位定时器最大计数上限，拓宽计数范围
    TIM_TimeBaseInitStruct.TIM_Prescaler = 1 - 1;               // PSC预分频器【编码器模式无效】，仅作占位
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;           // 重复计数器，仅高级定时器有效，通用定时器无需使用
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

    // 输入捕获通道初始化（TI1、TI2，为编码器接口提供信号通路与数字滤波）
    TIM_ICInitTypeDef TIM_ICInitStruct;
    TIM_ICStructInit(&TIM_ICInitStruct);         // 将输入捕获结构体填充默认参数

    // 配置通道1(TIM3_CH1/PA6，编码器A相 TI1)
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;    // 初始极性，后续Encoder接口函数会重新覆盖
    TIM_ICInitStruct.TIM_ICFilter = 0xF;                        // 数字滤波系数0xF，滤除机械抖动、高频毛刺
    // 下面两项参数在编码器模式下不生效，仅做初始化占位
    // TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    // TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM3, &TIM_ICInitStruct);

    // 配置通道2(TIM3_CH2/PA7，编码器B相 TI2)
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;    // 初始极性，后续Encoder接口函数会重新覆盖
    TIM_ICInitStruct.TIM_ICFilter = 0xF;                        // 数字滤波系数0xF
    TIM_ICInit(TIM3, &TIM_ICInitStruct);

    // 编码器接口模式配置
    // TIM_EncoderMode_TI12：TI1与TI2双边沿计数，4倍频模式（工程最常用）
    // 后两个参数：TI1、TI2信号极性，Rising=信号不反向；如需调换正反转可改为Falling
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    TIM_Cmd(TIM3, ENABLE);		// 使能定时器计数器，CNT开始根据编码器信号自动增减计数
}

int16_t Encoder_GetCNT(){
	return TIM_GetCounter(TIM3);	//获取CNT寄存器值
}

int16_t Encoder_GetSpeed(){
	int16_t Temp;
	Temp = TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3, 0);
	return Temp;
}
