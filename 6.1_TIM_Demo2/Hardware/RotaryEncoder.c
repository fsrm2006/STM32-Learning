#include<stm32f10x.h>

int16_t Encoder_cnt = 0;
void REn_Init(){
	//开启GPIOB和AFIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//因为EXTI和NVIC的时钟默认开启，所以不需要手动再开。

	//初始化GPIOB
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//利用AFIO将PB0 PB1端口绑定到EXTI通道上
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	
	//EXTI初始化
	
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line1; 	//PB0 & 1端口和EXTI0 & 1通道绑定
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;		//启用中断
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;		//下降沿

	EXTI_Init(&EXTI_InitStruct);
	
	//NVIC初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//设置为第二分组，更均衡
	
	NVIC_InitTypeDef NVIC_InitStruct;
	//设置PB0端口中断的优先级
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;	//设置抢占优先级为1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;		//设置相应优先级为1
	NVIC_Init(&NVIC_InitStruct);
	
	//设置PB1端口中断的优先级
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;	//设置抢占优先级为1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;		//设置相应优先级为2
	NVIC_Init(&NVIC_InitStruct);
}

////在启动文件starup_stm32f10x_md.s中找对应中断函数的函数名
void EXTI0_IRQHandler(){	//A电平下降，检测是否正转
	if (EXTI_GetITStatus(EXTI_Line0) == SET){
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)	//A电平先下降，B电平滞后下降为正转
			Encoder_cnt++;
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
	
}

void EXTI1_IRQHandler(){	//B电平下降，检测是否反转
	if (EXTI_GetITStatus(EXTI_Line1) == SET){
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1)	//B电平先下降，A电平滞后下降为反转
			Encoder_cnt--;
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

int16_t GetREnCnt(){
	return Encoder_cnt;
}

