#include<stm32f10x.h>
#include"Delay.h"
uint16_t cnt = 0;
void IS_Init(){
	//开启GPIOB和AFIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//因为EXTI和NVIC的时钟默认开启，所以不需要手动再开。

	//初始化GPIOB
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//利用AFIO将PB14端口绑定到EXTI通道上
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);	
	
	//EXTI初始化
	
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line14; 	//PB14端口和EXTI14通道绑定
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;		//启用中断
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;	//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;		//双边沿模式，检测更稳定，次数需/2

	EXTI_Init(&EXTI_InitStruct);
	
	//NVIC初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		//设置为第二分组，更均衡
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;	//设置抢占优先级为1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;		//设置相应优先级为1
	
	NVIC_Init(&NVIC_InitStruct);
}

//在启动文件starup_stm32f10x_md.s
//找IRQHandler结尾的有对应中断通道名称的名字就是指定的中断处理函数名称
void EXTI15_10_IRQHandler(){	//处理EXTI10-15端口的中断函数，无参无返回值
	//判断EXTI14中断标志，确保是14端口触发的中断函数
	if (EXTI_GetITStatus(EXTI_Line14) == SET)	
	{
		cnt++;
		Delay_ms(20);	//消抖，检测更稳定，防止一次多遍计数
		//很重要！！复位中断标志，防止一直重复触发中断
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}

uint16_t GetCnt(){
	return cnt;
}

